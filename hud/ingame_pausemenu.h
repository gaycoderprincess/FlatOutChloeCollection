class CHUD_PauseMenu : public CIngameHUDElement {
public:
	virtual void Init() {
		nHUDLayer = eHUDLayer::OVERLAY;
		PreloadTexture("data/global/overlay/pausemenubg.png");
	}

	static inline bool bMenuUp = false;

	enum eSubMenu {
		SUBMENU_MAIN,
		SUBMENU_SOUNDOPTIONS,
		SUBMENU_RESTARTPROMPT,
		SUBMENU_QUITPROMPT,
	};
	static inline eSubMenu nSubMenu;
	static inline int nSelectedOption = 1;
	static inline int nMenuReturnValue = -1;
	static void EnterSubmenu(eSubMenu menu) {
		nSubMenu = menu;
		nSelectedOption = 1;
	}

	struct tOption {
		const char* name;
		void(*func)();
	};
	static inline tOption aOptionsMain[] = {
			{"PAUSED", nullptr},
			{"CONTINUE RACE", [](){ nMenuReturnValue = 0; }},
			{"RESTART RACE", [](){ EnterSubmenu(SUBMENU_RESTARTPROMPT); }},
			{"SOUND OPTIONS", [](){ EnterSubmenu(SUBMENU_SOUNDOPTIONS); }},
			{"EXIT TO MENU", [](){ EnterSubmenu(SUBMENU_QUITPROMPT); }},
			{nullptr, nullptr},
	};

	static inline tOption aOptionsSound[] = {
			{"SOUND OPTIONS", nullptr},
			{"MUSIC VOLUME", nullptr},
			{"SFX VOLUME", nullptr},
			{nullptr, nullptr},
	};

	static inline tOption aOptionsRestart[] = {
			{"RESTART RACE", nullptr},
			{"NO", [](){ EnterSubmenu(SUBMENU_MAIN); }},
			{"YES", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_RESTARTRACE; }},
			{"", nullptr},
			{"YOU WILL LOSE YOUR CURRENT RACE", nullptr},
			{"PROGRESS!", nullptr},
			{nullptr, nullptr},
	};

	static inline tOption aOptionsQuit[] = {
			{"EXIT TO MENU", nullptr},
			{"NO", [](){ EnterSubmenu(SUBMENU_MAIN); }},
			{"YES", [](){ nMenuReturnValue = IngameMenu::MENU_ACTION_QUITRACE; }},
			{"", nullptr},
			{"YOU WILL LOSE YOUR CURRENT RACE", nullptr},
			{"PROGRESS!", nullptr},
			{nullptr, nullptr},
	};

	tOption* GetOptions() {
		switch (nSubMenu) {
			case SUBMENU_MAIN:
				return aOptionsMain;
			case SUBMENU_SOUNDOPTIONS:
				return aOptionsSound;
			case SUBMENU_RESTARTPROMPT:
				return aOptionsRestart;
			case SUBMENU_QUITPROMPT:
				return aOptionsQuit;
		}
	}

	static inline tDrawPositions1080p gMenuTitle = {662,436,0.031};
	static inline tDrawPositions1080p gMenuOptions = {690,520,0.035,0,58};
	int nSliderLeft = 675;
	int nSliderRight = 1024;
	int nSliderTop = 24;
	int nSliderBottom = 58;

	void DrawMenuTitle() {
		tNyaStringData data;
		data.x = gMenuTitle.nPosX;
		data.y = gMenuTitle.nPosY;
		data.size = gMenuTitle.fSize;
		data.SetColor(GetPaletteColor(COLOR_INGAMEMENU_TITLE));
		Draw1080pString(JUSTIFY_CENTER, data, GetOptions()[0].name, &DrawStringFO2_Small);
	}

	int GetNumOptions() {
		auto options = GetOptions();
		int count = 0;
		while (true) {
			count++;
			if (!options[count].name || !options[count].name[0]) return count;
		};
	}

	virtual void Process() {
		static CNyaTimer gTimer;
		gTimer.Process();

		if (!IsRaceHUDUp() || !bMenuUp) {
			EnterSubmenu(SUBMENU_MAIN);
			nMenuReturnValue = -1;
			return;
		}

		static auto texture = LoadTextureFromBFS("data/global/overlay/pausemenubg.png");
		static auto textureBar = LoadTextureFromBFS("data/global/overlay/pausemenu_bar.png");
		static auto textureBarFill = LoadTextureFromBFS("data/global/overlay/pausemenu_bar_filled.png");
		Draw1080pSprite(JUSTIFY_CENTER, 0, 1920, 0, 1080, {255,255,255,255}, texture);

		auto selectedAlpha = GetFlashingAlpha(gTimer.fTotalTime) / 255.0;
		if (selectedAlpha < 0) selectedAlpha = 0;
		if (selectedAlpha > 1) selectedAlpha = 1;
		auto rgb1 = GetPaletteColor(COLOR_MENU_WHITE);
		auto rgb2 = GetPaletteColor(COLOR_MENU_YELLOW);
		NyaDrawing::CNyaRGBA32 rgbSelected;
		rgbSelected.r = std::lerp(rgb1.r, rgb2.r, selectedAlpha);
		rgbSelected.g = std::lerp(rgb1.g, rgb2.g, selectedAlpha);
		rgbSelected.b = std::lerp(rgb1.b, rgb2.b, selectedAlpha);
		rgbSelected.a = 255;

		DrawMenuTitle();

		auto options = GetOptions();
		bool afterEmpty = false;
		float ySpacing = 0;
		for (int i = 1; options[i].name; i++) {
			auto& option = options[i];
			if (!option.name[0]) {
				afterEmpty = true;
				ySpacing += 1;
				continue;
			}

			tNyaStringData data;
			data.x = gMenuOptions.nPosX;
			data.y = gMenuOptions.nPosY + gMenuOptions.nSpacingY * ySpacing;
			data.size = gMenuOptions.fSize;
			if (afterEmpty) {
				data.SetColor(GetPaletteColor(COLOR_INGAMEMENU_WARNING));
			}
			else if (i == nSelectedOption) {
				data.SetColor(rgbSelected);
			}
			else {
				data.SetColor(rgb1);
			}

			// hack for sliders
			if (!strcmp(option.name, "SFX VOLUME") || !strcmp(option.name, "MUSIC VOLUME")) {
				Draw1080pString(JUSTIFY_CENTER, data, option.name, &DrawStringFO2_Ingame12);

				int& value = !strcmp(option.name, "SFX VOLUME") ? nIngameSfxVolume : nIngameMusicVolume;

				float delta = value/100.0;
				Draw1080pSprite(JUSTIFY_CENTER, nSliderLeft, std::lerp(nSliderLeft, nSliderRight, delta), data.y + nSliderTop, data.y + nSliderBottom, {255,255,255,255}, textureBarFill, {0,0}, {delta, 1});
				Draw1080pSprite(JUSTIFY_CENTER, nSliderLeft, nSliderRight, data.y + nSliderTop, data.y + nSliderBottom, {255,255,255,255}, textureBar);

				if (i == nSelectedOption) {
					if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_LEFT)) {
						value -= 5;
						if (value < 0) value = 0;
					}

					if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_RIGHT)) {
						value += 5;
						if (value > 100) value = 100;
					}
				}

				ySpacing += 1;
			}
			else {
				Draw1080pString(JUSTIFY_CENTER, data, option.name, &DrawStringFO2_Ingame12);
			}
			ySpacing += 1;
		}

		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_UP)) {
			nSelectedOption--;
			if (nSelectedOption <= 0) {
				nSelectedOption = GetNumOptions()-1;
			}
		}

		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_DOWN)) {
			nSelectedOption++;
			if (!options[nSelectedOption].name || !options[nSelectedOption].name[0]) {
				nSelectedOption = 1;
			}
		}

		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_A) || pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_START)) {
			if (options[nSelectedOption].func) options[nSelectedOption].func();
		}

		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_SELECT)) {
			if (options != aOptionsMain) {
				EnterSubmenu(SUBMENU_MAIN);
			}
			else {
				nMenuReturnValue = IngameMenu::MENU_NONE;
			}
		}

		bMenuUp = false;
	}
} HUD_PauseMenu;