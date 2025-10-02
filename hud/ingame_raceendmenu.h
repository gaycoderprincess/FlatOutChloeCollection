class CHUD_RaceEndMenu : public CHUD_MenuGeneric<IngameMenu::MENU_END_OF_RACE> {
public:
	enum eSubMenu {
		SUBMENU_MAIN,
		SUBMENU_RESTARTPROMPT,
	};

	static inline bool bSpectating = false;

	static inline MenuOption* aOptionsMain[] = {
		new MenuOption("EVENT FINISHED"),
		new MenuOptionSelectable("EXIT TO MENU", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_QUITRACE; }),
		new MenuOptionSelectable("RETRY EVENT", [](){ EnterSubmenu(SUBMENU_RESTARTPROMPT); }),
		nullptr
	};

	static inline MenuOption* aOptionsMainNoRestart[] = {
			new MenuOption("EVENT FINISHED"),
			new MenuOptionSelectable("EXIT TO MENU", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_QUITRACE; }),
			nullptr
	};

	static inline MenuOption* aOptionsMainMultiplayer[] = {
		new MenuOption("EVENT FINISHED"),
		new MenuOptionSelectable("VIEW RACE", [](){ ChloeNet::SetSpectate(true); bSpectating = true; }),
		new MenuOptionSelectable("EXIT TO MENU", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_QUITRACE; }),
		nullptr
	};

	static inline MenuOption* aOptionsRetry[] = {
		new MenuOption("RETRY EVENT"),
		new MenuOptionSelectable("NO", [](){ EnterSubmenu(SUBMENU_MAIN); }),
		new MenuOptionSelectable("YES", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_RESTARTRACE; }),
		new MenuOption(""),
		new MenuOption("YOU WILL LOSE YOUR CURRENT PROGRESS!"),
		nullptr
	};

	MenuOption** GetOptions(int menu) override {
		switch (menu) {
			case SUBMENU_MAIN:
			default:
			{
				if (bIsInMultiplayer) return ChloeNet::CanSpectate() ? aOptionsMainMultiplayer : aOptionsMainNoRestart;
				return aOptionsMain;
			}
			case SUBMENU_RESTARTPROMPT:
				return aOptionsRetry;
		}
	}

	void Process() override {
		if (!bMenuUp) {
			bSpectating = false;
		}

		if (bSpectating) {
			if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_SELECT) || !ChloeNet::CanSpectate()) {
				ChloeNet::SetSpectate(false);
				bSpectating = false;
			}
		}
		else {
			CHUD_MenuGeneric::Process();
		}
	}
} HUD_RaceEndMenu;