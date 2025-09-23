class CMenuHUDElement : public CHUDElement {
public:
	static inline std::vector<CMenuHUDElement*> aMenuHUD;

	CMenuHUDElement() {
		aMenuHUD.push_back(this);
	}

	virtual const char* GetName() = 0;
	virtual void MoveLeft() {};
	virtual void MoveRight() {};
	virtual void MoveUp() {};
	virtual void MoveDown() {};
	virtual void Select() {};
};

namespace ChloeMenuHud {
	void OnTick() {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			if (!hud->bEnabled) hud->Reset();
			hud->Process();
		}
	}

	void Init() {
		static bool bInited = false;
		if (bInited) return;
		bInited = true;

		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			hud->Init();
		}
	}

	void InitHooks() {
		static bool bInited = false;
		if (bInited) return;
		bInited = true;

		ChloeEvents::DrawAboveUIEvent.AddHandler(OnTick);
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			hud->InitHooks();
		}
	}

	void EnterMenu(CMenuHUDElement* menu) {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			hud->bEnabled = false;
		}
		if (menu) menu->bEnabled = true;
	}

	void EnterMenu(const std::string& menu) {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			hud->bEnabled = hud->GetName() == menu;
		}
	}

	void OnMoveUp() {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			if (hud->bEnabled) hud->MoveUp();
		}
	}

	void OnMoveDown() {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			if (hud->bEnabled) hud->MoveDown();
		}
	}

	void OnMoveLeft() {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			if (hud->bEnabled) hud->MoveLeft();
		}
	}

	void OnMoveRight() {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			if (hud->bEnabled) hud->MoveRight();
		}
	}

	void OnSelect() {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			if (hud->bEnabled) hud->Select();
		}
	}
}

#include "menu_career.h"
#include "menu_career_cupselect.h"
#include "menu_career_classselect.h"
#include "menu_career_finalresults.h"
#include "menu_career_unlock.h"
#include "menu_cardealer.h"
#include "menu_cardealer_sell.h"
#include "menu_crashresult.h"
#include "menu_trackselect.h"
#include "menu_arcadecareer.h"
#include "menu_multiplayer_lobby.h"
#include "menu_achievement_description.h"
#include "menu_chloetrax_config.h"
#include "menu_controller_layout.h"
#include "menu_loadingscreen.h"