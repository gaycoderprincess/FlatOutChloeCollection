class CMenu_Career_Unlock : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_career_unlock"; }

	static inline tDrawPositions1080p gUnlockTitle = {1222,195,0.035};
	static inline tDrawPositions1080p gUnlockSubtitle = {1222,254,0.03};

	static inline tDrawPositions1080p gBottomTitle = {960,800,0.03};
	//static inline tDrawPositions1080p gBottomAmount = {960,850,0.03};

	void Init() override {
		PreloadTexture("data/menu/event_unlock_box.png");
	}

	void Process() override {
		if (!bEnabled) return;

		static auto textureRight = LoadTextureFromBFS("data/menu/event_unlock_box.png");

		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		tNyaStringData data;
		data.x = gUnlockTitle.nPosX;
		data.y = gUnlockTitle.nPosY;
		data.size = gUnlockTitle.fSize;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		Draw1080pString(JUSTIFY_RIGHT, data, "Car:", &DrawStringFO2_Small);
		data.x = gUnlockSubtitle.nPosX;
		data.y = gUnlockSubtitle.nPosY;
		data.size = gUnlockSubtitle.fSize;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_RIGHT, data, GetCarName(pGameFlow->pMenuInterface->pMenuScene->nCar), &DrawStringFO2_Small);

		data.x = gBottomTitle.nPosX;
		data.y = gBottomTitle.nPosY;
		data.size = gBottomTitle.fSize;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		Draw1080pString(JUSTIFY_CENTER, data, "YOU HAVE UNLOCKED A NEW CAR!", &DrawStringFO2_Small);
		//data.x = gBottomAmount.nPosX;
		//data.y = gBottomAmount.nPosY;
		//data.size = gBottomAmount.fSize;
		//data.SetColor(GetPaletteColor(17));
		//Draw1080pString(JUSTIFY_CENTER, data, std::format("${}", total), &DrawStringFO2_Small);
	}
} Menu_Career_Unlock;