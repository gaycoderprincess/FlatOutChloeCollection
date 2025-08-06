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
		if (pGameFlow->nEventType == eEventType::STUNT) return;

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

	float fHealthBarAlpha[32];

	void ProcessPlayerHealthBarAlpha() {
		static CNyaTimer gTimer;
		gTimer.Process();

		int closestPlayerId = -1;
		float closestPlayerDist = 9999;
		auto playerPos = GetPlayer(0)->pCar->GetMatrix()->p;
		for (int i = 1; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayer(i);
			if (GetPlayerScore<PlayerScoreRace>(i+1)->bIsDNF) continue;
			auto dist = (ply->pCar->GetMatrix()->p - playerPos).length();
			if (dist < closestPlayerDist) {
				closestPlayerDist = dist;
				closestPlayerId = i;
			}
		}

		int targetAlpha = -1;
		if (closestPlayerDist < 8) targetAlpha = closestPlayerId;

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (targetAlpha == i) {
				fHealthBarAlpha[i] += gTimer.fDeltaTime;
				if (closestPlayerDist < 3) fHealthBarAlpha[i] = 1;
			}
			else {
				fHealthBarAlpha[i] -= gTimer.fDeltaTime;
				if (closestPlayerDist < 3) fHealthBarAlpha[i] = 0;
			}

			if (fHealthBarAlpha[i] < 0) fHealthBarAlpha[i] = 0;
			if (fHealthBarAlpha[i] > 1) fHealthBarAlpha[i] = 1;
		}
	}

	float fPlayerHealthTextX = 0.4;
	float fPlayerHealthTextY = 0.9;
	float fPlayerHealthTextSize = 0.03;
	float fPlayerHealthBarX = 0.395;
	float fPlayerHealthBarY = 0.94;
	float fPlayerHealthBarSizeX = 0.45;
	float fPlayerHealthBarSizeY = 0.02;

	void DrawPlayerHealthBar() {
		return;

		if (!IsRaceHUDUp()) return;
		ProcessPlayerHealthBarAlpha();

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (fHealthBarAlpha[i] <= 0.0) continue;

			int alpha = 255*fHealthBarAlpha[i];
			if (alpha < 0) alpha = 0;
			if (alpha > 255) alpha = 255;

			auto ply = GetPlayer(i);

			tNyaStringData data;
			data.x = fPlayerHealthTextX * GetAspectRatioInv();
			data.y = fPlayerHealthTextY;
			data.size = fPlayerHealthTextSize;
			data.a = alpha;
			//DrawStringFO2_Ingame12(data, GetStringNarrow(ply->sPlayerName.Get()));
			DrawStringFO2_Small(data, GetStringNarrow(ply->sPlayerName.Get()));

			float x1 = fPlayerHealthBarX;
			float y1 = fPlayerHealthBarY;
			float x2 = fPlayerHealthBarX + std::lerp(0, fPlayerHealthBarSizeX, 1 - ply->pCar->fDamage);
			float x2Max = fPlayerHealthBarX + fPlayerHealthBarSizeX;
			float y2 = fPlayerHealthBarY + fPlayerHealthBarSizeY;
			x1 *= GetAspectRatioInv();
			x2 *= GetAspectRatioInv();
			x2Max *= GetAspectRatioInv();
			DrawRectangle(x1, x2Max, y1, y2, {0,0,0,(uint8_t)alpha});
			DrawRectangle(x1, x2, y1, y2, {255,255,255,(uint8_t)alpha});
		}
	}

	void OnTick() {
		if (pLoadingScreen) return;
		if (GetGameState() != GAME_STATE_RACE) {
			memset(fHealthBarAlpha,0,sizeof(fHealthBarAlpha));
			bPlayerListUp = nPlayerListDefaultState;
			return;
		}
		DrawPlayerList();
		DrawPlayerHealthBar();
	}
}