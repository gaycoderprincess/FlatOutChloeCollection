namespace NewIngameMenu {
	int __fastcall DrawPauseMenu(IngameMenu* menu) {
		HUD_PauseMenu.bMenuUp = true;
		if (HUD_PauseMenu.nMenuReturnValue >= 0) {
			auto value = HUD_PauseMenu.nMenuReturnValue;
			HUD_PauseMenu.nMenuReturnValue = -1;
			return value;
		}
		return IngameMenu::MENU_PAUSEMENU;
	}

	void __attribute__((naked)) DrawPauseMenuASM() {
		__asm__ (
			"mov ecx, eax\n\t"
			"jmp %0\n\t"
				:
				:  "i" (DrawPauseMenu)
		);
	}

	int __fastcall DrawPressStartMenu(IngameMenu* menu) {
		if (bIsInMultiplayer) return IngameMenu::MENU_RACE_COUNTDOWN;

		HUD_StartMenu.bMenuUp = true;
		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_A) || pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_START)) {
			pPlayerHost->StartRace();
			return IngameMenu::MENU_RACE_COUNTDOWN;
		}
		return IngameMenu::MENU_PRESS_START;
	}

	void __attribute__((naked)) DrawPressStartMenuASM() {
		__asm__ (
			"mov ecx, ebx\n\t"
			"jmp %0\n\t"
				:
				:  "i" (DrawPressStartMenu)
		);
	}

	void Init() {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x459FCF, &DrawPauseMenuASM);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x459F79, &DrawPressStartMenuASM);
	}
}