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

	void Init() {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x459FCF, &DrawPauseMenuASM);
	}
}