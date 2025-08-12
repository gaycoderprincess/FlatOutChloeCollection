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
};

namespace ChloeMenuHud {
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
}

#include "menu_career.h"
#include "menu_career_cupselect.h"
#include "menu_career_classselect.h"
#include "menu_career_finalresults.h"
#include "menu_cardealer.h"
#include "menu_loadingscreen.h"