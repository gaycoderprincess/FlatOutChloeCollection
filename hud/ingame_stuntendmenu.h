class CHUD_StuntEndMenu : public CHUD_MenuGeneric<IngameMenu::MENU_STUNT_END_OF_ROUND> {
public:
	enum eSubMenu {
		SUBMENU_MAIN,
		SUBMENU_QUITPROMPT,
	};

	static bool IsLastStuntRound() {
		// last round for the last player
		if (GetScoreManager()->nStuntRoundId >= GetScoreManager()->nNumLaps-1) {
			if (pGameFlow->nGameMode != eGameMode::SPLITSCREEN) return true;
			if (GetScoreManager()->nStuntPlayerId >= pGameFlow->nNumSplitScreenPlayers - 1) return true;
		}
		return false;
	}

	static inline MenuOption* aOptionsMain[] = {
		new MenuOption(""),
		// todo does this work correctly in hotseat?
		new MenuOptionSelectable("CONTINUE EVENT", [](){ nMenuReturnValue = IsLastStuntRound() ? IngameMenu::MENU_RACE_RESULTS : IngameMenu::MENU_ACTION_RESTARTRACE; }),
		new MenuOptionSelectable("VIEW REPLAY", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_VIEWREPLAY; }),
		new MenuOptionSelectable("EXIT TO MENU", [](){ EnterSubmenu(SUBMENU_QUITPROMPT); }),
		nullptr
	};

	static inline MenuOption* aOptionsQuit[] = {
			new MenuOption("EXIT TO MENU"),
			new MenuOptionSelectable("NO", [](){ EnterSubmenu(SUBMENU_MAIN); }),
			new MenuOptionSelectable("YES", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_QUITRACE; }),
			new MenuOption(""),
			new MenuOption("YOU WILL LOSE YOUR CURRENT PROGRESS!"),
			nullptr
	};

	virtual MenuOption** GetOptions(int menu) {
		switch (menu) {
			case SUBMENU_MAIN:
			default:
				return aOptionsMain;
			case SUBMENU_QUITPROMPT:
				return aOptionsQuit;
		}
	}

	virtual void Process() {
		return CHUD_MenuGeneric::Process();
	}
} HUD_StuntEndMenu;