class CHUD_ContactTimer : public CIngameHUDElement {
public:
	static constexpr float fPosX = 0.009;
	static constexpr float fPosY1 = 0.119;
	static constexpr float fPosY2 = 0.034;
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
		if (IsInSplitScreen()) return;
		if (pGameFlow->nEventType != eEventType::DERBY) return;

		auto timer = fDerbyMaxContactTimer - fDerbyContactTimer[0];
		DrawElement(1, "CONTACT TIMER", GetContactTimerString(timer * 1000));
	}
} HUD_ContactTimer;