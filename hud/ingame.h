enum class eHUDLayer {
	WORLD, // world UI, e.g. 3d player icons
	BASE, // normal UI
	FADE, // car reset fade
	OVERLAY, // pause menu, tutorial screen, etc.
	NUM_LAYERS
};

class CIngameHUDElement : public CHUDElement {
public:
	static inline std::vector<CIngameHUDElement*> aGameHUD;

	eHUDLayer nHUDLayer = eHUDLayer::BASE;

	CIngameHUDElement() {
		aGameHUD.push_back(this);
	}

	static bool IsRaceHUDUp() {
		if (GetScoreManager()->nHideRaceHUD) return false;
		return true;
	}

	static inline tDrawPositions gElementBase = {0.008, 0.029, 0.042, 0, 0.034};
	static inline float fElementTotalSpacing = 0.092;

	static void DrawElement(float y, const std::string& title, const std::string& value, const NyaDrawing::CNyaRGBA32 rgb = {255,255,255,255}) {
		tNyaStringData data;
		data.x = gElementBase.fPosX * GetAspectRatioInv();
		data.y = gElementBase.fPosY + y * fElementTotalSpacing;
		data.size = gElementBase.fSize;
		data.SetColor(GetPaletteColor(COLOR_INGAME_YELLOW));
		data.a = rgb.a;
		DrawStringFO2_Ingame12(data, title);
		data.y += gElementBase.fSpacingY;
		data.SetColor(rgb);
		DrawStringFO2_Ingame24(data, value);
	}

	static void DrawElementCustomX(eJustify justify, int x, float y, const std::string& title, const std::string& value, const NyaDrawing::CNyaRGBA32 rgb = {255,255,255,255}) {
		float x2 = x, tmp;
		DoJustify(justify, x2, tmp);

		tNyaStringData data;
		data.x = x2;
		data.y = gElementBase.fPosY + y * fElementTotalSpacing;
		data.size = gElementBase.fSize;
		data.SetColor(GetPaletteColor(COLOR_INGAME_YELLOW));
		data.a = rgb.a;
		DrawStringFO2_Ingame12(data, title);
		data.y += gElementBase.fSpacingY;
		data.SetColor(rgb);
		DrawStringFO2_Ingame24(data, value);
	}

	static void DrawElementCenter(float y, const std::string& title, const std::string& value, const NyaDrawing::CNyaRGBA32 rgb = {255,255,255,255}) {
		tNyaStringData data;
		data.x = 0.5;
		data.y = gElementBase.fPosY + y * fElementTotalSpacing;
		data.size = gElementBase.fSize;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_INGAME_YELLOW));
		data.a = rgb.a;
		DrawStringFO2_Ingame12(data, title);
		data.y += gElementBase.fSpacingY;
		data.SetColor(rgb);
		DrawStringFO2_Ingame24(data, value);
	}

	static void DrawElementRight(float y, const std::string& title, const std::string& value, const NyaDrawing::CNyaRGBA32 rgb = {255,255,255,255}) {
		tNyaStringData data;
		data.x = 1.0 - (gElementBase.fPosX * GetAspectRatioInv());
		data.y = gElementBase.fPosY + y * fElementTotalSpacing;
		data.size = gElementBase.fSize;
		data.XRightAlign = true;
		data.SetColor(GetPaletteColor(COLOR_INGAME_YELLOW));
		data.a = rgb.a;
		DrawStringFO2_Ingame12(data, title);
		data.y += gElementBase.fSpacingY;
		data.SetColor(rgb);
		DrawStringFO2_Ingame24(data, value);
	}

	virtual bool DrawInReplay() { return false; }
};

namespace NewGameHud {
	void OnTick() {
		auto state = GetGameState();
		if (state == GAME_STATE_RACE) return;

		for (auto& hud : CIngameHUDElement::aGameHUD) {
			if ((state == GAME_STATE_REPLAY && !hud->DrawInReplay()) || state == GAME_STATE_MENU) hud->Reset();
		}
	}

	void OnHUDTick() {
		if (pLoadingScreen) return;
		auto state = GetGameState();
		if (state != GAME_STATE_RACE && state != GAME_STATE_REPLAY) return;

		for (int i = 0; i < (int)eHUDLayer::NUM_LAYERS; i++) {
			for (auto& hud: CIngameHUDElement::aGameHUD) {
				if (hud->nHUDLayer != (eHUDLayer)i) continue;
				if (state == GAME_STATE_REPLAY && !hud->DrawInReplay()) continue;
				hud->Process();
			}
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
		static bool bInited = false;
		if (bInited) return;
		bInited = true;

		for (auto& hud : CIngameHUDElement::aGameHUD) {
			hud->Init();
		}

		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4695C5, &OnMinimapLoadASM);

		NyaHookLib::Patch(0x4539D4 + 2, &fMusicPlayer416);
		NyaHookLib::Patch(0x4539F4 + 2, &fMusicPlayer480);
		NyaHookLib::Patch(0x453A52 + 2, &fMusicPlayer428);
		ChloeEvents::DrawRaceUIEvent.AddHandler(OnHUDTick);
		ChloeEvents::FinishFrameEvent.AddHandler(OnTick);
	}
}

#include "ingame_playerlist.h"
#include "ingame_damagemeter.h"
#include "ingame_contacttimer.h"
#include "ingame_wrecked.h"
#include "ingame_arcademode.h"
#include "ingame_tutorial.h"
#include "ingame_laptime.h"
#include "ingame_resetfade.h"
#include "ingame_minimap.h"
#include "ingame_menugeneric.h"
#include "ingame_pausemenu.h"
#include "ingame_stuntendmenu.h"
#include "ingame_raceendmenu.h"
#include "ingame_stuntresultsmenu.h"
#include "ingame_startmenu.h"
#include "ingame_raceresults.h"
#include "ingame_derby_overhead.h"