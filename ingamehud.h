namespace NewGameHud {
	bool IsRaceHUDUp() {
		if (GetScoreManager()->nIsRaceOver) return false;
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
	float fHealthBarGlow[32];
	double fHealthBarGlowTimer[32];
	float fHealthBarGlowSpeed = 0.2;
	bool bHealthBarMatches[32];

	void ProcessPlayerHealthBarAlpha() {
		static CNyaTimer gTimer;
		gTimer.Process();

		int closestPlayerId = -1;
		float closestPlayerDist = 9999;
		auto playerPos = GetPlayer(0)->pCar->GetMatrix()->p;
		for (int i = 1; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayer(i);
			float damage = ply->pCar->fDamage;
			if (bHealthBarMatches[i] = fHealthBarGlow[i] == damage) {
				fHealthBarGlowTimer[i] = 0;
			}
			else {
				if (fHealthBarGlowTimer[i] < 0.666) {
					fHealthBarGlowTimer[i] += gTimer.fDeltaTime;
				}
				else {
					if (fHealthBarGlow[i] < damage) {
						fHealthBarGlow[i] += fHealthBarGlowSpeed * gTimer.fDeltaTime;
						if (fHealthBarGlow[i] > damage) fHealthBarGlow[i] = damage;
					}
					if (fHealthBarGlow[i] > damage) {
						fHealthBarGlow[i] -= fHealthBarGlowSpeed * gTimer.fDeltaTime;
						if (fHealthBarGlow[i] < damage) fHealthBarGlow[i] = damage;
					}
				}
			}
			if (GetPlayerScore<PlayerScoreRace>(i+1)->bIsDNF) continue;
			auto dist = (ply->pCar->GetMatrix()->p - playerPos).length();
			if (dist < closestPlayerDist) {
				closestPlayerDist = dist;
				closestPlayerId = i;
			}
		}

		int targetAlpha = -1;
		if (closestPlayerDist < 10) targetAlpha = closestPlayerId;

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			// keep glowing health bars for a while longer
			if (targetAlpha == -1 && fHealthBarAlpha[i] == 1 && !bHealthBarMatches[i]) {

			}
			else if (targetAlpha == i) {
				fHealthBarAlpha[i] += gTimer.fDeltaTime;
				if (closestPlayerDist < 4) fHealthBarAlpha[i] = 1;
			}
			else {
				fHealthBarAlpha[i] -= gTimer.fDeltaTime;
				if (closestPlayerDist < 4) fHealthBarAlpha[i] = 0;
			}

			if (fHealthBarAlpha[i] < 0) fHealthBarAlpha[i] = 0;
			if (fHealthBarAlpha[i] > 1) fHealthBarAlpha[i] = 1;
		}
	}

	float fPlayerHealthTextX = 0.027;
	float fPlayerHealthTextY = 0.91;
	float fPlayerHealthTextSize = 0.029;
	float fPlayerHealthBarX = 0.0205;
	float fPlayerHealthBarY = 0.941;
	float fPlayerHealthBarSize = 0.0389;

	void DrawPlayerHealthBar() {
		if (!IsRaceHUDUp()) return;

		static auto ai_damage_meter = NewMenuHud::LoadTextureFromBFS("data/menu/ai_damage_meter.png");
		static auto ai_damage_meter_glow = NewMenuHud::LoadTextureFromBFS("data/menu/ai_damage_meter_glow.png");
		static auto ai_damage_meter_bg = NewMenuHud::LoadTextureFromBFS("data/menu/ai_damage_meter_bg.png");

		ProcessPlayerHealthBarAlpha();

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (fHealthBarAlpha[i] <= 0.0) continue;

			int alpha = 255*fHealthBarAlpha[i];
			if (alpha < 0) alpha = 0;
			if (alpha > 255) alpha = 255;

			NewMenuHud::Draw1080pSprite(NewMenuHud::JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,(uint8_t)alpha}, ai_damage_meter_bg);

			auto ply = GetPlayer(i);

			tNyaStringData data;
			data.x = fPlayerHealthTextX * GetAspectRatioInv();
			data.y = fPlayerHealthTextY;
			data.size = fPlayerHealthTextSize;
			data.SetColor(GetPaletteColor(18));
			data.a = alpha;
			//DrawStringFO2_Ingame12(data, GetStringNarrow(ply->sPlayerName.Get()));
			DrawStringFO2_Small(data, GetStringNarrow(ply->sPlayerName.Get()));

			float aspect = 402.0 / 42.0;

			float uv = 1 - ply->pCar->fDamage;
			float uvGlow = 1 - fHealthBarGlow[i];
			float x1 = fPlayerHealthBarX;
			float y1 = fPlayerHealthBarY;
			float x2 = fPlayerHealthBarX + std::lerp(0, fPlayerHealthBarSize * aspect, 1 - ply->pCar->fDamage);
			float x2Glow = fPlayerHealthBarX + std::lerp(0, fPlayerHealthBarSize * aspect, 1 - fHealthBarGlow[i]);
			float y2 = fPlayerHealthBarY + fPlayerHealthBarSize;
			if (x2 != x2Glow) DrawRectangle(x1 * GetAspectRatioInv(), x2Glow * GetAspectRatioInv(), y1, y2, {255,255,255,(uint8_t)alpha}, 0, ai_damage_meter_glow, 0, {0,0}, {uvGlow,1});
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,(uint8_t)alpha}, 0, ai_damage_meter, 0, {0,0}, {uv,1});
		}
	}

	void OnTick() {
		if (pLoadingScreen) return;
		if (GetGameState() != GAME_STATE_RACE) {
			memset(fHealthBarAlpha,0,sizeof(fHealthBarAlpha));
			memset(fHealthBarGlow,0,sizeof(fHealthBarGlow));
			memset(fHealthBarGlowTimer,0,sizeof(fHealthBarGlowTimer));
			bPlayerListUp = nPlayerListDefaultState;
			return;
		}
		DrawPlayerList();
		DrawPlayerHealthBar();
	}
}