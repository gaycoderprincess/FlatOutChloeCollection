class CMenu_Career : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_career"; }

	static inline const char* aAIPlayerNames[] = {
			"FRANK BENTON",
			"SUE O'NEILL",
			"TANIA GRAHAM",
			"KATIE DAGGERT",
			"RAY SMITH",
			"PAUL MCGUIRE",
			"SETH BELLINGER",
	};

	static constexpr int nListPositionX = 27;
	static constexpr int nListNameX = 80;
	static constexpr int nListPointsX = 760;
	static constexpr int nListTopY = 200;
	static constexpr int nListStartY = 263;
	static constexpr int nListSpacing = 82;
	static constexpr float fListSize = 0.045;

	static constexpr int nEventNameX = 1275;
	static constexpr int nEventNameY = 230;
	static constexpr int nEventDescY = 295;
	static constexpr float fEventSize = 0.035;
	static constexpr float fEventDescSize = 0.036;

	static std::string GetPlayerName(int i) {
		return i == 0 ? GetStringNarrow(pGameFlow->Profile.sPlayerName) : aAIPlayerNames[i-1];
	}

	virtual void Init() {
		PreloadTexture("data/menu/cupresultscreenbg_left.png");
		PreloadTexture("data/menu/cupresultscreenbg_right.png");
	}

	virtual void Process() {
		if (!bEnabled) return;
		if (!CareerMode::IsCupActive()) return;

		static auto textureLeft = LoadTextureFromBFS("data/menu/cupresultscreenbg_left.png");
		static auto textureRight = LoadTextureFromBFS("data/menu/cupresultscreenbg_right.png");

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		tNyaStringData data;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		data.x = nListPositionX;
		data.y = nListTopY;
		data.size = fListSize;
		Draw1080pString(JUSTIFY_LEFT, data, "#", &DrawStringFO2_Ingame12);
		data.x = nListNameX;
		Draw1080pString(JUSTIFY_LEFT, data, "NAME", &DrawStringFO2_Ingame12);
		data.x = nListPointsX;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_LEFT, data, "POINTS", &DrawStringFO2_Ingame12);
		data.y = nListStartY;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		for (int i = 0; i < 8; i++) {
			gCustomSave.CalculateCupPlayersByPosition();
			int playerId = gCustomSave.aCupPlayersByPosition[i];
			auto player = &gCustomSave.aCareerCupPlayers[playerId];
			std::string playerName = GetPlayerName(playerId);
			data.XCenterAlign = false;
			data.x = nListPositionX;
			Draw1080pString(JUSTIFY_LEFT, data, std::format("{}.", i+1), &DrawStringFO2_Ingame12);
			data.x = nListNameX;
			Draw1080pString(JUSTIFY_LEFT, data, playerName, &DrawStringFO2_Ingame12);
			data.x = nListPointsX;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_LEFT, data, std::to_string(player->points), &DrawStringFO2_Ingame12);
			data.y += nListSpacing;
		}

		data.x = nEventNameX;
		data.y = nEventNameY;
		data.size = fEventSize;
		data.XCenterAlign = false;
		Draw1080pString(JUSTIFY_RIGHT, data, CareerMode::GetCurrentCup()->sName, DrawStringFO2_Small);
		data.y = nEventDescY;
		data.size = fEventDescSize;
		Draw1080pString(JUSTIFY_RIGHT, data, std::format("EVENT {}/{}", gCustomSave.nCareerCupNextEvent+1, CareerMode::GetCurrentCup()->aRaces.size()), DrawStringFO2_Ingame12);
	}
} Menu_Career;