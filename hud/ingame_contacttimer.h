class CHUD_ContactTimer : public CIngameHUDElement {
public:
	static std::string GetContactTimerString(int value) {
		auto valueAsSeconds = value / 1000;
		int seconds = valueAsSeconds;
		int milliseconds = value % 1000;

		return std::format("{:02}\'\'{:02}", seconds, milliseconds/10);
	}

	void Process() override {
		if (!IsRaceHUDUp()) return;
		if (IsInSplitScreen()) return;
		if (pGameFlow->nEventType != eEventType::DERBY) return;
		if (bIsFragDerby) return;

		auto timer = DerbyMaxContactTimer - fDerbyContactTimer[0];
		DrawElement(1, "CONTACT TIMER", GetContactTimerString(timer * 1000));
	}
} HUD_ContactTimer;