class CMenu_Achievement_Description : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_achievement_description"; }

	tDrawPositions1080p gPos = {960,845,0.04};
	std::string sDescription;

	void SetAchievement(Achievements::CAchievement* achievement) {
		if (!achievement) {
			sDescription = std::format("{} out of {} completed", Achievements::GetNumUnlockedAchievements(), Achievements::GetNumVisibleAchievements());
			return;
		}
		sDescription = achievement->sDescription;
	}

	void Process() override {
		if (!bEnabled) return;

		tNyaStringData data;
		data.x = gPos.nPosX;
		data.y = gPos.nPosY;
		data.size = gPos.fSize;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_CENTER, data, sDescription, &DrawStringFO2_Condensed12);
	}
} Menu_Achievement_Description;