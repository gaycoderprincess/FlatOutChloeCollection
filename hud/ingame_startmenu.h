class CHUD_StartMenu : public CIngameHUDElement {
public:
	virtual void Init() {
		nHUDLayer = eHUDLayer::OVERLAY;
	}

	static inline tDrawPositions1080p gMenuText = {955,620,0.04};

	static inline bool bMenuUp = false;

	virtual void Process() {
		if (!bMenuUp || ArcadeMode::bIsArcadeMode) return;

		HUD_PauseMenu.DrawBackground();
		if (pGameFlow->nEventType == eEventType::STUNT) {
			if (pGameFlow->nGameMode == eGameMode::SPLITSCREEN) {
				HUD_PauseMenu.DrawMenuTitle(std::format("PLAYER {} START", GetScoreManager()->nStuntPlayerId+1));
			}
			else {
				HUD_PauseMenu.DrawMenuTitle(std::format("ROUND {} START", GetScoreManager()->nStuntRoundId+1));
			}
		}
		else {
			HUD_PauseMenu.DrawMenuTitle("EVENT START");
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