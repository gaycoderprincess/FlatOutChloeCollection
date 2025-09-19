class CHUD_PauseMenu : public CHUD_MenuGeneric<IngameMenu::MENU_PAUSEMENU> {
public:
	enum eSubMenu {
		SUBMENU_MAIN,
		SUBMENU_SOUNDOPTIONS,
		SUBMENU_RESTARTPROMPT,
		SUBMENU_QUITPROMPT,
	};

	static inline MenuOption* aOptionsMain[] = {
		new MenuOption("PAUSED"),
		new MenuOptionSelectable("CONTINUE EVENT", [](){ nMenuReturnValue = 0; }),
		new MenuOptionSelectable("RETRY EVENT", [](){ EnterSubmenu(SUBMENU_RESTARTPROMPT); }),
		new MenuOptionSelectable("SOUND OPTIONS", [](){ EnterSubmenu(SUBMENU_SOUNDOPTIONS); }),
		new MenuOptionSelectable("EXIT TO MENU", [](){ EnterSubmenu(SUBMENU_QUITPROMPT); }),
		nullptr
	};

	static inline MenuOption* aOptionsMainNoRestart[] = {
		new MenuOption("PAUSED"),
		new MenuOptionSelectable("CONTINUE EVENT", [](){ nMenuReturnValue = 0; }),
		new MenuOptionSelectable("SOUND OPTIONS", [](){ EnterSubmenu(SUBMENU_SOUNDOPTIONS); }),
		new MenuOptionSelectable("EXIT TO MENU", [](){ EnterSubmenu(SUBMENU_QUITPROMPT); }),
		nullptr
	};

	static inline MenuOption* aOptionsSound[] = {
			new MenuOption("SOUND OPTIONS"),
			new MenuOptionSlider("MUSIC VOLUME", &nIngameMusicVolume, 5, 0, 100),
			new MenuOptionSlider("SFX VOLUME", &nIngameSfxVolume, 5, 0, 100),
			new MenuOptionPlaylist("PLAYLIST"),
			nullptr
	};

	static inline MenuOption* aOptionsSoundStunt[] = {
			new MenuOption("SOUND OPTIONS"),
			new MenuOptionSlider("MUSIC VOLUME", &nIngameMusicVolume, 5, 0, 100),
			new MenuOptionSlider("SFX VOLUME", &nIngameSfxVolume, 5, 0, 100),
			nullptr
	};

	static inline MenuOption* aOptionsRestart[] = {
			new MenuOption("RETRY EVENT"),
			new MenuOptionSelectable("NO", [](){ EnterSubmenu(SUBMENU_MAIN); }),
			new MenuOptionSelectable("YES", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_RESTARTRACE; }),
			new MenuOption(""),
			new MenuOption("YOU WILL LOSE YOUR CURRENT PROGRESS!"),
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

	MenuOption** GetOptions(int menu) override {
		switch (menu) {
			case SUBMENU_MAIN:
			default:
			{
				// stunts continue to the next round if you restart them
				if (pGameFlow->nEventType == eEventType::STUNT || bIsInMultiplayer) return aOptionsMainNoRestart;
				return aOptionsMain;
			}
			case SUBMENU_SOUNDOPTIONS: {
				if (pGameFlow->nEventType == eEventType::STUNT) return aOptionsSoundStunt;
				return aOptionsSound;
			}
			case SUBMENU_RESTARTPROMPT:
				return aOptionsRestart;
			case SUBMENU_QUITPROMPT:
				return aOptionsQuit;
		}
	}
} HUD_PauseMenu;