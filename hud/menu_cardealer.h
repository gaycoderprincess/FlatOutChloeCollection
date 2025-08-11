class CMenu_CarDealer : public CMenuHUDElement {
public:
	int nCarHorsepower = 0;
	int nCarWeight = 0;
	int nCarPrice = 0;
	std::string sCarName;
	std::string sCarDescription;

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

	virtual void Process() {
		static auto textureLeft = LoadTextureFromBFS("data/menu/carselect_left.png");
		static auto textureCarLogos = LoadTextureFromBFS("data/menu/car_logos.dds");
		static std::vector<tHUDData> gCarLogos = LoadHUDData("data/menu/car_logos.bed", "car_logos");

		if (!bEnabled) return;
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
} Menu_CarDealer;