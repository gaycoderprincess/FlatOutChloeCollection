class CHUD_RaceResults : public CIngameHUDElement {
public:

	void Init() override {
		nHUDLayer = eHUDLayer::OVERLAY;
	}

	tDrawPositions1080p gGamemodeLabel = {50,124,0.035};

	int nPlayerYStartLabel = 276;
	int nPlayerYStart = 330;
	int nPlayerSpacing = 64;

	int nPositionX = 138;
	int nPositionY = -9;
	int nNameX = 250;
	int nBestLapX = 1270;
	int nTotalTimeX = 1550;

	int nWreckingDerbyCrashX = 1550 - (140*3);
	int nWreckingDerbyWreckX = 1550 - (140*2);
	int nWreckingDerbyBonusX = 1550 - (140*1);
	int nWreckingDerbyTotalX = 1550;

	float fBalkOffsetY = -0.026;
	float fBalkSizeY = 0.055;

	float fPlayerSize = 0.035;

	int nPressStartY = 930;

	static inline bool bMenuUp = false;

	uint32_t GetBestLap(PlayerScore* player) {
		uint32_t best = UINT_MAX;
		for (int i = 0; i < player->nCurrentLap; i++) {
			auto lap = player->nLapTimes[i];
			if (i > 0) lap -= player->nLapTimes[i-1];
			if (lap < best) best = lap;
		}
		return best;
	}

	tDrawPositions1080p gTotalEventScore = {960,217,0.025};
	tDrawPositions1080p gEventScore = {955,320,0.055};
	tDrawPositions1080p gHighScore = {952,400,0.03, 16};
	tDrawPositions1080p gScoreBreakdown = {960,480,0.025, 16};
	tDrawPositions1080p gScoreTypes = {952,532,0.03,16, (int)(0.042*1080)};
	void DrawArcadeResults() {
		static auto texture1 = LoadTextureFromBFS("data/global/overlay/raceresult_balk1.tga");
		static auto texture2 = LoadTextureFromBFS("data/global/overlay/raceresult_balk2.tga");

		int numScoreTypes = bIsFragDerby ? 3 : 4;

		for (int i = 0; i < numScoreTypes; i++) {
			DrawRectangle(0, 1, 0.475 + 0.042 * i, (0.475 + 0.042 * i) + 0.039, {255, 255, 255, 255}, 0, texture1); // car crashes
		}
		DrawRectangle(0, 1, 0.235, 0.235 + 0.18, {255, 255, 255, 255}, 0, texture1); // event score balk
		DrawRectangle(0, 1, 0.188, 0.188 + 0.039, {255,255,255,255}, 0, texture2); // total event score
		DrawRectangle(0, 1, 0.432, 0.432 + 0.039, {255,255,255,255}, 0, texture2); // score breakdown

		tNyaStringData data;
		data.x = gTotalEventScore.nPosX;
		data.y = gTotalEventScore.nPosY;
		data.size = gTotalEventScore.fSize;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_INGAMEMENU_TITLE));
		Draw1080pString(JUSTIFY_CENTER, data, "TOTAL EVENT SCORE", &DrawStringFO2_Regular18);
		data.y = gScoreBreakdown.nPosY;
		Draw1080pString(JUSTIFY_CENTER, data, bIsFragDerby ? "TOP DRIVERS" : "SCORE BREAKDOWN", &DrawStringFO2_Regular18);
		data.x = gEventScore.nPosX;
		data.y = gEventScore.nPosY;
		data.size = gEventScore.fSize;
		auto event = ArcadeMode::pCurrentEvent;
		auto score = ArcadeMode::nCurrentEventScore;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		if (score >= event->aGoalScores[2]) {
			data.SetColor(GetMedalRGB(3));
		}
		if (score >= event->aGoalScores[1]) {
			data.SetColor(GetMedalRGB(2));
		}
		if (score >= event->aGoalScores[0]) {
			data.SetColor(GetMedalRGB(1));
		}
		if (score >= event->nPlatinumScore) {
			data.SetColor(GetMedalRGB(4));
		}
		if (event->nCommunityScore > 0 && score >= event->nCommunityScore) {
			data.SetColor(GetMedalRGB(5));
		}
		Draw1080pString(JUSTIFY_CENTER, data, FormatScore(score), &DrawStringFO2_Regular18);
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		data.x = gHighScore.nPosX;
		data.y = gHighScore.nPosY;
		data.size = gHighScore.fSize;
		data.XRightAlign = true;
		data.XCenterAlign = false;
		Draw1080pString(JUSTIFY_CENTER, data, "HIGH SCORE", &DrawStringFO2_Condensed12);
		data.x += gHighScore.nSpacingX;
		data.XRightAlign = false;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_CENTER, data, FormatScore(gCustomSave.aArcadeCareerScores[ArcadeMode::nCurrentEventId]), &DrawStringFO2_Condensed12);
		if (bIsFragDerby) {
			for (int i = 0; i < 3; i++) {
				const char* types[] = {
						"STREAKER",
						"VICTIM",
						"SURVIVOR",
				};

				int topDriverId = FragDerby::GetTopDriverOfType(i);
				auto topDriverName = GetStringNarrow(GetPlayer(topDriverId)->sPlayerName.Get());

				data.x = gScoreTypes.nPosX;
				data.y = gScoreTypes.nPosY + (gScoreTypes.nSpacingY * i);
				data.size = gScoreTypes.fSize;
				data.XRightAlign = true;
				data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
				Draw1080pString(JUSTIFY_CENTER, data, types[i], &DrawStringFO2_Condensed12);
				data.x += gScoreTypes.nSpacingX;
				data.XRightAlign = false;
				data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
				Draw1080pString(JUSTIFY_CENTER, data, topDriverName, &DrawStringFO2_Condensed12);
			}
		}
		else if (bIsCarnageRace) {
			for (int i = 0; i < 4; i++) {
				const char* types[] = {
					"CAR CRASHES",
					"SCENERY CRASHES",
					"CHECKPOINT BONUS",
					"AIRTIME",
				};

				data.x = gScoreTypes.nPosX;
				data.y = gScoreTypes.nPosY + (gScoreTypes.nSpacingY * i);
				data.size = gScoreTypes.fSize;
				data.XRightAlign = true;
				data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
				Draw1080pString(JUSTIFY_CENTER, data, types[i], &DrawStringFO2_Condensed12);
				data.x += gScoreTypes.nSpacingX;
				data.XRightAlign = false;
				data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
				Draw1080pString(JUSTIFY_CENTER, data, FormatScore(CarnageRace::nPlayerScoresByType[i]), &DrawStringFO2_Condensed12);
			}
		}
	}

	void DrawPlayerList() {
		static auto texture1 = LoadTextureFromBFS("data/global/overlay/raceresult_balk1.tga");
		static auto texture2 = LoadTextureFromBFS("data/global/overlay/raceresult_balk2.tga");

		tNyaStringData data;
		data.size = fPlayerSize;
		data.y = nPlayerYStartLabel;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		data.x = nPositionX;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_CENTER, data, "#", &DrawStringFO2_Condensed12);
		data.XCenterAlign = false;
		data.x = nNameX;
		Draw1080pString(JUSTIFY_CENTER, data, "DRIVER", &DrawStringFO2_Condensed12);
		if (bIsCarnageRace) {
			data.x = nBestLapX;
			Draw1080pString(JUSTIFY_CENTER, data, "BEST LAP", &DrawStringFO2_Condensed12);
			data.x = nTotalTimeX;
			Draw1080pString(JUSTIFY_CENTER, data, "SCORE", &DrawStringFO2_Condensed12);
		}
		else if (bIsFragDerby) {
			data.x = nBestLapX;
			Draw1080pString(JUSTIFY_CENTER, data, "WRECKS", &DrawStringFO2_Condensed12);
			data.x = nTotalTimeX;
			Draw1080pString(JUSTIFY_CENTER, data, "SCORE", &DrawStringFO2_Condensed12);
		}
		else if (bIsWreckingDerby) {
			data.x = nWreckingDerbyCrashX;
			Draw1080pString(JUSTIFY_CENTER, data, "CRASH", &DrawStringFO2_Condensed12);
			data.x = nWreckingDerbyWreckX;
			Draw1080pString(JUSTIFY_CENTER, data, "WRECK", &DrawStringFO2_Condensed12);
			data.x = nWreckingDerbyBonusX;
			Draw1080pString(JUSTIFY_CENTER, data, "BONUS", &DrawStringFO2_Condensed12);
			data.x = nWreckingDerbyTotalX;
			Draw1080pString(JUSTIFY_CENTER, data, "TOTAL", &DrawStringFO2_Condensed12);
		}
		else {
			if (bIsSmashyRace) {
				data.x = nTotalTimeX;
				Draw1080pString(JUSTIFY_CENTER, data, "SCORE", &DrawStringFO2_Condensed12);
			}
			else {
				data.x = nBestLapX;
				Draw1080pString(JUSTIFY_CENTER, data, pGameFlow->nEventType == eEventType::DERBY ? "WRECKS" : "BEST LAP", &DrawStringFO2_Condensed12);
				data.x = nTotalTimeX;
				Draw1080pString(JUSTIFY_CENTER, data, "TOTAL TIME", &DrawStringFO2_Condensed12);
			}
		}

		data.y = nPlayerYStart;

		auto aScores = GetSortedPlayerScores();
		for (auto& ply : aScores) {
			//if (!ply->bHasFinished && !ply->bIsDNF) continue;

			DrawRectangle(0, 1, (data.y / 1080.0) + fBalkOffsetY, (data.y / 1080.0) + fBalkOffsetY + fBalkSizeY, {255,255,255,255}, 0, texture1);

			auto player = GetPlayer(ply->nPlayerId);
			data.SetColor(GetPaletteColor(player->nPlayerType == PLAYERTYPE_LOCAL ? COLOR_MENU_YELLOW : COLOR_MENU_WHITE));
			if (bIsCarnageRace) {
				std::string bestLap;
				if (!ply->nCurrentLap) {
					bestLap = "--'--\"--";
				} else {
					bestLap = FormatGameTime(GetBestLap(ply), true);
				}

				data.x = nBestLapX;
				Draw1080pString(JUSTIFY_CENTER, data, bestLap, &DrawStringFO2_Condensed12);
				data.x = nTotalTimeX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(CarnageRace::nPlayerScore[ply->nPlayerId]), &DrawStringFO2_Condensed12);
			}
			else if (bIsFragDerby) {
				data.x = nBestLapX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(aCrashBonusesReceived[ply->nPlayerId][CRASHBONUS_WRECKED]), &DrawStringFO2_Condensed12);
				data.x = nTotalTimeX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(FragDerby::nPlayerScore[ply->nPlayerId]), &DrawStringFO2_Condensed12);
			}
			else if (bIsWreckingDerby) {
				data.x = nWreckingDerbyCrashX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(GetWreckingDerbyCrashScore(ply->nPlayerId)), &DrawStringFO2_Condensed12);
				data.x = nWreckingDerbyWreckX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(GetWreckingDerbyWreckScore(ply->nPlayerId)), &DrawStringFO2_Condensed12);
				data.x = nWreckingDerbyBonusX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(GetWreckingDerbyBonusScore(ply->nPosition, ply->bHasFinished || ply->bIsDNF)), &DrawStringFO2_Condensed12);
				data.x = nWreckingDerbyTotalX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(GetWreckingDerbyTotalScore(ply->nPlayerId, ply->nPosition, ply->bHasFinished || ply->bIsDNF)), &DrawStringFO2_Condensed12);
			}
			else if (bIsSmashyRace) {
				data.x = nTotalTimeX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(ArcadeMode::nCurrentEventScore), &DrawStringFO2_Condensed12);
			}
			else {
				std::string bestLap;
				if (!ply->nCurrentLap) {
					bestLap = "--'--\"--";
				} else {
					bestLap = FormatGameTime(GetBestLap(ply), true);
				}

				std::string finish;
				if (pGameFlow->nEventType == eEventType::DERBY) {
					if (ply->bHasFinished || ply->bIsDNF) {
						finish = FormatGameTime(ply->nFinishTime, true);
					} else {
						finish = "--'--\"--";
					}
				} else {
					if (ply->bHasFinished) {
						finish = FormatGameTime(ply->nFinishTime, true);
					} else if (ply->bIsDNF) {
						finish = "DNF";
					} else {
						finish = "--'--\"--";
					}
				}
				data.x = nBestLapX;
				Draw1080pString(JUSTIFY_CENTER, data, pGameFlow->nEventType == eEventType::DERBY ? std::to_string(aCrashBonusesReceived[ply->nPlayerId][CRASHBONUS_WRECKED]) : bestLap, &DrawStringFO2_Condensed12);
				data.x = nTotalTimeX;
				Draw1080pString(JUSTIFY_CENTER, data, finish, &DrawStringFO2_Condensed12);
			}

			data.x = nNameX;
			Draw1080pString(JUSTIFY_CENTER, data, GetStringNarrow(player->sPlayerName.Get()), &DrawStringFO2_Condensed12);

			data.x = nPositionX;
			data.y += nPositionY;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_CENTER, data, std::format("{}.", (&ply - &aScores[0]) + 1), &DrawStringFO2_Regular18);
			data.XCenterAlign = false;
			data.y -= nPositionY;

			data.y += nPlayerSpacing;
		}
	}

	static std::string GetResultsTitle() {
		if (bIsArcadeMode) return "FINAL SCORES";
		if (pGameFlow->nEventType == eEventType::DERBY) return "DERBY RESULTS";
		return "RACE RESULTS";
	}

	void Process() override {
		if (!bMenuUp) return;

		DrawRectangle(0, 1, 0, 1, {0,0,0,200}); // background box

		static auto texture1 = LoadTextureFromBFS("data/global/overlay/raceresult_balk1.tga");
		static auto texture2 = LoadTextureFromBFS("data/global/overlay/raceresult_balk2.tga");
		DrawRectangle(0, 1, 99 / 1080.0, (99 + 72) / 1080.0, {255,255,255,255}, 0, texture2);
		tNyaStringData data;
		data.x = gGamemodeLabel.nPosX;
		data.y = gGamemodeLabel.nPosY;
		data.size = gGamemodeLabel.fSize;
		data.SetColor(GetPaletteColor(COLOR_INGAMEMENU_TITLE));
		Draw1080pString(JUSTIFY_CENTER, data, GetResultsTitle(), &DrawStringFO2_Regular18);

		if (bIsArcadeMode && (bIsCarnageRace || bIsFragDerby)) {
			DrawArcadeResults();
		}
		else {
			DrawPlayerList();
		}

		data.x = 960;
		data.y = nPressStartY;
		data.size = 0.035;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_CENTER, data, "Press START button", &DrawStringFO2_Regular18);

		bMenuUp = false;
	}
} HUD_RaceResults;