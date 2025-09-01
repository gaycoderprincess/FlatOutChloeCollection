class CMenu_CarDealer : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_cardealer"; }

	bool bSkinSelector = false;

	static inline int nCarHorsepower = 0;
	static inline int nCarWeight = 0;
	static inline int nCarPrice = 0;
	static inline std::string sCarName;
	static inline std::string sCarDescription;

	void SetCarStats(int carId) {
		auto data = GetCarDataTable(carId);
		auto config = GetCarPerformanceTable(carId);

		nCarPrice = data["Data"]["Price"].value_or(0);
		if (!nCarPrice) nCarPrice = config["Data"]["Price"].value_or(0);
		CAR_PERFORMANCE(nCarWeight, "Body", "Mass");
		CAR_PERFORMANCE(nCarHorsepower, "Engine", "Horsepower");
		sCarName = GetCarName(carId);
	}

	void SetCarStatsTuned(int carId) {
		SetCarStats(carId);

		auto config = GetCarPerformanceTable(carId);
		auto tuning = GetPlayerCareerTuningData(carId);
		CAR_PERFORMANCE_TUNE(nCarHorsepower, "Engine", "Engine_Max", "Horsepower", tuning.fHorsepower);
	}

	// todo add interpolation with upgrades
	static constexpr int nPowerY = 392;
	static constexpr int nPriceY = 320;
	static constexpr int nWeightY = 466;
	static constexpr int nOffsetY = 38;
	static constexpr int nBaseX = 160;
	static constexpr int nOffsetX = 385;
	static constexpr float fBaseSize = 0.04;
	static constexpr float fOffsetSize = 0.05;
	static constexpr int nOffsetPriceY = 30;
	static constexpr float fOffsetPriceSize = 0.04;

	static constexpr tDrawPositions1080p gCarClass = {160, 805, 0.04};
	static constexpr tDrawPositions1080p gCarDescription = {160, 593, 0.035};
	static constexpr tDrawPositions1080p gCarSkinSelectTitle = {1720, 328, 0.035};

	static constexpr tDrawPositions gCarName = {0.225, 0.17, 0.06};
	static constexpr float fCarNameAspect = 5;

	static constexpr tDrawPositions1080p gCarNameText = {405, 200, 0.05};

	static constexpr tDrawPositions1080p gSkinTitle = {1720, 328, 0.035};
	//static constexpr tDrawPositions1080p gSkinAuthor = {1588,797,0.02};
	static constexpr tDrawPositions1080p gSkinAuthor = {1716,801,0.035};

	static void ProcessSkinSelector() {
		static auto textureRight = LoadTextureFromBFS("data/menu/carselect_right.png");
		static auto textureSkinAuthor = LoadTextureFromBFS("data/menu/carselect_skin.png");
		static auto textureArrows = LoadTextureFromBFS("data/menu/carselect_arrows.png");

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

		if (auto author = GetSkinAuthor(car, skin, false); !author.empty()) {
			Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255, 255, 255, 255}, textureSkinAuthor);

			tNyaStringData data;
			data.x = gSkinAuthor.nPosX;
			data.y = gSkinAuthor.nPosY;
			data.size = gSkinAuthor.fSize;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_RIGHT, data, author, &DrawStringFO2_Ingame12);
		}

		tNyaStringData data;
		data.x = gSkinTitle.nPosX;
		data.y = gSkinTitle.nPosY;
		data.size = gSkinTitle.fSize;
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
				data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
			}
			else {
				data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
			}

			DrawString(data, GetSkinName(car, skinId, skinLoop), &DrawStringFO2_Ingame12);
			data.y += fSpacing;
		}
	}

	void DrawCarLogo() {
		static auto textureCarLogos = LoadTextureFromBFS("data/menu/car_logos.dds");
		static std::vector<tHUDData> gCarLogos = LoadHUDData("data/menu/car_logos.bed", "car_logos");

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
	}

	virtual void Init() {
		PreloadTexture("data/menu/carselect_left.png");
		PreloadTexture("data/menu/car_logos.dds");
		PreloadTexture("data/menu/carselect_right.png");
		PreloadTexture("data/menu/carselect_skin.png");
		PreloadTexture("data/menu/carselect_arrows.png");
	}

	virtual void Process() {
		if (!bEnabled) return;

		static auto textureLeft = LoadTextureFromBFS("data/menu/carselect_left.png");

		DrawCarLogo();
		if (bSkinSelector) ProcessSkinSelector();

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);

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
} Menu_CarDealer;