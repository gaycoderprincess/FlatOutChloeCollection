class CMenu_CareerFinalResults : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_career_finalresults"; }

	static constexpr tDrawPositions1080p gTitle = {1740,230,0.04};
	static constexpr tDrawPositions1080p gCupName = {1740,275,0.02};
	static constexpr tDrawPositions1080p gFinishString = {1740,325,0.04};
	static constexpr tDrawPositions1080p gRewardString = {1740,803,0.04};
	static constexpr tDrawPositions1080p gWinCounts = {1800,410,0.05,0,80};
	static constexpr int nEventsX = 630;
	static constexpr int nEventsSpacing = 70;
	static constexpr int nPointsX = 1380;

	void Init() override {
		PreloadTexture("data/menu/final_results_screen_bg_left.png");
		PreloadTexture("data/menu/final_results_screen_bg_right.png");
	}

	void Process() override {
		static CNyaTimer gTimer;
		gTimer.Process();

		if (!bEnabled) return;

		static auto textureLeft = LoadTextureFromBFS("data/menu/final_results_screen_bg_left.png");
		static auto textureRight = LoadTextureFromBFS("data/menu/final_results_screen_bg_right.png");

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		tNyaStringData data;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		data.x = CMenu_Career::nListPositionX;
		data.y = CMenu_Career::nListTopY;
		data.size = CMenu_Career::fListSize;
		Draw1080pString(JUSTIFY_LEFT, data, "#", &DrawStringFO2_Condensed12);
		data.x = CMenu_Career::nListNameX;
		Draw1080pString(JUSTIFY_LEFT, data, "NAME", &DrawStringFO2_Condensed12);
		int numRaces = CareerMode::nLastCupNumRaces;
		data.x = nEventsX + (nEventsSpacing * 8);
		data.x -= (nEventsSpacing * numRaces);
		for (int i = 0; i < numRaces; i++) {
			data.XCenterAlign = false;
			Draw1080pString(JUSTIFY_LEFT, data, std::to_string(i+1), &DrawStringFO2_Condensed12);
			data.x += nEventsSpacing;
		}
		data.x = nPointsX;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_LEFT, data, "POINTS", &DrawStringFO2_Condensed12);
		data.y = CMenu_Career::nListStartY;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		for (int i = 0; i < 8; i++) {
			gCustomSave.CalculateCupPlayersByPosition();
			int playerId = gCustomSave.aCupPlayersByPosition[i];
			if (pGameFlow->nGameMode == eGameMode::SPLITSCREEN && i == 7 && playerId == 1) continue;
			auto player = &gCustomSave.aCareerCupPlayers[playerId];
			std::string playerName = CMenu_Career::GetPlayerName(playerId);
			data.XCenterAlign = false;
			data.x = CMenu_Career::nListPositionX;
			Draw1080pString(JUSTIFY_LEFT, data, std::format("{}.", i+1), &DrawStringFO2_Condensed12);
			data.x = CMenu_Career::nListNameX;
			Draw1080pString(JUSTIFY_LEFT, data, playerName, &DrawStringFO2_Condensed12);
			data.x = nEventsX + (nEventsSpacing * 8);
			data.x -= (nEventsSpacing * numRaces);
			for (int j = 0; j < numRaces; j++) {
				data.XCenterAlign = false;
				Draw1080pString(JUSTIFY_LEFT, data, std::to_string(player->eventPoints[j]), &DrawStringFO2_Condensed12);
				data.x += nEventsSpacing;
			}
			data.x = nPointsX;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_LEFT, data, std::to_string(player->points), &DrawStringFO2_Condensed12);
			data.y += CMenu_Career::nListSpacing;
		}
		data.x = gTitle.nPosX;
		data.y = gTitle.nPosY;
		data.size = gTitle.fSize;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		Draw1080pString(JUSTIFY_RIGHT, data, "Final Results", &DrawStringFO2_Condensed12);
		data.x = gCupName.nPosX;
		data.y = gCupName.nPosY;
		data.size = gCupName.fSize;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_RIGHT, data, CareerMode::sLastCupName, &DrawStringFO2_Regular18);
		const char* placementStrings[] = {
				"You finished first!",
				"You finished second!",
				"You finished third!",
				"You finished fourth!",
				"You finished fifth!",
				"You finished sixth!",
				"You finished seventh!",
				"You finished eighth!",
		};
		int playerPosition = gCustomSave.aCupPlayerPosition[0];
		if (playerPosition >= 0 && playerPosition < 8) {
			data.x = gFinishString.nPosX;
			data.y = gFinishString.nPosY;
			data.size = gFinishString.fSize;
			Draw1080pString(JUSTIFY_RIGHT, data, placementStrings[playerPosition], &DrawStringFO2_Condensed12);
			data.x = gRewardString.nPosX;
			data.y = gRewardString.nPosY;
			data.size = gRewardString.fSize;
			Draw1080pString(JUSTIFY_RIGHT, data, std::format("Reward: ${}", CareerMode::nLastCupAward), &DrawStringFO2_Condensed12);
		}
		for (int i = 0; i < 3; i++) {
			data.x = gWinCounts.nPosX;
			data.y = gWinCounts.nPosY + (gWinCounts.nSpacingY * i);
			data.size = gWinCounts.fSize;
			data.XCenterAlign = false;
			data.XRightAlign = true;
			Draw1080pString(JUSTIFY_RIGHT, data, std::format("{}x", gCustomSave.aCupLocalPlayerPlacements[i+1]), &DrawStringFO2_Condensed12);
		}
	}
} Menu_CareerFinalResults;