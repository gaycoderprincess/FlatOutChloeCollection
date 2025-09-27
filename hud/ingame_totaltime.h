class CHUD_TotalTime : public CIngameHUDElement {
public:
	static constexpr float fPosX = 0.07;
	static constexpr float fPosXFragDerby = 0.14;
	static constexpr float fPosY = 0.3;
	static constexpr float fSize = 0.038;
	static constexpr float fSpacing = 0.03;
	static constexpr float fSpacingX = 0.006;

	void Process() override {
		if (!IsRaceHUDUp()) return;
		if (IsInSplitScreen()) return;
		if (bIsCarnageRace || bIsFragDerby || bIsSmashyRace) return;
		if (pGameFlow->nEventType == eEventType::STUNT) return;

		auto time = pPlayerHost->nRaceTime;
		if (time < 0) time = 0;
		DrawElement(0, "TOTAL TIME", FormatGameTime(time));
	}
} HUD_TotalTime;