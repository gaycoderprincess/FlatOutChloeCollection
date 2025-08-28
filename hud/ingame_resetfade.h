class CHUD_ResetFade : public CIngameHUDElement {
public:

	virtual void Init() {
		nHUDLayer = eHUDLayer::FADE;
	}

	virtual void Process() {
		if (!IsRaceHUDUp()) return;

		// repeat for all 4 players
		for (int i = 0; i < 4; i++) {
			uint8_t alpha = fCarResetFadeTimer[i]*255;
			if (IsInQuarteredSplitScreen()) {
				switch (i) {
					case 0:
						DrawRectangle(0, 0.5, 0, 0.5, {0, 0, 0, alpha});
						break;
					case 1:
						DrawRectangle(0.5, 1, 0, 0.5, {0, 0, 0, alpha});
						break;
					case 2:
						DrawRectangle(0, 0.5, 0.5, 1, {0, 0, 0, alpha});
						break;
					case 3:
						DrawRectangle(0.5, 1, 0.5, 1, {0, 0, 0, alpha});
						break;
				}
			}
			else if (IsInHalvedSplitScreen()) {
				DrawRectangle(0, 1, 0 + (i*0.5), 0.5 + (i*0.5), {0, 0, 0, alpha});
			}
			else {
				DrawRectangle(0, 1, 0, 1, {0, 0, 0, alpha});
			}
		}
	}
} HUD_ResetFade;