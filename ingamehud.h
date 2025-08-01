namespace NewGameHud {
	bool IsRaceHUDUp() {
		if (auto ply = GetPlayerScore<PlayerScoreRace>(1)) {
			if (ply->bHasFinished) return false;
			if (ply->bIsDNF) return false;
		}
		return true;
	}

	bool bPlayerListUp = true;

	float fPlayerListX = 0.07;
	float fPlayerListY = 0.3;
	float fPlayerListSize = 0.038;
	float fPlayerListSpacing = 0.03;

	// from easing-functions by nicolausYes
	double easeInOutQuart(double t) {
		if (t < 0.5) {
			t *= t;
			return 8 * t * t;
		} else {
			t = (--t) * t;
			return 1 - 8 * t * t;
		}
	}

	void DrawPlayerList() {
		if (!IsRaceHUDUp()) return;

		static CNyaTimer gTimer;
		static double fPlayerListState = 0;
		gTimer.Process();

		// todo rebindable?
		if (IsKeyJustPressed('L')) {
			bPlayerListUp = !bPlayerListUp;
		}

		if (bPlayerListUp && fPlayerListState < 1) fPlayerListState += gTimer.fDeltaTime * 2;
		if (!bPlayerListUp && fPlayerListState > 0) fPlayerListState -= gTimer.fDeltaTime * 2;
		if (fPlayerListState < 0.333) fPlayerListState = 0.333;
		if (fPlayerListState > 1) fPlayerListState = 1;

		tNyaStringData data;
		data.x = std::lerp(-0.5, fPlayerListX * fPlayerListState * GetAspectRatioInv(), easeInOutQuart(fPlayerListState));
		data.y = fPlayerListY;
		data.size = fPlayerListSize;
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetScoreManager()->aScores[i];
			if (!ply) continue;
			auto string = GetStringNarrow(GetPlayer(ply->nPlayerId)->sPlayerName.Get());
			string = std::format("{}. {}", ply->nPosition, string);
			if (ply->nPlayerId == 0) {
				data.SetColor(GetPaletteColor(18));
			}
			else {
				data.SetColor(GetPaletteColor(17));
			}
			if (ply->bIsDNF) {
				data.SetColor(64,64,64,255);
			}
			DrawStringFO2_Ingame12(data, string);
			data.y += fPlayerListSpacing;
		}
	}

	void OnTick() {
		if (pLoadingScreen) return;
		if (GetGameState() != GAME_STATE_RACE) {
			bPlayerListUp = true;
			return;
		}
		DrawPlayerList();
	}
}