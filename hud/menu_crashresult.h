class CMenu_CrashResult : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_crashresult"; }

	static inline tDrawPositions1080p gTitles = {291,350,0.035,524,144};
	float fNumberOffset = 128;
	float fCashOffset = 285;
	float fBottomOffset = 28;
	float fSubSize = 0.04;

	static inline tDrawPositions1080p gTotalTitle = {960,750,0.03};
	static inline tDrawPositions1080p gTotalAmount = {960,800,0.03};

	void DrawCrashBonus(int type, int x, int y) const {
		auto name = GetCrashBonusName(type);
		if (!name) return;

		int count = CareerMode::aPlayerResults[0].aCrashBonuses[type];
		int price = count*CareerMode::GetCrashBonusPrice(type);

		tNyaStringData data;
		float baseX = data.x = gTitles.nPosX + gTitles.nSpacingX * x;
		float baseY = data.y = gTitles.nPosY + gTitles.nSpacingY * y;
		data.size = gTitles.fSize;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		Draw1080pString(JUSTIFY_CENTER, data, name, &DrawStringFO2_Ingame12);
		data.x = baseX + fNumberOffset;
		data.y = baseY - fBottomOffset;
		data.size = fSubSize;
		data.XCenterAlign = false;
		Draw1080pString(JUSTIFY_CENTER, data, "#", &DrawStringFO2_Ingame12);
		data.y = baseY + fBottomOffset;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_CENTER, data, std::to_string(count), &DrawStringFO2_Ingame12);
		data.x = baseX + fCashOffset;
		data.y = baseY - fBottomOffset;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		Draw1080pString(JUSTIFY_CENTER, data, "TOTAL", &DrawStringFO2_Ingame12);
		data.y = baseY + fBottomOffset;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_CENTER, data, std::format("${}", price), &DrawStringFO2_Ingame12);
	}

	void Init() override {
		PreloadTexture("data/menu/reward_bonus_screen_bg.png");
	}

	void Process() override {
		if (!bEnabled) return;

		static auto textureBg = LoadTextureFromBFS("data/menu/reward_bonus_screen_bg.png");

		Draw1080pSprite(JUSTIFY_CENTER, 0, 1920, 0, 1080, {255,255,255,255}, textureBg);

		DrawCrashBonus(CRASHBONUS_SLAM, 0, 0);
		DrawCrashBonus(CRASHBONUS_POWERHIT, 1, 0);
		DrawCrashBonus(CRASHBONUS_SUPERFLIP, 2, 0);
		DrawCrashBonus(CRASHBONUS_BLASTOUT, 0, 1);
		DrawCrashBonus(CRASHBONUS_RAGDOLLED, 1, 1);
		DrawCrashBonus(CRASHBONUS_WRECKED, 2, 1);

		int total = 0;
		for (int i = 0; i < NUM_CRASHBONUS_TYPES; i++) {
			int count = CareerMode::aPlayerResults[0].aCrashBonuses[i];
			int price = count*CareerMode::GetCrashBonusPrice(i);
			total += price;
		}

		tNyaStringData data;
		data.x = gTotalTitle.nPosX;
		data.y = gTotalTitle.nPosY;
		data.size = gTotalTitle.fSize;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		Draw1080pString(JUSTIFY_CENTER, data, "CRASH BONUS TOTAL", &DrawStringFO2_Small);
		data.x = gTotalAmount.nPosX;
		data.y = gTotalAmount.nPosY;
		data.size = gTotalAmount.fSize;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_CENTER, data, std::format("${}", total), &DrawStringFO2_Small);
	}
} Menu_CrashResult;