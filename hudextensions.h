namespace NewMenuHud {
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

	bool bInCareer = false;
	bool bInCareerCupSelect = false;
	bool bInCarDealer = false;
	bool bInSkinSelector = false;
	int nCarHorsepower = 0;
	int nCarWeight = 0;
	int nCarPrice = 0;
	std::string sCarName;
	std::string sCarDescription;

	struct tLoadedTexture {
		std::string path;
		DevTexture* devTexture;
		IDirect3DTexture9* texture;
	};
	std::vector<tLoadedTexture> aLoadedTextures;

	IDirect3DTexture9* LoadTextureFromBFS(const char* path) {
		for (auto& tex : aLoadedTextures) {
			if (tex.path == path) return tex.texture;
		}

		if (auto tex = pDeviceD3d->_vf_CreateTextureFromFile(nullptr, path, 9)) {
			aLoadedTextures.push_back({path, tex, tex->pD3DTexture});
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

	float GetFlashingAlpha(float timer) {
		return (0.75 + 0.25 * (std::cos(timer * 3.141592 * 2) + 1.0) / 2) * 255;
	}

	// todo add interpolation with upgrades
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

	tDrawPositions1080p gCarClass = {160, 805, 0.04};
	tDrawPositions1080p gCarDescription = {160, 593, 0.035};
	tDrawPositions1080p gCarSkinSelectTitle = {1720, 328, 0.035};

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

	tDrawPositions gCarName = {0.225, 0.17, 0.06};
	float fCarNameAspect = 5;

	tDrawPositions1080p gCarNameText = {405, 200, 0.05};

	void DrawCarDealer() {
		static auto textureLeft = LoadTextureFromBFS("data/menu/carselect_left.png");
		static auto textureCarLogos = LoadTextureFromBFS("data/menu/car_logos.dds");
		static std::vector<tHUDData> gCarLogos = LoadHUDData("data/menu/car_logos.bed", "car_logos");

		if (!bInCarDealer) return;
		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);

		if (auto logo = GetHUDData(gCarLogos, std::format("car{}", pGameFlow->pMenuInterface->pMenuScene->nCar))) {
			DrawRectangle(gCarName.fPosX * GetAspectRatioInv(),
						  (gCarName.fPosX + gCarName.fSize * fCarNameAspect) * GetAspectRatioInv(), gCarName.fPosY,
						  gCarName.fPosY + gCarName.fSize, {255, 255, 255, 255}, 0, textureCarLogos, 0, logo->min,
						  logo->max);
		}
		else {
			tNyaStringData data;
			data.x = gCarNameText.nPosX;
			data.y = gCarNameText.nPosY;
			data.size = gCarNameText.fSize;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_LEFT, data, sCarName, &DrawStringFO2_Small);
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
		data.x = gCarDescription.nPosX;
		data.y = gCarDescription.nPosY;
		data.size = gCarDescription.fSize;
		Draw1080pString(JUSTIFY_LEFT, data, sCarDescription, &DrawStringFO2_Ingame12);
		data.x = gCarClass.nPosX;
		data.y = gCarClass.nPosY;
		data.size = gCarClass.fSize;
		std::string className = "BONUS";
		switch (GetDealerCar(pGameFlow->pMenuInterface->pMenuScene->nCar)->classId) {
			case 1:
				className = "BRONZE CLASS";
				break;
			case 2:
				className = "SILVER CLASS";
				break;
			case 3:
				className = "GOLD CLASS";
				break;
		}
		Draw1080pString(JUSTIFY_LEFT, data, className, &DrawStringFO2_Ingame12);
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
		static auto textureRight = LoadTextureFromBFS("data/menu/carselect_right.png");
		static auto textureArrows = LoadTextureFromBFS("data/menu/carselect_arrows.png");

		if (!bInSkinSelector) return;

		auto menu = pGameFlow->pMenuInterface;
		if (!menu) return;
		if (!menu->pMenuScene) return;

		int car = menu->pMenuScene->nCar;
		int skin = menu->pMenuScene->nCarSkin;
		int numSkins = GetNumSkinsForCar(car);

		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		bool skinLoop = false;
		if (numSkins > 6) {
			Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255, 255, 255, 255}, textureArrows);
			skinLoop = true;
		}
		else {
			skinLoop = false;
		}

		tNyaStringData data;
		data.x = gCarSkinSelectTitle.nPosX;
		data.y = gCarSkinSelectTitle.nPosY;
		data.size = gCarSkinSelectTitle.fSize;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, "SKINS", &DrawStringFO2_Small);

		float fPosX = 0.316;
		float fPosY = 0.372;
		float fSize = 0.035;
		float fSpacing = 0.0495;

		data.XCenterAlign = false;
		data.x = 1.0 - (fPosX * GetAspectRatioInv());
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

	std::string sLoadingScreenTextureName = "data/menu/loading.tga";
	void SetLoadingScreenTexture(const char* path) {
		sLoadingScreenTextureName = path;
	}

	float fLoadingSpriteX = 0.07;
	float fLoadingSpriteY = 0.8;
	float fLoadingSpriteSize = 0.15;

	float nLoadingLogoY = 64;

	int nLoadingTextX = 1650;
	int nLoadingTextY = 980;
	float fLoadingTextSize = 0.04;

	void DrawLoadingScreen() {
		if (!pLoadingScreen && GetGameState() == GAME_STATE_RACE) {
			sLoadingScreenTextureName = "data/menu/loading.tga";
		}

		if (!nUseNewLoadingScreen) return;
		if (!pLoadingScreen) return;
		if (sLoadingScreenTextureName.empty()) return;

		auto tex = LoadTextureFromBFS(sLoadingScreenTextureName.c_str());
		if (!tex) return;

		static auto loadingAnim = LoadTextureFromBFS("data/menu/loading_anim.tga");
		static auto gameLogo = LoadTextureFromBFS("data/menu/flatout_logo.png");
		static std::vector<tHUDData> loadingAnims = LoadHUDData("data/menu/loading_anim.bed", "loading_anim");

		// assuming a size of 640x480
		// todo re-add this after the track select menu background is fixed up
		//float aspectModifier = (GetAspectRatio() / (4.0 / 3.0)) - 1.0;
		float aspectModifier = 0;
		DrawRectangle(0, 1, 0 - (aspectModifier * 0.5), 1 + (aspectModifier * 0.5), {255,255,255,255}, 0, tex);

		const float fLoadingTimerSpeed = 0.4;

		static CNyaTimer gTimer;

		static int nLoadingSprite = 0;
		static double fLoadingTimer = 0;
		fLoadingTimer += gTimer.Process();
		while (fLoadingTimer > fLoadingTimerSpeed) {
			nLoadingSprite++;
			if (nLoadingSprite > 3) nLoadingSprite = 0;
			fLoadingTimer -= fLoadingTimerSpeed;
		}

		DrawRectangle(1.0 - ((fLoadingSpriteX + fLoadingSpriteSize) * GetAspectRatioInv()), 1.0 - (fLoadingSpriteX * GetAspectRatioInv()), fLoadingSpriteY, fLoadingSpriteY + fLoadingSpriteSize, {255,255,255,255}, 0, loadingAnim, 0, loadingAnims[nLoadingSprite].min, loadingAnims[nLoadingSprite].max);

		Draw1080pSprite(JUSTIFY_CENTER, 960 - 256, 960 + 256, nLoadingLogoY, nLoadingLogoY + 166, {255,255,255,255}, gameLogo);

		tNyaStringData data;
		data.x = nLoadingTextX;
		data.y = nLoadingTextY;
		data.size = fLoadingTextSize;
		data.XRightAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, "LOADING", &DrawStringFO2_Small);
	}

	const char* aAIPlayerNames[] = {
		"FRANK BENTON",
		"SUE O'NEILL",
		"TANIA GRAHAM",
		"KATIE DAGGERT",
		"RAY SMITH",
		"PAUL MCGUIRE",
		"SETH BELLINGER",
	};

	int nCareerListPositionX = 27;
	int nCareerListNameX = 80;
	int nCareerListPointsX = 760;
	int nCareerListTopY = 200;
	int nCareerListStartY = 263;
	int nCareerListSpacing = 82;
	float fCareerListSize = 0.045;

	int nCareerEventNameX = 1275;
	int nCareerEventNameY = 230;
	int nCareerEventDescY = 295;
	float fCareerEventSize = 0.035;
	float fCareerEventDescSize = 0.036;

	std::string GetCareerPlayerName(int i) {
		return i == 0 ? GetStringNarrow(pGameFlow->Profile.wsPlayerName) : aAIPlayerNames[i-1];
	}

	void DrawCareerMenu() {
		static auto textureLeft = LoadTextureFromBFS("data/menu/cupresultscreenbg_left.png");
		static auto textureRight = LoadTextureFromBFS("data/menu/cupresultscreenbg_right.png");

		if (!bInCareer) return;
		if (!CareerMode::IsCupActive()) return;

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		tNyaStringData data;
		data.SetColor(GetPaletteColor(18));
		data.x = nCareerListPositionX;
		data.y = nCareerListTopY;
		data.size = fCareerListSize;
		Draw1080pString(JUSTIFY_LEFT, data, "#", &DrawStringFO2_Ingame12);
		data.x = nCareerListNameX;
		Draw1080pString(JUSTIFY_LEFT, data, "NAME", &DrawStringFO2_Ingame12);
		data.x = nCareerListPointsX;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_LEFT, data, "POINTS", &DrawStringFO2_Ingame12);
		data.y = nCareerListStartY;
		data.SetColor(GetPaletteColor(17));
		for (int i = 0; i < 8; i++) {
			gCustomSave.CalculateCupPlayersByPosition();
			int playerId = gCustomSave.aCupPlayersByPosition[i];
			auto player = &gCustomSave.aCareerCupPlayers[playerId];
			std::string playerName = GetCareerPlayerName(playerId);
			data.XCenterAlign = false;
			data.x = nCareerListPositionX;
			Draw1080pString(JUSTIFY_LEFT, data, std::format("{}.", i+1), &DrawStringFO2_Ingame12);
			data.x = nCareerListNameX;
			Draw1080pString(JUSTIFY_LEFT, data, playerName, &DrawStringFO2_Ingame12);
			data.x = nCareerListPointsX;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_LEFT, data, std::to_string(player->points), &DrawStringFO2_Ingame12);
			data.y += nCareerListSpacing;
		}

		data.x = nCareerEventNameX;
		data.y = nCareerEventNameY;
		data.size = fCareerEventSize;
		data.XCenterAlign = false;
		Draw1080pString(JUSTIFY_RIGHT, data, CareerMode::GetCurrentCup()->sName, DrawStringFO2_Small);
		data.y = nCareerEventDescY;
		data.size = fCareerEventDescSize;
		Draw1080pString(JUSTIFY_RIGHT, data, std::format("EVENT {}/{}", gCustomSave.nCareerCupNextEvent+1, CareerMode::GetCurrentCup()->aRaces.size()), DrawStringFO2_Ingame12);
	}

	tDrawPositions gCareerCupSelectEvent = {0.15, 0.29, 0.1, 0.19, 0.135};
	float fCareerCupSelectEventHighlightSize = 0.094;

	tDrawPositions1080p gCareerCupSelectCupName = {1320, 193, 0.03};
	int nCareerCupSelectLapsX[10] = {
			1766, // 0
			1769, // 1
			1766, // 2
			1766, // 3
			1761, // 4
			1763, // 5
			1764, // 6
			1766, // 7
			1766, // 8
			1766, // 9
	};
	int nCareerCupSelectLapsY = 323;
	float fCareerCupSelectLapsSize = 0.03;
	tDrawPositions1080p gCareerCupSelectEvents = {1530, 435, 0.04, 0, 45};
	tDrawPositions1080p gCareerCupSelectEventsTitle = {1505, 285, 0.04};

	int nCareerCupSelectClass = 0;
	int nCareerCupSelectCursorX = 0;
	int nCareerCupSelectCursorY = 0;
	void CareerCupSelect_MoveLeft() {
		nCareerCupSelectCursorX--;
		if (nCareerCupSelectCursorX < 0) nCareerCupSelectCursorX = CareerMode::aLUACareerClasses[nCareerCupSelectClass].aCups.size()-1;
	}
	void CareerCupSelect_MoveRight() {
		nCareerCupSelectCursorX++;
		if (nCareerCupSelectCursorX >= CareerMode::aLUACareerClasses[nCareerCupSelectClass].aCups.size()) nCareerCupSelectCursorX = 0;
	}
	void CareerCupSelect_MoveUp() {
		nCareerCupSelectCursorY--;
		if (nCareerCupSelectCursorY < 0) nCareerCupSelectCursorY = 2;
	}
	void CareerCupSelect_MoveDown() {
		nCareerCupSelectCursorY++;
		if (nCareerCupSelectCursorY > 2) nCareerCupSelectCursorY = 0;
	}

	void DrawCareerCupSelect() {
		static CNyaTimer gTimer;
		gTimer.Process();

		static IDirect3DTexture9* textureLeft[3] = {
			LoadTextureFromBFS("data/menu/cup_bronze_bg.png"),
			LoadTextureFromBFS("data/menu/cup_silver_bg.png"),
			LoadTextureFromBFS("data/menu/cup_gold_bg.png"),
		};
		static auto textureRight = LoadTextureFromBFS("data/menu/cupselect_bg_right.png");
		static auto textureTracks = LoadTextureFromBFS("data/menu/track_icons.dds");
		static auto textureTracks2 = LoadTextureFromBFS("data/menu/track_icons_inactive.dds");
		static auto trackIcons = LoadHUDData("data/menu/track_icons.bed", "track_icons");

		if (!bInCareerCupSelect) return;

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft[nCareerCupSelectClass]);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		auto careerClass = &CareerMode::aLUACareerClasses[nCareerCupSelectClass];
		for (int i = 0; i < careerClass->aCups.size(); i++) {
			auto cup = &careerClass->aCups[i];
			auto cupSave = &gCustomSave.aCareerClasses[nCareerCupSelectClass].aCups[i];
			auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(cup->aRaces[0].nLevel, "Image"));
			if (!trackIcon) {
				MessageBoxA(0, std::format("Failed to find image for track {}", cup->aRaces[0].nLevel).c_str(), "Fatal error", MB_ICONERROR);
			}
			auto data = gCareerCupSelectEvent;
			float x1 = data.fPosX + data.fSpacingX * i;
			float y1 = data.fPosY;
			float x2 = x1 + data.fSize * 1.5;
			float y2 = y1 + data.fSize;
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, cupSave->bUnlocked ? textureTracks : textureTracks2, 0, trackIcon->min, trackIcon->max);
			if (i == nCareerCupSelectCursorX && nCareerCupSelectCursorY == 0) {
				auto rgb = GetPaletteColor(18);
				rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;
				x2 = x1 + fCareerCupSelectEventHighlightSize * 1.5;
				y2 = y1 + fCareerCupSelectEventHighlightSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, rgb);
			}
		}
		{
			auto cup = &careerClass->Finals;
			auto cupSave = &gCustomSave.aCareerClasses[nCareerCupSelectClass].Finals;
			auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(cup->aRaces[0].nLevel, "Image"));
			if (!trackIcon) {
				MessageBoxA(0, std::format("Failed to find image for track {}", cup->aRaces[0].nLevel).c_str(), "Fatal error", MB_ICONERROR);
			}
			auto data = gCareerCupSelectEvent;
			float x1 = data.fPosX + data.fSpacingX * 0.5;
			float y1 = data.fPosY + data.fSpacingY;
			float x2 = x1 + data.fSize * 1.5;
			float y2 = y1 + data.fSize;
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, cupSave->bUnlocked ? textureTracks : textureTracks2, 0, trackIcon->min, trackIcon->max);
			if (nCareerCupSelectCursorY == 1) {
				auto rgb = GetPaletteColor(18);
				rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;
				x2 = x1 + fCareerCupSelectEventHighlightSize * 1.5;
				y2 = y1 + fCareerCupSelectEventHighlightSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, rgb);
			}
		}
		for (int i = 0; i < careerClass->aEvents.size(); i++) {
			auto cup = &careerClass->aEvents[i];
			auto cupSave = &gCustomSave.aCareerClasses[nCareerCupSelectClass].aEvents[i];
			auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(cup->aRaces[0].nLevel, "Image"));
			if (!trackIcon) {
				MessageBoxA(0, std::format("Failed to find image for track {}", cup->aRaces[0].nLevel).c_str(), "Fatal error", MB_ICONERROR);
			}
			auto data = gCareerCupSelectEvent;
			float x1 = data.fPosX + data.fSpacingX * i;
			float y1 = data.fPosY + data.fSpacingY * 2;
			float x2 = x1 + data.fSize * 1.5;
			float y2 = y1 + data.fSize;
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, cupSave->bUnlocked ? textureTracks : textureTracks2, 0, trackIcon->min, trackIcon->max);
			if (i == nCareerCupSelectCursorX && nCareerCupSelectCursorY == 2) {
				auto rgb = GetPaletteColor(18);
				rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;
				x2 = x1 + fCareerCupSelectEventHighlightSize * 1.5;
				y2 = y1 + fCareerCupSelectEventHighlightSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, rgb);
			}
		}

		tNyaStringData data;
		data.x = gCareerCupSelectCupName.nPosX;
		data.y = gCareerCupSelectCupName.nPosY;
		data.size = gCareerCupSelectCupName.fSize;
		if (nCareerCupSelectCursorY == 0) {
			auto cup = &careerClass->aCups[nCareerCupSelectCursorX];
			Draw1080pString(JUSTIFY_RIGHT, data, cup->sName, &DrawStringFO2_Small);
			data.x = nCareerCupSelectLapsX[cup->aRaces.size()];
			data.y = nCareerCupSelectLapsY;
			data.size = fCareerCupSelectLapsSize;
			Draw1080pString(JUSTIFY_RIGHT, data, std::to_string(cup->aRaces.size()), &DrawStringFO2_Small);
			auto hud = gCareerCupSelectEvents;
			data.x = hud.nPosX;
			data.y = hud.nPosY;
			data.size = hud.fSize;
			for (auto& race : cup->aRaces) {
				Draw1080pString(JUSTIFY_RIGHT, data, GetTrackName(race.nLevel), &DrawStringFO2_Ingame12);
				data.y += hud.nSpacingY;
			}
		}
		else if (nCareerCupSelectCursorY == 1) {
			auto cup = &careerClass->Finals;
			Draw1080pString(JUSTIFY_RIGHT, data, cup->sName, &DrawStringFO2_Small);
			data.x = nCareerCupSelectLapsX[cup->aRaces.size()];
			data.y = nCareerCupSelectLapsY;
			data.size = fCareerCupSelectLapsSize;
			Draw1080pString(JUSTIFY_RIGHT, data, std::to_string(cup->aRaces.size()), &DrawStringFO2_Small);
			auto hud = gCareerCupSelectEvents;
			data.x = hud.nPosX;
			data.y = hud.nPosY;
			data.size = hud.fSize;
			for (auto& race : cup->aRaces) {
				Draw1080pString(JUSTIFY_RIGHT, data, GetTrackName(race.nLevel), &DrawStringFO2_Ingame12);
				data.y += hud.nSpacingY;
			}
		}
		else if (nCareerCupSelectCursorY == 2) {
			if (nCareerCupSelectCursorX >= careerClass->aEvents.size()) return;
			auto cup = &careerClass->aEvents[nCareerCupSelectCursorX];
			Draw1080pString(JUSTIFY_RIGHT, data, cup->sName, &DrawStringFO2_Small);
			data.x = nCareerCupSelectLapsX[cup->aRaces.size()];
			data.y = nCareerCupSelectLapsY;
			data.size = fCareerCupSelectLapsSize;
			Draw1080pString(JUSTIFY_RIGHT, data, std::to_string(cup->aRaces.size()), &DrawStringFO2_Small);
		}
		data.x = gCareerCupSelectEventsTitle.nPosX;
		data.y = gCareerCupSelectEventsTitle.nPosY;
		data.size = gCareerCupSelectEventsTitle.fSize;
		data.SetColor(GetPaletteColor(18));
		Draw1080pString(JUSTIFY_RIGHT, data, "EVENTS", &DrawStringFO2_Ingame12);
	}

	void OnTick() {
		DrawCarDealer();
		DrawSkinSelector();
		DrawCareerMenu();
		DrawCareerCupSelect();
		DrawLoadingScreen();
	}
}