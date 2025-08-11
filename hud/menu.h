class CMenuHUDElement : public CHUDElement {
public:
	static inline std::vector<CMenuHUDElement*> aMenuHUD;

	CMenuHUDElement() {
		aMenuHUD.push_back(this);
	}
};

namespace NewMenuHud {
	void OnTick() {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			if (!hud->bEnabled) hud->Reset();
			hud->Process();
		}
	}

	void Init() {
		for (auto& hud : CMenuHUDElement::aMenuHUD) {
			hud->Init();
		}
	}
}

#include "menu_career.h"
#include "menu_career_cupselect.h"
#include "menu_career_classselect.h"
#include "menu_career_finalresults.h"
#include "menu_cardealer.h"
#include "menu_cardealer_skinselect.h"
#include "menu_loadingscreen.h"