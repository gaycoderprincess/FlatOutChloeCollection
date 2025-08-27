namespace NewIngameMenu {
	int __fastcall DrawPressStartMenu(IngameMenu* menu) {
		if (!ArcadeMode::bIsArcadeMode) return menu->DrawPressStart();

		if (pInputManager->IsKeyJustPressed(0) || pInputManager->IsKeyJustPressed(8)) {
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
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x459F79, &DrawPressStartMenuASM);
	}
}