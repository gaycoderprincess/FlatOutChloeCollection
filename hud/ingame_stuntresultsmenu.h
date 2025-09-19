class CHUD_StuntResultsMenu : public CIngameHUDElement {
public:
	void Init() override {
		nHUDLayer = eHUDLayer::OVERLAY;
	}

	static inline tDrawPositions1080p gMenuText = {720,540,0.04, 0, 64};
	int nMenuText2 = 1210;

	static inline bool bMenuUp = false;

	static std::string GetPlayerScoreString(PlayerScoreRace* player) {
		bool useMeters = pGameFlow->nSubEventType == eSubEventType::STUNT_LONGJUMP || pGameFlow->nSubEventType == eSubEventType::STUNT_HIGHJUMP;

		float score = 0;
		for (int i = 0; i < GetScoreManager()->nNumLaps; i++) {
			if (useMeters) score += player->nStuntMetersScore[i] * 0.01;
			else score += player->nStuntPointsScore[i];
		}

		if (useMeters) {
			return std::format("{:.2f} METERS", score);
		}
		else if (pGameFlow->nSubEventType == eSubEventType::STUNT_CLOWN) {
			return std::format("${}", (int)score);
		}
		else {
			return std::format("{} POINTS", (int)score);
		}
	}

	void Process() override {
		if (!bMenuUp) return;

		CHUD_PauseMenu::DrawBackground();
		CHUD_PauseMenu::DrawMenuTitle("FINAL SCORES");

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetScoreManager()->aScores[i];

			tNyaStringData data;
			data.x = gMenuText.nPosX;
			data.y = gMenuText.nPosY + (gMenuText.nSpacingY * i);
			data.size = gMenuText.fSize;
			data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
			Draw1080pString(JUSTIFY_CENTER, data, std::format("{}. {}", i+1, GetStringNarrow(GetPlayer(ply->nPlayerId)->sPlayerName.Get())), &DrawStringFO2_Ingame12);
			data.x = nMenuText2;
			data.XRightAlign = true;
			data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
			Draw1080pString(JUSTIFY_CENTER, data, GetPlayerScoreString(ply), &DrawStringFO2_Ingame12);
		}

		bMenuUp = false;
	}
} HUD_StuntResultsMenu;