class CHUD_RaceResults : public CIngameHUDElement {
public:

	virtual void Init() {
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

	uint32_t GetBestLap(PlayerScoreRace* player) {
		uint32_t best = UINT_MAX;
		for (int i = 0; i < player->nCurrentLap; i++) {
			auto lap = player->nLapTimes[i];
			if (i > 0) lap -= player->nLapTimes[i-1];
			if (lap < best) best = lap;
		}
		return best;
	}

	virtual void Process() {
		if (!bMenuUp) return;

		DrawRectangle(0, 1, 0, 1, {0,0,0,200}); // background box

		static auto texture1 = LoadTextureFromBFS("data/global/overlay/raceresult_balk1.png");
		static auto texture2 = LoadTextureFromBFS("data/global/overlay/raceresult_balk2.png");
		DrawRectangle(0, 1, 99 / 1080.0, (99 + 72) / 1080.0, {255,255,255,255}, 0, texture2);
		tNyaStringData data;
		data.x = gGamemodeLabel.nPosX;
		data.y = gGamemodeLabel.nPosY;
		data.size = gGamemodeLabel.fSize;
		data.SetColor(GetPaletteColor(COLOR_INGAMEMENU_TITLE));
		Draw1080pString(JUSTIFY_CENTER, data, pGameFlow->nEventType == eEventType::DERBY ? "DERBY RESULTS" : "RACE RESULTS", &DrawStringFO2_Small);

		data.size = fPlayerSize;
		data.y = nPlayerYStartLabel;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		data.x = nPositionX;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_CENTER, data, "#", &DrawStringFO2_Ingame12);
		data.XCenterAlign = false;
		data.x = nNameX;
		Draw1080pString(JUSTIFY_CENTER, data, "DRIVER", &DrawStringFO2_Ingame12);
		if (bIsWreckingDerby) {
			data.x = nWreckingDerbyCrashX;
			Draw1080pString(JUSTIFY_CENTER, data, "CRASH", &DrawStringFO2_Ingame12);
			data.x = nWreckingDerbyWreckX;
			Draw1080pString(JUSTIFY_CENTER, data, "WRECK", &DrawStringFO2_Ingame12);
			data.x = nWreckingDerbyBonusX;
			Draw1080pString(JUSTIFY_CENTER, data, "BONUS", &DrawStringFO2_Ingame12);
			data.x = nWreckingDerbyTotalX;
			Draw1080pString(JUSTIFY_CENTER, data, "TOTAL", &DrawStringFO2_Ingame12);
		}
		else {
			data.x = nBestLapX;
			Draw1080pString(JUSTIFY_CENTER, data, pGameFlow->nEventType == eEventType::DERBY ? "WRECKS" : "BEST LAP", &DrawStringFO2_Ingame12);
			data.x = nTotalTimeX;
			Draw1080pString(JUSTIFY_CENTER, data, "TOTAL TIME", &DrawStringFO2_Ingame12);
		}

		data.y = nPlayerYStart;

		auto aScores = GetSortedPlayerScores();
		for (auto& ply : aScores) {
			//if (!ply->bHasFinished && !ply->bIsDNF) continue;

			DrawRectangle(0, 1, (data.y / 1080.0) + fBalkOffsetY, (data.y / 1080.0) + fBalkOffsetY + fBalkSizeY, {255,255,255,255}, 0, texture1);

			auto player = GetPlayer(ply->nPlayerId);
			data.SetColor(GetPaletteColor(player->nPlayerType == PLAYERTYPE_LOCAL ? COLOR_MENU_YELLOW : COLOR_MENU_WHITE));
			if (bIsWreckingDerby) {
				data.x = nWreckingDerbyCrashX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(GetWreckingDerbyCrashScore(ply->nPlayerId)), &DrawStringFO2_Ingame12);
				data.x = nWreckingDerbyWreckX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(GetWreckingDerbyWreckScore(ply->nPlayerId)), &DrawStringFO2_Ingame12);
				data.x = nWreckingDerbyBonusX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(GetWreckingDerbyBonusScore(ply->nPosition, ply->bHasFinished || ply->bIsDNF)), &DrawStringFO2_Ingame12);
				data.x = nWreckingDerbyTotalX;
				Draw1080pString(JUSTIFY_CENTER, data, std::to_string(GetWreckingDerbyTotalScore(ply->nPlayerId, ply->nPosition, ply->bHasFinished || ply->bIsDNF)), &DrawStringFO2_Ingame12);
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
				Draw1080pString(JUSTIFY_CENTER, data, pGameFlow->nEventType == eEventType::DERBY ? std::to_string(aCrashBonusesReceived[ply->nPlayerId][CRASHBONUS_WRECKED]) : bestLap, &DrawStringFO2_Ingame12);
				data.x = nTotalTimeX;
				Draw1080pString(JUSTIFY_CENTER, data, finish, &DrawStringFO2_Ingame12);
			}

			data.x = nNameX;
			Draw1080pString(JUSTIFY_CENTER, data, GetStringNarrow(player->sPlayerName.Get()), &DrawStringFO2_Ingame12);

			data.x = nPositionX;
			data.y += nPositionY;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_CENTER, data, std::format("{}.", (&ply - &aScores[0]) + 1), &DrawStringFO2_Small);
			data.XCenterAlign = false;
			data.y -= nPositionY;

			data.y += nPlayerSpacing;
		}

		data.x = 960;
		data.y = nPressStartY;
		data.size = 0.035;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_CENTER, data, "Press START button", &DrawStringFO2_Small);

		bMenuUp = false;
	}
} HUD_RaceResults;