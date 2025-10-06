class CMenu_CarDealer_Sell : public CMenu_CarDealer {
public:
	const char* GetName() override { return "menu_cardealer_sell"; }

	void Init() override {
		PreloadTexture("data/menu/sellcar_left.png");
	}

	void Process() override {
		if (!bEnabled) return;

		static auto textureLeft = LoadTextureFromBFS("data/menu/sellcar_left.png");
		DrawCarLogo();

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);

		auto menu = pGameFlow->pMenuInterface;
		if (!menu) return;
		if (!menu->pMenuScene) return;

		int car = menu->pMenuScene->nCar;
		int price = nCarPrice/2;
		int upgradePrice = gCustomSave.aCareerGarage[car].nUpgradesValue/2;

		tNyaStringData data;
		data.x = nBaseX;
		data.size = fBaseSize;
		data.y = nPriceY;
		Draw1080pString(JUSTIFY_LEFT, data, "TOTAL", &DrawStringFO2_Condensed12);
		data.y = nPowerY;
		Draw1080pString(JUSTIFY_LEFT, data, "CAR", &DrawStringFO2_Condensed12);
		data.y = nWeightY;
		Draw1080pString(JUSTIFY_LEFT, data, "UPGRADES", &DrawStringFO2_Condensed12);
		data.x = nOffsetX;
		data.y = nPriceY + nOffsetPriceY;
		data.size = fOffsetPriceSize;
		Draw1080pString(JUSTIFY_LEFT, data, std::format("${}", price + upgradePrice), &DrawStringFO2_Regular18);
		data.y = nPowerY + nOffsetY;
		data.size = fOffsetSize;
		Draw1080pString(JUSTIFY_LEFT, data, std::format("${}", price), &DrawStringFO2_Condensed12);
		data.y = nWeightY + nOffsetY;
		Draw1080pString(JUSTIFY_LEFT, data, std::format("${}", upgradePrice), &DrawStringFO2_Condensed12);
	}
} Menu_CarDealer_Sell;