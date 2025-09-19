class CHUD_TimeTrial : public CIngameHUDElement {
public:

	void Process() override {
		if (!IsRaceHUDUp()) return;
		if (!bIsTimeTrial) return;
		if (CareerMode::IsCareerTimeTrial()) return;

		if (CMenu_TrackSelect::nTimeTrial3LapMode) {
			auto pb1 = ChloeTimeTrial::GetCurrentRacePBTime(false, false);
			auto pb2 = ChloeTimeTrial::GetCurrentRacePBTime(false, true);
			DrawElement(2, "BEST TIME", pb1 == UINT_MAX ? "N /A" : FormatGameTime(pb1));
			if (pb2 != UINT_MAX) {
				DrawElement(3, "OPPONENT", FormatGameTime(pb2));
			}
		}
		else {
			auto pb1 = ChloeTimeTrial::GetCurrentRacePBTime(false, false);
			auto pb2 = ChloeTimeTrial::GetCurrentRacePBTime(false, true);
			auto pb3 = ChloeTimeTrial::GetCurrentRacePBTime(true, false);
			auto pb4 = ChloeTimeTrial::GetCurrentRacePBTime(true, true);
			DrawElement(2, "ROLLING PB", pb1 == UINT_MAX ? "N /A" : FormatGameTime(pb1));
			DrawElement(3, "STANDING PB", pb3 == UINT_MAX ? "N /A" : FormatGameTime(pb3));
			if (pb2 != UINT_MAX || pb4 != UINT_MAX) {
				DrawElement(4, "OPPONENT ROLLING PB", pb2 == UINT_MAX ? "N /A" : FormatGameTime(pb2));
				DrawElement(5, "OPPONENT STANDING PB", pb4 == UINT_MAX ? "N /A" : FormatGameTime(pb4));
			}
		}
	}
} HUD_TimeTrial;
