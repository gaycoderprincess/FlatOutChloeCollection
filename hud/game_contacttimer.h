class CHUD_ContactTimer : public CIngameHUDElement {
	static constexpr float fPosX = 0.009;
	static constexpr float fPosY1 = 0.119;
	static constexpr float fPosY2 = 0.153;
	static constexpr float fSize1 = 0.041;
	static constexpr float fSize2 = 0.041;

	static std::string GetContactTimerString(int value) {
		auto valueAsSeconds = value / 1000;
		int seconds = valueAsSeconds;
		int milliseconds = value % 1000;

		return std::format("{:02}\'\'{:02}", seconds, milliseconds/10);
	}

	virtual void Process() {
		if (!IsRaceHUDUp()) return;
		if (pGameFlow->nEventType != eEventType::DERBY) return;

		auto timer = fDerbyMaxContactTimer - fDerbyContactTimer[0];

		tNyaStringData data;
		data.x = fPosX * GetAspectRatioInv();
		data.y = fPosY1;
		data.size = fSize1;
		data.SetColor(GetPaletteColor(18));
		DrawStringFO2_Ingame12(data, "CONTACT TIMER");
		data.x = fPosX * GetAspectRatioInv();
		data.y = fPosY2;
		data.size = fSize2;
		data.SetColor({255,255,255,255});
		DrawStringFO2_Ingame24(data, GetContactTimerString(timer * 1000));
	}
} HUD_ContactTimer;