class CMenu_CareerFinalResults : public CMenuHUDElement {
public:
	static constexpr tDrawPositions1080p gCareerFinalResultsTitle = {1740,230,0.04};
	static constexpr tDrawPositions1080p gCareerFinalResultsCupName = {1740,275,0.02};
	static constexpr tDrawPositions1080p gCareerFinalResultsFinishString = {1740,325,0.04};
	static constexpr tDrawPositions1080p gCareerFinalResultsWinCounts = {1800,410,0.05,0,80};
	static constexpr int nCareerFinalResultsEventsX = 630;
	static constexpr int nCareerFinalResultsEventsSpacing = 70;
	static constexpr int nCareerFinalResultsPointsX = 1380;

	virtual void Process() {
		static CNyaTimer gTimer;
		gTimer.Process();

		static auto textureLeft = LoadTextureFromBFS("data/menu/final_results_screen_bg_left.png");
		static auto textureRight = LoadTextureFromBFS("data/menu/final_results_screen_bg_right.png");

		if (!bEnabled) return;

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		tNyaStringData data;
		data.SetColor(GetPaletteColor(18));
		data.x = CMenu_Career::nListPositionX;
		data.y = CMenu_Career::nListTopY;
		data.size = CMenu_Career::fListSize;
		Draw1080pString(JUSTIFY_LEFT, data, "#", &DrawStringFO2_Ingame12);
		data.x = CMenu_Career::nListNameX;
		Draw1080pString(JUSTIFY_LEFT, data, "NAME", &DrawStringFO2_Ingame12);
		int numRaces = CareerMode::nLastCupNumRaces;
		data.x = nCareerFinalResultsEventsX + (nCareerFinalResultsEventsSpacing * 8);
		data.x -= (nCareerFinalResultsEventsSpacing * numRaces);
		for (int i = 0; i < numRaces; i++) {
			data.XCenterAlign = false;
			Draw1080pString(JUSTIFY_LEFT, data, std::to_string(i+1), &DrawStringFO2_Ingame12);
			data.x += nCareerFinalResultsEventsSpacing;
		}
		data.x = nCareerFinalResultsPointsX;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_LEFT, data, "POINTS", &DrawStringFO2_Ingame12);
		data.y = CMenu_Career::nListStartY;
		data.SetColor(GetPaletteColor(17));
		for (int i = 0; i < 8; i++) {
			gCustomSave.CalculateCupPlayersByPosition();
			int playerId = gCustomSave.aCupPlayersByPosition[i];
			auto player = &gCustomSave.aCareerCupPlayers[playerId];
			std::string playerName = CMenu_Career::GetCareerPlayerName(playerId);
			data.XCenterAlign = false;
			data.x = CMenu_Career::nListPositionX;
			Draw1080pString(JUSTIFY_LEFT, data, std::format("{}.", i+1), &DrawStringFO2_Ingame12);
			data.x = CMenu_Career::nListNameX;
			Draw1080pString(JUSTIFY_LEFT, data, playerName, &DrawStringFO2_Ingame12);
			data.x = nCareerFinalResultsEventsX + (nCareerFinalResultsEventsSpacing * 8);
			data.x -= (nCareerFinalResultsEventsSpacing * numRaces);
			for (int j = 0; j < numRaces; j++) {
				data.XCenterAlign = false;
				Draw1080pString(JUSTIFY_LEFT, data, std::to_string(player->eventPoints[j]), &DrawStringFO2_Ingame12);
				data.x += nCareerFinalResultsEventsSpacing;
			}
			data.x = nCareerFinalResultsPointsX;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_LEFT, data, std::to_string(player->points), &DrawStringFO2_Ingame12);
			data.y += CMenu_Career::nListSpacing;
		}
		data.x = gCareerFinalResultsTitle.nPosX;
		data.y = gCareerFinalResultsTitle.nPosY;
		data.size = gCareerFinalResultsTitle.fSize;
		data.SetColor(GetPaletteColor(18));
		Draw1080pString(JUSTIFY_RIGHT, data, "Final Results", &DrawStringFO2_Ingame12);
		data.x = gCareerFinalResultsCupName.nPosX;
		data.y = gCareerFinalResultsCupName.nPosY;
		data.size = gCareerFinalResultsCupName.fSize;
		data.SetColor(GetPaletteColor(17));
		Draw1080pString(JUSTIFY_RIGHT, data, CareerMode::sLastCupName, &DrawStringFO2_Small);
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
			data.x = gCareerFinalResultsFinishString.nPosX;
			data.y = gCareerFinalResultsFinishString.nPosY;
			data.size = gCareerFinalResultsFinishString.fSize;
			Draw1080pString(JUSTIFY_RIGHT, data, placementStrings[playerPosition], &DrawStringFO2_Ingame12);
		}
		for (int i = 0; i < 3; i++) {
			data.x = gCareerFinalResultsWinCounts.nPosX;
			data.y = gCareerFinalResultsWinCounts.nPosY + (gCareerFinalResultsWinCounts.nSpacingY * i);
			data.size = gCareerFinalResultsWinCounts.fSize;
			data.XCenterAlign = false;
			data.XRightAlign = true;
			Draw1080pString(JUSTIFY_RIGHT, data, std::format("{}x", gCustomSave.aCupLocalPlayerPlacements[i+1]), &DrawStringFO2_Ingame12);
		}
	}
} Menu_CareerFinalResults;