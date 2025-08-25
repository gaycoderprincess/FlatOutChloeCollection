class CHUDElement {
public:
	bool bEnabled = false;

	struct tDrawPositions {
		float fPosX;
		float fPosY;
		float fSize;
		float fSpacingX;
		float fSpacingY;
	};

	struct tDrawPositions1080p {
		int nPosX;
		int nPosY;
		float fSize;
		int nSpacingX;
		int nSpacingY;
	};

	struct tLoadedTexture {
		std::string path;
		DevTexture* devTexture;
		IDirect3DTexture9* texture;
	};
	static inline std::vector<tLoadedTexture> aLoadedTextures;

	static inline std::mutex gLoadedTextureMutex;

	static IDirect3DTexture9* LoadTextureFromBFS(const char* path) {
		gLoadedTextureMutex.lock();
		for (auto& tex : aLoadedTextures) {
			if (tex.path == path) {
				gLoadedTextureMutex.unlock();
				return tex.texture;
			}
		}
		gLoadedTextureMutex.unlock();

		if (auto tex = pDeviceD3d->_vf_CreateTextureFromFile(nullptr, path, 9)) {
			gLoadedTextureMutex.lock();
			aLoadedTextures.push_back({path, tex, tex->pD3DTexture});
			gLoadedTextureMutex.unlock();
			return tex->pD3DTexture;
		}
		return nullptr;
	}

	static IDirect3DTexture9* LoadTextureFromMemory(std::string path, uint8_t* data, size_t dataSize) {
		// fix header
		if (path.ends_with(".dds") && data[0x4C] == 0x18) {
			data[0x4C] = 0x20;
		}

		IDirect3DTexture9* tex = nullptr;
		auto hr = D3DXCreateTextureFromFileInMemory(pDeviceD3d->pD3DDevice, data, dataSize, &tex);
		if (hr == S_OK) {
			gLoadedTextureMutex.lock();
			aLoadedTextures.push_back({path, nullptr, tex});
			gLoadedTextureMutex.unlock();
			return tex;
		}
		return nullptr;
	}

	static inline std::vector<std::string> aTexturePreloadList;
	static void PreloadTexture(const std::string& path) {
		for (auto& preload : aTexturePreloadList) {
			if (path == preload) return;
		}
		aTexturePreloadList.push_back(path);

		size_t dataSize = 0;
		auto file = ReadTextureDataFromFile(path.c_str(), &dataSize);
		if (dataSize <= 0x4C) return;
		std::thread(LoadTextureFromMemory, path, file, dataSize).detach();
	}

	enum eJustify {
		JUSTIFY_LEFT,
		JUSTIFY_CENTER,
		JUSTIFY_RIGHT
	};

	static void DoJustify(eJustify justify, float& x, float& y) {
		switch (justify) {
			case JUSTIFY_LEFT:
			default:
				x /= 1920.0;
				y /= 1080.0;

				// aspect correction
				x *= 16.0 / 9.0;
				x /= GetAspectRatio();
				break;
			case JUSTIFY_CENTER:
				x -= 960;
				x /= 1920.0;
				y /= 1080.0;

				// aspect correction
				x *= 16.0 / 9.0;
				x /= GetAspectRatio();

				// recenter
				x += 0.5;
				break;
			case JUSTIFY_RIGHT:
				x = 1920 - x;
				x /= 1920.0;
				y /= 1080.0;

				// aspect correction
				x *= 16.0 / 9.0;
				x /= GetAspectRatio();

				// recenter
				x = 1 - x;
				break;
		}
	}

	static bool Draw1080pSprite(eJustify justify, float left, float right, float top, float bottom, NyaDrawing::CNyaRGBA32 rgb, TEXTURE_TYPE* texture) {
		DoJustify(justify, left, top);
		DoJustify(justify, right, bottom);
		return DrawRectangle(left, right, top, bottom, rgb, 0, texture);
	}

	static void Draw1080pString(eJustify justify, tNyaStringData data, const std::string& text, void(*drawFunc)(const tNyaStringData&, const std::string&)) {
		DoJustify(justify, data.x, data.y);
		return DrawString(data, text, drawFunc);
	}

	static void Draw1080pStringBottomMost(eJustify justify, tNyaStringData data, const std::string& text, void(*drawFunc)(const tNyaStringData&, const std::string&)) {
		DoJustify(justify, data.x, data.y);
		drawFunc(data, text);
	}

	static float GetFlashingAlpha(float timer) {
		return (0.75 + 0.25 * (std::cos(timer * 3.141592 * 2) + 1.0) / 2) * 255;
	}

	struct tHUDData {
		std::string name;
		ImVec2 min;
		ImVec2 max;
	};

	static std::vector<tHUDData> LoadHUDData(const char* path, const char* name) {
		size_t size;
		auto file = (char*)ReadFileFromBfs(path, size);
		if (!file) return {};

		std::vector<tHUDData> vec;

		auto ss = std::stringstream(file);

		int sizes[2] = {1,1};

		for (std::string line; std::getline(ss, line); ) {
			while (line[0] == '\t') line.erase(line.begin());
			if (line.starts_with("--")) continue;
			if (line.length() > 64) continue;

			if (line.starts_with(std::format("{}_size = ", name))) {
				sscanf(line.c_str(), std::format("{}_size = {{ %d, %d }}", name).c_str(), &sizes[0], &sizes[1]);
			}
			else if (!line.empty() && !line.starts_with(std::format("{} = {{", name))) {
				tHUDData data;
				int x = 0, y = 0, sizex = 0, sizey = 0;

				char varName[64] = "";
				sscanf(line.c_str(), "%s = {  %d, %d, %d, %d, },", varName, &x, &y, &sizex, &sizey);
				if (!varName[0]) continue;
				if (x == 0 && y == 0 && sizex == 0 && sizey == 0) continue;

				data.name = varName;
				data.min.x = x / (double)sizes[0];
				data.min.y = y / (double)sizes[1];
				data.max.x = data.min.x + (sizex / (double)sizes[0]);
				data.max.y = data.min.y + (sizey / (double)sizes[1]);

				vec.push_back(data);
			}
		}

		return vec;
	}

	static tHUDData* GetHUDData(std::vector<tHUDData>& hud, const std::string& name) {
		for (auto& data : hud) {
			if (data.name == name) return &data;
		}
		return nullptr;
	}

	virtual void Init() {}
	virtual void Reset() {}
	virtual void Process() = 0;
};