class CMenu_LoadingScreen : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_loadingscreen"; }

	static constexpr float fLoadingSpriteX = 0.07;
	static constexpr float fLoadingSpriteY = 0.8;
	static constexpr float fLoadingSpriteSize = 0.15;

	static constexpr float nLoadingLogoY = 64;

	static constexpr int nLoadingTextX = 1650;
	static constexpr int nLoadingTextY = 980;
	static constexpr float fLoadingTextSize = 0.04;

	static inline std::string sTextureName = "data/menu/loading.tga";
	static const char* GetLoadingScreenForTrack(int track) {
		switch (track) {
			case TRACK_FOREST1A: return "forest1_1";
			case TRACK_FOREST1B: return "forest1_2";
			case TRACK_FOREST1C: return "forest1_3";
			case TRACK_FOREST2A: return "forest2_1";
			case TRACK_FOREST2B: return "forest2_2";
			case TRACK_FOREST2C: return "forest2_3";
			case TRACK_FOREST3A: return "forest3_1";
			case TRACK_FOREST3B: return "forest3_2";
			case TRACK_FOREST3C: return "forest3_3";
			case TRACK_PIT1A: return "pit1_1";
			case TRACK_PIT1B: return "pit1_2";
			case TRACK_PIT1C: return "pit1_3";
			case TRACK_PIT2A: return "pit2_1";
			case TRACK_PIT2B: return "pit2_2";
			case TRACK_PIT2C: return "pit2_3";
			case TRACK_PIT3A: return "pit3_1";
			case TRACK_PIT3B: return "pit3_2";
			case TRACK_PIT3C: return "pit3_3";
			case TRACK_RACING1A: return "racing1_1";
			case TRACK_RACING1B: return "racing1_2";
			case TRACK_RACING1C: return "racing1_3";
			case TRACK_RACING2A: return "racing2_1";
			case TRACK_RACING2B: return "racing2_2";
			case TRACK_RACING2C: return "racing2_3";
			case TRACK_TOWN2A: return "town2_1";
			case TRACK_TOWN2B: return "town2_2";
			case TRACK_TOWN2C: return "town2_3";
			case TRACK_TOWN3A: return "town3_1";
			case TRACK_TOWN3B: return "town3_2";
			case TRACK_TOWN3C: return "town3_3";
			case TRACK_WINTER1A: return "winter1_1";
			case TRACK_WINTER1B: return "winter1_2";
			case TRACK_WINTER1C: return "winter1_3";
			case TRACK_WINTER2A: return "winter2_1";
			case TRACK_WINTER2B: return "winter2_2";
			case TRACK_WINTER2C: return "winter2_3";
			case TRACK_ARENA1A: return "arena1";
			case TRACK_ARENA2A: return "arena2";
			case TRACK_ARENA3A: return "arena3";
			case TRACK_DERBY1A: return "derby1";
			case TRACK_DERBY2A: return "derby2";
			case TRACK_DERBY3A: return "derby3";
			case TRACK_LONGJUMP: return "longjump";
			case TRACK_HIGHJUMP: return "highjump";
			case TRACK_BULLSEYE: return "bullseye";
			case TRACK_BOWLING: return "bowling";
			case TRACK_DARTS: return "darts";
			case TRACK_CLOWN: return "clown";
			default:
				return nullptr;
		}
	}

	virtual void Process() {
		if (!nUseNewLoadingScreen) return;

		if (!pLoadingScreen) return;
		if (sTextureName.empty()) return;

		auto tex = LoadTextureFromBFS(sTextureName.c_str());
		if (!tex) return;

		static auto loadingAnim = LoadTextureFromBFS("data/menu/loading_anim.tga");
		static auto gameLogo = LoadTextureFromBFS("data/menu/flatout_logo.png");
		static std::vector<tHUDData> loadingAnims = LoadHUDData("data/menu/loading_anim.bed", "loading_anim");

		// assuming a size of 640x480
		float aspectModifier = (GetAspectRatio() / (4.0 / 3.0)) - 1.0;
		if (sTextureName == "data/menu/loading.tga") aspectModifier = 0.0;
		DrawRectangle(0, 1, 0 - (aspectModifier * 0.5), 1 + (aspectModifier * 0.5), {255,255,255,255}, 0, tex);

		const float fLoadingTimerSpeed = 0.4;

		static CNyaTimer gTimer;

		static int nLoadingSprite = 0;
		static double fLoadingTimer = 0;
		fLoadingTimer += gTimer.Process();
		while (fLoadingTimer > fLoadingTimerSpeed) {
			nLoadingSprite++;
			if (nLoadingSprite > 3) nLoadingSprite = 0;
			fLoadingTimer -= fLoadingTimerSpeed;
		}

		DrawRectangle(1.0 - ((fLoadingSpriteX + fLoadingSpriteSize) * GetAspectRatioInv()), 1.0 - (fLoadingSpriteX * GetAspectRatioInv()), fLoadingSpriteY, fLoadingSpriteY + fLoadingSpriteSize, {255,255,255,255}, 0, loadingAnim, 0, loadingAnims[nLoadingSprite].min, loadingAnims[nLoadingSprite].max);

		Draw1080pSprite(JUSTIFY_CENTER, 960 - 256, 960 + 256, nLoadingLogoY, nLoadingLogoY + 166, {255,255,255,255}, gameLogo);

		tNyaStringData data;
		data.x = nLoadingTextX;
		data.y = nLoadingTextY;
		data.size = fLoadingTextSize;
		data.XRightAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, "LOADING", &DrawStringFO2_Small);
	}

	static void OnLoadToMenu() {
		// set image to loading.tga for going to the menu
		sTextureName = "data/menu/loading.tga";
	}

	static void OnLoadToRace() {
		// set image to track loading when loading into a race
		auto loading = GetLoadingScreenForTrack(pGameFlow->nLevel);
		if (loading) sTextureName = std::format("data/menu/bg/{}.tga", loading);
	}

	static inline uintptr_t OnLoadToMenuASM_jmp = 0x4C0610;
	static void __attribute__((naked)) OnLoadToMenuASM() {
		__asm__ (
			"pushad\n\t"
			"mov ecx, ebx\n\t"
			"call %1\n\t"
			"popad\n\t"

			"jmp %0\n\t"
				:
				:  "m" (OnLoadToMenuASM_jmp), "i" (OnLoadToMenu)
		);
	}

	static inline uintptr_t OnLoadToRaceASM_jmp = 0x4C0610;
	static void __attribute__((naked)) OnLoadToRaceASM() {
		__asm__ (
			"pushad\n\t"
			"mov ecx, ebx\n\t"
			"call %1\n\t"
			"popad\n\t"

			"jmp %0\n\t"
				:
				:  "m" (OnLoadToRaceASM_jmp), "i" (OnLoadToRace)
		);
	}

	virtual void Init() {
		OnLoadToMenuASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x452458, &OnLoadToMenuASM);
		OnLoadToRaceASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x451CAE, &OnLoadToRaceASM);
	}
} Menu_LoadingScreen;