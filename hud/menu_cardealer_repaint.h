class CMenu_CarDealer_Repaint : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_cardealer_repaint"; }

	void Process() override {
		if (!bEnabled) return;

		CMenu_CarDealer::ProcessSkinSelector();
	}
} Menu_CarDealer_Repaint;