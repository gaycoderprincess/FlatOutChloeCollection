template<int menuType> // template hack for static variables
class CHUD_MenuGeneric : public CIngameHUDElement {
public:
	void Init() override {
		nHUDLayer = eHUDLayer::OVERLAY;
		PreloadTexture("data/global/overlay/pausemenubg.tga");
		PreloadTexture("data/global/overlay/pausemenu_bar.tga");
		PreloadTexture("data/global/overlay/pausemenu_bar_filled.tga");
	}

	constexpr static inline bool bCenteredOptions = false;

	constexpr static inline int nBoxCenter = 955;
	constexpr static inline int nBoxTrueCenter = 969;
	static inline tDrawPositions1080p gMenuTitle = {662,436,0.031};
	static inline tDrawPositions1080p gMenuOptions = {690,520,0.035,0,58};
	constexpr static inline int nSliderLeft = 675;
	constexpr static inline int nSliderRight = 1024;
	constexpr static inline int nSliderTop = 24;
	constexpr static inline int nSliderBottom = 58;

	static inline bool bMenuUp = false;
	static inline int nMenuReturnValue = -1;

	static inline int nSubMenu;
	static inline int nSelectedOption = 1;
	static void EnterSubmenu(int menu) {
		nSubMenu = menu;
		nSelectedOption = 1;
	}

	class MenuOption {
	public:
		std::string sName;

		MenuOption(const std::string& name) {
			sName = name;
		}

		virtual bool IsSlider() { return false; }
		virtual bool IsPlaylist() { return false; }
		virtual int GetValue() { return 0; }
		virtual float GetValuePercent() { return 0; }

		virtual void OnSelected() {}
		virtual void OnLeft() {}
		virtual void OnRight() {}
	};

	class MenuOptionSelectable : public MenuOption {
	protected:
		void(*pFunction)();

	public:
		MenuOptionSelectable(const std::string& name, void(*func)()) : MenuOption(name) {
			pFunction = func;
		}
		virtual void OnSelected() {
			pFunction();
		}
	};

	class MenuOptionSlider : public MenuOption {
	protected:
		int* nValue;
		int nIncrementValue;
		int nMinValue;
		int nMaxValue;

	public:
		MenuOptionSlider(const std::string& name, int* value, int increment, int min, int max) : MenuOption(name) {
			nValue = value;
			nIncrementValue = increment;
			nMinValue = min;
			nMaxValue = max;
		}

		virtual bool IsSlider() { return true; }
		virtual int GetValue() { return *nValue; }
		virtual float GetValuePercent() { return ((*nValue) - nMinValue) / (double)(nMaxValue - nMinValue); }

		virtual void OnLeft() {
			if (*nValue <= nMinValue) return;
			*nValue -= nIncrementValue;
		}
		virtual void OnRight() {
			if (*nValue >= nMaxValue) return;
			*nValue += nIncrementValue;
		}
	};

	class MenuOptionPlaylist : public MenuOption {
	public:
		MenuOptionPlaylist(const std::string& name) : MenuOption(name) { }

		virtual bool IsPlaylist() { return true; }

		virtual void OnLeft() {
			auto& value = pGameFlow->nEventType == eEventType::DERBY ? nIngameDerbySoundtrack : nIngameSoundtrack;
			value--;
			if (value < 0) value = NewMusicPlayer::aPlaylistsIngame.size()-1;
			NewMusicPlayer::StopPlayback();
		}
		virtual void OnRight() {
			auto& value = pGameFlow->nEventType == eEventType::DERBY ? nIngameDerbySoundtrack : nIngameSoundtrack;
			value++;
			if (value >= NewMusicPlayer::aPlaylistsIngame.size()) value = 0;
			NewMusicPlayer::StopPlayback();
		}
	};

	virtual MenuOption** GetOptions(int menu) = 0;

	static void DrawMenuTitle(const std::string& title) {
		tNyaStringData data;
		data.x = gMenuTitle.nPosX;
		data.y = gMenuTitle.nPosY;
		data.size = gMenuTitle.fSize;
		data.SetColor(GetPaletteColor(COLOR_INGAMEMENU_TITLE));
		Draw1080pString(JUSTIFY_CENTER, data, title, &DrawStringFO2_Regular18);
	}

