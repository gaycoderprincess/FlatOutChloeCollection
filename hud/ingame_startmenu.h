class CHUD_StartMenu : public CIngameHUDElement {
public:
	void Init() override {
		nHUDLayer = eHUDLayer::OVERLAY;
	}

	static inline tDrawPositions1080p gMenuText = {(int)CHUD_PauseMenu::nBoxCenter,620,0.04};

	static inline bool bMenuUp = false;

	void Process() override {
		if (!bMenuUp || bIsArcadeMode) return;
		if (!bIsInMultiplayer) {
			if (bIsCarnageRace || bIsSmashyRace) return;
		}

		CHUD_PauseMenu::DrawBackground();
		if (pGameFlow->nEventType == eEventType::STUNT) {
			if (pGameFlow->nGameMode == eGameMode::SPLITSCREEN) {
				CHUD_PauseMenu::DrawMenuTitle(std::format("PLAYER {} START", GetScoreManager()->nStuntPlayerId+1));
			}
			else {
				CHUD_PauseMenu::DrawMenuTitle(std::format("ROUND {} START", GetScoreManager()->nStuntRoundId+1));
			}
		}
		else {
			CHUD_PauseMenu::DrawMenuTitle("EVENT START");
		}

		tNyaStringData data;
		data.x = gMenuText.nPosX;
		data.y = gMenuText.nPosY;
		data.size = gMenuText.fSize;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_CENTER, data, "PRESS START BUTTON", &DrawStringFO2_Ingame12);

		bMenuUp = false;
	}
} HUD_StartMenu;