namespace NewIngameMenu {
	int DrawPauseMenu() {
		HUD_PauseMenu.bMenuUp = true;
		if (HUD_PauseMenu.nMenuReturnValue >= 0) {
			auto value = HUD_PauseMenu.nMenuReturnValue;
			HUD_PauseMenu.nMenuReturnValue = -1;
			return value;
		}
		return IngameMenu::MENU_PAUSEMENU;
	}

	int DrawStuntEndOfRoundMenu() {
		HUD_StuntEndMenu.bMenuUp = true;
		if (HUD_StuntEndMenu.nMenuReturnValue >= 0) {
			auto value = HUD_StuntEndMenu.nMenuReturnValue;
			HUD_StuntEndMenu.nMenuReturnValue = -1;
			return value;
		}
		return IngameMenu::MENU_STUNT_END_OF_ROUND;
	}

	int DrawRaceEndMenu() {
		HUD_RaceEndMenu.bMenuUp = true;
		if (HUD_RaceEndMenu.nMenuReturnValue >= 0) {
			auto value = HUD_RaceEndMenu.nMenuReturnValue;
			HUD_RaceEndMenu.nMenuReturnValue = -1;
			return value;
		}
		return IngameMenu::MENU_END_OF_RACE;
	}

	int DrawPressStartMenu() {
		if (bIsInMultiplayer) {
			pPlayerHost->StartRace();
			return IngameMenu::MENU_RACE_COUNTDOWN;
		}

		HUD_StartMenu.bMenuUp = true;
		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_A) || pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_START)) {
			pPlayerHost->StartRace();
			return IngameMenu::MENU_RACE_COUNTDOWN;
		}
		return IngameMenu::MENU_PRESS_START;
	}

	int __stdcall DrawRaceResultsMenu(int) {
		if (pGameFlow->nEventType == eEventType::STUNT) {
			HUD_StuntResultsMenu.bMenuUp = true;
			if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_A) || pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_START)) {
				return IngameMenu::MENU_END_OF_RACE;
			}
		}
		else {
			GetScoreManager()->nHideRaceHUD = 1;

			HUD_RaceResults.bMenuUp = true;
			if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_A) || pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_START)) {
				return bIsInMultiplayer ? IngameMenu::MENU_END_OF_RACE : IngameMenu::MENU_ACTION_VIEWREPLAY;
			}
		}
		return IngameMenu::MENU_RACE_RESULTS;
	}

	void DrawDerbyTutorialMenu() {
		HUD_Tutorial.bMenuUpInNormalMode = true;
	}

	void Init() {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x459FCF, &DrawPauseMenu);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x459F79, &DrawPressStartMenu);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x45A00E, &DrawStuntEndOfRoundMenu);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x459FD8, &DrawRaceEndMenu);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x45A01D, &DrawRaceResultsMenu);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x45B092, &DrawDerbyTutorialMenu);
	}
}