	int GetNumOptions() {
		auto options = GetOptions(nSubMenu);
		int count = 0;
		while (true) {
			count++;
			if (!options[count] || options[count]->sName.empty()) return count;
		};
	}

	static void DrawBackground() {
		static auto texture = LoadTextureFromBFS("data/global/overlay/pausemenubg.tga");
		Draw1080pSprite(JUSTIFY_CENTER, 0, 1920, 0, 1080, {255,255,255,255}, texture);
	}

	bool DrawInReplay() override { return true; }

	void Reset() override {
		bMenuUp = false;
	}

	void Process() override {
		static CNyaTimer gTimer;
		gTimer.Process();

		if (!bMenuUp) {
			EnterSubmenu(0);
			nMenuReturnValue = -1;
			return;
		}

		DrawBackground();

		static auto textureBar = LoadTextureFromBFS("data/global/overlay/pausemenu_bar.tga");
		static auto textureBarFill = LoadTextureFromBFS("data/global/overlay/pausemenu_bar_filled.tga");

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

		DrawMenuTitle(GetOptions(nSubMenu)[0]->sName);

		auto options = GetOptions(nSubMenu);
		bool afterEmpty = false;
		float ySpacing = 0;
		for (int i = 1; options[i]; i++) {
			auto& option = options[i];
			if (option->sName.empty()) {
				afterEmpty = true;
				ySpacing += 1.5;
				continue;
			}

			tNyaStringData data;
			if (bCenteredOptions) {
				if (option->IsSlider()) {
					data.x = gMenuOptions.nPosX;
					data.XCenterAlign = false;
				}
				else {
					data.x = afterEmpty ? nBoxTrueCenter : nBoxCenter;
					data.XCenterAlign = true;
				}
			}
			else {
				data.x = gMenuOptions.nPosX;
			}
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
			if (option->IsPlaylist()) {
				Draw1080pString(JUSTIFY_CENTER, data, std::format("{} < {} >", option->sName, GetStringNarrow(NewMusicPlayer::pCurrentPlaylist->wsName)), &DrawStringFO2_Condensed12);
			}
			else if (option->IsSlider()) {
				Draw1080pString(JUSTIFY_CENTER, data, option->sName, &DrawStringFO2_Condensed12);

				float delta = option->GetValuePercent();
				Draw1080pSprite(JUSTIFY_CENTER, nSliderLeft, std::lerp(nSliderLeft, nSliderRight, delta), data.y + nSliderTop, data.y + nSliderBottom, {255,255,255,255}, textureBarFill, {0,0}, {delta, 1});
				Draw1080pSprite(JUSTIFY_CENTER, nSliderLeft, nSliderRight, data.y + nSliderTop, data.y + nSliderBottom, {255,255,255,255}, textureBar);

				ySpacing += 1;
			}
			else {
				Draw1080pString(JUSTIFY_CENTER, data, option->sName, &DrawStringFO2_Condensed12);
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
			if (!options[nSelectedOption] || options[nSelectedOption]->sName.empty()) {
				nSelectedOption = 1;
			}
		}

		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_LEFT)) {
			options[nSelectedOption]->OnLeft();
		}

		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_RIGHT)) {
			options[nSelectedOption]->OnRight();
		}

		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_A) || pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_START)) {
			options[nSelectedOption]->OnSelected();
		}

		if (pInputManager->IsKeyJustPressed(CONTROLLER_BUTTON_SELECT)) {
			if (options != GetOptions(0)) {
				EnterSubmenu(0);
			}
			else if (menuType == IngameMenu::MENU_PAUSEMENU) {
				nMenuReturnValue = IngameMenu::MENU_NONE;
			}
			else if (menuType == IngameMenu::MENU_END_OF_RACE) {
				if (pGameFlow->nIsInReplay) {
					nMenuReturnValue = IngameMenu::MENU_REPLAY_UI;
				}
				// if not in replay, return to race results (only happens in MP)
				else if (pGameFlow->nEventType != eEventType::STUNT) {
					nMenuReturnValue = IngameMenu::MENU_RACE_RESULTS;
				}
			}
		}

		if (nMenuReturnValue == IngameMenu::MENU_ACTION_RESTARTRACE) {
			ChloeEvents::RaceRestartEvent.OnHit();
		}

		bMenuUp = false;
	}
};
