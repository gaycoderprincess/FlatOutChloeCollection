class CIngameHUDElement {
public:
	static inline std::vector<CIngameHUDElement*> aGameHUD;

	CIngameHUDElement() {
		aGameHUD.push_back(this);
	}

	bool IsRaceHUDUp() {
		if (GetScoreManager()->nHideRaceHUD) return false;
		return true;
	}

	virtual void Reset() {}
	virtual void Process() = 0;
};

namespace NewGameHud {
	void OnTick() {
		if (pLoadingScreen) return;
		if (GetGameState() != GAME_STATE_RACE) {
			for (auto& hud : CIngameHUDElement::aGameHUD) {
				hud->Reset();
			}
			return;
		}

		for (auto& hud : CIngameHUDElement::aGameHUD) {
			hud->Process();
		}
	}

	float fMapOffset = -50;
	void OnMinimapLoad() {
		pEnvironment->pMinimap->fScreenPos[1] += fMapOffset;
	}

	uintptr_t OnMinimapLoadASM_jmp = 0x4695CB;
	void __attribute__((naked)) OnMinimapLoadASM() {
		__asm__ (
			"mov [eax+0x15C], edx\n\t"

			"pushad\n\t"
			"mov ecx, ebx\n\t"
			"call %1\n\t"
			"popad\n\t"

			"jmp %0\n\t"
				:
				:  "m" (OnMinimapLoadASM_jmp), "i" (OnMinimapLoad)
		);
	}

	float fMusicPlayerOffset = -410;
	float fMusicPlayer416 = 416 + fMusicPlayerOffset;
	float fMusicPlayer480 = 480 + fMusicPlayerOffset;
	float fMusicPlayer428 = 428 + fMusicPlayerOffset;
	void Init() {
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4695C5, &OnMinimapLoadASM);

		NyaHookLib::Patch(0x4539D4 + 2, &fMusicPlayer416);
		NyaHookLib::Patch(0x4539F4 + 2, &fMusicPlayer480);
		NyaHookLib::Patch(0x453A52 + 2, &fMusicPlayer428);
	}
}