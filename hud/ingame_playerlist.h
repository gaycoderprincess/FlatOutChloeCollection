class CHUD_PlayerList : public CIngameHUDElement {
public:
	static constexpr float fPosX = 0.07;
	static constexpr float fPosY = 0.3;
	static constexpr float fSize = 0.038;
	static constexpr float fSpacing = 0.03;
	static constexpr float fSpacingX = 0.006;

	// from easing-functions by nicolausYes
	static double easeInOutQuart(double t) {
		if (t < 0.5) {
			t *= t;
			return 8 * t * t;
		} else {
			t = (--t) * t;
			return 1 - 8 * t * t;
		}
	}

	virtual void Reset() {
		bEnabled = nPlayerListDefaultState;
	}

	virtual void Process() {
		if (!IsRaceHUDUp()) return;
		if (pGameFlow->nEventType == eEventType::STUNT) return;

		static CNyaRaceTimer gTimer;
		static double fPlayerListState = 0;
		gTimer.Process();

		// todo rebindable?
		if (!pGameFlow->nIsPaused && (IsKeyJustPressed('L') || IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_LEFT))) {
			bEnabled = !bEnabled;
		}

		if (bEnabled && fPlayerListState < 1) fPlayerListState += gTimer.fDeltaTime * 2;
		if (!bEnabled && fPlayerListState > 0) fPlayerListState -= gTimer.fDeltaTime * 2;
		if (fPlayerListState < 0.333) fPlayerListState = 0.333;
		if (fPlayerListState > 1) fPlayerListState = 1;

		tNyaStringData data;
		data.x = std::lerp(-0.5, fPosX * fPlayerListState * GetAspectRatioInv(), easeInOutQuart(fPlayerListState));
		data.y = fPosY;
		if (IsInSplitScreen()) data.y *= 0.5;
		data.size = fSize;
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetScoreManager()->aScores[i];
			if (!ply) continue;
			auto string1 = std::format("{}.", ply->nPosition);
			auto string2 = std::format("{}", GetStringNarrow(GetPlayer(ply->nPlayerId)->sPlayerName.Get()));
			if (!IsInSplitScreen() && ply->nPlayerId == 0) {
				data.SetColor(GetPaletteColor(18));
			}
			else {
				data.SetColor(GetPaletteColor(17));
			}
			if (ply->bIsDNF) {
				data.SetColor(64,64,64,255);
			}
			data.XRightAlign = true;
			DrawStringFO2_Ingame12(data, string1);
			data.x += fSpacingX * GetAspectRatioInv();
			data.XRightAlign = false;
			DrawStringFO2_Ingame12(data, string2);
			data.x -= fSpacingX * GetAspectRatioInv();
			data.y += fSpacing;
		}
	}
} HUD_PlayerList;