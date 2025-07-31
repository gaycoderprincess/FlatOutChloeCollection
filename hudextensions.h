namespace NewMenuHud {
	bool bInCarDealer = false;
	bool bInSkinSelector = false;
	int nCarHorsepower = 0;
	int nCarWeight = 0;
	int nCarPrice = 0;
	std::string sCarDescription;

	IDirect3DTexture9* LoadTextureFromBFS(const char* path) {
		if (auto tex = pDeviceD3d->_vf_CreateTextureFromFile(nullptr, path, 9)) {
			return tex->pD3DTexture;
		}
		return nullptr;
	}

	enum eJustify {
		JUSTIFY_LEFT,
		JUSTIFY_CENTER,
		JUSTIFY_RIGHT
	};

	void DoJustify(eJustify justify, float& x, float& y) {
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

	bool Draw1080pSprite(eJustify justify, float left, float right, float top, float bottom, NyaDrawing::CNyaRGBA32 rgb, TEXTURE_TYPE* texture) {
		DoJustify(justify, left, top);
		DoJustify(justify, right, bottom);
		return DrawRectangle(left, right, top, bottom, rgb, 0, texture);
	}

	void Draw1080pString(eJustify justify, tNyaStringData data, const std::string& text, void(*drawFunc)(const tNyaStringData&, const std::string&)) {
		DoJustify(justify, data.x, data.y);
		return DrawString(data, text, drawFunc);
	}

	int nPowerY = 392;
	int nPriceY = 320;
	int nWeightY = 466;
	int nOffsetY = 38;
	int nBaseX = 160;
	int nOffsetX = 385;
	float fBaseSize = 0.04;
	float fOffsetSize = 0.05;
	int nOffsetPriceY = 30;
	float fOffsetPriceSize = 0.04;

	int nDescriptionX = 160;
	int nDescriptionY = 593;
	float fDescriptionSize = 0.035;

	int nSkinSelectTitleX = 1720;
	//int nSkinSelectTitleY = 255;
	//int nSkinSelectTitleY = 335;
	int nSkinSelectTitleY = 328;
	//float fSkinSelectSize = 0.04;
	float fSkinSelectSize = 0.035;

	struct tHUDData {
		std::string name;
		ImVec2 min;
		ImVec2 max;
	};

	std::vector<tHUDData> LoadHUDData(const char* path, const char* name) {
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

	tHUDData* GetHUDData(std::vector<tHUDData>& hud, const std::string& name) {
		for (auto& data : hud) {
			if (data.name == name) return &data;
		}
		return nullptr;
	}

	float fCarNameX = 0.225;
	float fCarNameY = 0.17;
	float fCarNameSize = 0.06;
	float fCarNameAspect = 5;

	void DrawCarDealer() {
		static auto textureLeft = LoadTextureFromBFS("data/menu/carselect_left.png");
		static auto textureRight = LoadTextureFromBFS("data/menu/carselect_right.png");
		static auto textureArrows = LoadTextureFromBFS("data/menu/carselect_arrows.png");
		static auto textureCarLogos = LoadTextureFromBFS("data/menu/car_logos.dds");
		static std::vector<tHUDData> gCarLogos = LoadHUDData("data/menu/car_logos.bed", "car_logos");

		if (!bInCarDealer) return;
		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);
		if (GetNumSkinsForCar(pGameFlow->pMenuInterface->pMenuScene->nCar) > 6) {
			Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255, 255, 255, 255}, textureArrows);
		}

		if (auto logo = GetHUDData(gCarLogos, std::format("car{}", pGameFlow->pMenuInterface->pMenuScene->nCar))) {
			DrawRectangle(fCarNameX * GetAspectRatioInv(),
						  (fCarNameX + fCarNameSize * fCarNameAspect) * GetAspectRatioInv(), fCarNameY,
						  fCarNameY + fCarNameSize, {255, 255, 255, 255}, 0, textureCarLogos, 0, logo->min,
						  logo->max);
		}

		tNyaStringData data;
		data.x = nBaseX;
		data.y = nPowerY;
		data.size = fBaseSize;
		Draw1080pString(JUSTIFY_LEFT, data, "POWER", &DrawStringFO2_Ingame12);
		data.y = nWeightY;
		Draw1080pString(JUSTIFY_LEFT, data, "WEIGHT", &DrawStringFO2_Ingame12);
		data.y = nPriceY;
		Draw1080pString(JUSTIFY_LEFT, data, "PRICE", &DrawStringFO2_Ingame12);
		data.x = nOffsetX;
		data.y = nPowerY + nOffsetY;
		data.size = fOffsetSize;
		Draw1080pString(JUSTIFY_LEFT, data, std::format("{}hp", nCarHorsepower), &DrawStringFO2_Ingame12);
		data.y = nWeightY + nOffsetY;
		Draw1080pString(JUSTIFY_LEFT, data, std::format("{}kg", nCarWeight), &DrawStringFO2_Ingame12);
		data.y = nPriceY + nOffsetPriceY;
		data.size = fOffsetPriceSize;
		Draw1080pString(JUSTIFY_LEFT, data, std::format("${}", nCarPrice), &DrawStringFO2_Small);
		data.x = nDescriptionX;
		data.y = nDescriptionY;
		data.size = fDescriptionSize;
		Draw1080pString(JUSTIFY_LEFT, data, sCarDescription, &DrawStringFO2_Ingame12);
		data.x = nSkinSelectTitleX;
		data.y = nSkinSelectTitleY;
		data.size = fSkinSelectSize;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, "SKINS", &DrawStringFO2_Small);
	}

	std::string GetSkinName(int carId, int skinId, bool wrapAround) {
		static auto config = toml::parse_file("Config/CarSkins.toml");
		int numSkins = GetNumSkinsForCar(carId);
		if (!wrapAround && (skinId < 1 || skinId > numSkins)) {
			return "---";
		}
		// wrap around
		while (skinId < 1) {
			skinId += numSkins;
		}
		while (skinId > numSkins) {
			skinId -= numSkins;
		}
		std::string string = config["car_" + std::to_string(carId)]["skin" + std::to_string(skinId) + "name"].value_or("");
		if (string.empty()) string = "Skin " + std::to_string(skinId);
		return string;
	}

	std::string GetSkinAuthor(int carId, int skinId, bool wrapAround) {
		static auto config = toml::parse_file("Config/CarSkins.toml");
		int numSkins = GetNumSkinsForCar(carId);
		if (!wrapAround && (skinId < 1 || skinId > numSkins)) {
			return "---";
		}
		// wrap around
		while (skinId < 1) {
			skinId += numSkins;
		}
		while (skinId > numSkins) {
			skinId -= numSkins;
		}
		std::string string = config["car_" + std::to_string(carId)]["skin" + std::to_string(skinId)].value_or("");
		if (!string.empty()) string = "Skin Author: " + std::to_string(skinId);
		return string;
	}

	void DrawSkinSelector() {
		if (!bInSkinSelector) return;

		auto menu = pGameFlow->pMenuInterface;
		if (!menu) return;
		if (!menu->pMenuScene) return;

		int car = menu->pMenuScene->nCar;
		int skin = menu->pMenuScene->nCarSkin;
		int numSkins = GetNumSkinsForCar(car);

		bool skinLoop = false;
		if (numSkins > 6) {
			skinLoop = true;
		}
		else {
			skinLoop = false;
		}

		float fPosX = 0.1;
		float fPosY = 0.372;
		float fSize = 0.035;
		float fSpacing = 0.0495;

		tNyaStringData data;
		data.x = 1.0 - (fPosX * GetAspectRatio());
		data.y = fPosY;
		data.size = fSize;
		for (int i = 1; i <= 6; i++) {
			int skinId = 0;
			if (numSkins > 6) {
				skinId = skin - 3 + i;
			}
			else {
				skinId = i;
			}

			if (skinId == skin) {
				data.SetColor(GetPaletteColor(18));
			}
			else {
				data.SetColor(GetPaletteColor(17));
			}

			DrawString(data, GetSkinName(car, skinId, skinLoop), &DrawStringFO2_Ingame12);
			data.y += fSpacing;
		}
	}

	void OnTick() {
		DrawCarDealer();
		DrawSkinSelector();
	}
}