class CMenu_Achievement_Description : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_achievement_description"; }

	tDrawPositions1080p gPos = {960,845,0.04};
	std::string sDescription;

	void SetAchievement(Achievements::CAchievement* achievement) {
		sDescription = achievement->sDescription;
	}

	virtual void Process() {
		if (!bEnabled) return;

		tNyaStringData data;
		data.x = gPos.nPosX;
		data.y = gPos.nPosY;
		data.size = gPos.fSize;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_LEFT, data, sDescription, &DrawStringFO2_Ingame12);
	}
} Menu_Achievement_Description;