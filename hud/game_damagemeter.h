class CHUD_DamageMeter : public CIngameHUDElement {
public:
	float fHealthBarAlpha[32];
	float fHealthBarGlow[32];
	double fHealthBarGlowTimer[32];
	float fHealthBarGlowSpeed = 0.2;
	bool bHealthBarMatches[32];

	static constexpr float fHealthTextX = 0.027;
	static constexpr float fHealthTextY = 0.91;
	static constexpr float fHealthTextSize = 0.029;
	static constexpr float fHealthBarX = 0.0205;
	static constexpr float fHealthBarY = 0.941;
	static constexpr float fHealthBarSize = 0.0389;

	void ProcessHealthBarAlpha() {
		auto localPlayer = GetPlayer(0);

		static CNyaRaceTimer gTimer;
		gTimer.Process();

		int closestPlayerId = -1;
		float closestPlayerDist = 9999;
		bool playerJustHit = false;
		auto playerPos = GetPlayer(0)->pCar->GetMatrix()->p;
		for (int i = 1; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayer(i);
			float damage = GetCarDamage(ply->pCar);
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
			if (localPlayer->pCar->aCarCollisions[i].lastHitTimestamp > pPlayerHost->nRaceTime - 100) {
				playerJustHit = true;
				closestPlayerDist = (ply->pCar->GetMatrix()->p - playerPos).length();
				closestPlayerId = i;
				break;
			}
			else {
				auto dist = (ply->pCar->GetMatrix()->p - playerPos).length();
				if (dist < closestPlayerDist) {
					closestPlayerDist = dist;
					closestPlayerId = i;
				}
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
				if (playerJustHit) fHealthBarAlpha[i] = 1;
			}
			else {
				fHealthBarAlpha[i] -= gTimer.fDeltaTime;
				if (playerJustHit) fHealthBarAlpha[i] = 0;
			}

			if (fHealthBarAlpha[i] < 0) fHealthBarAlpha[i] = 0;
			if (fHealthBarAlpha[i] > 1) fHealthBarAlpha[i] = 1;
		}
	}

	virtual void Reset() {
		memset(fHealthBarAlpha,0,sizeof(fHealthBarAlpha));
		memset(fHealthBarGlow,0,sizeof(fHealthBarGlow));
		memset(fHealthBarGlowTimer,0,sizeof(fHealthBarGlowTimer));
	}
	virtual void Process() {
		if (!IsRaceHUDUp()) return;

		static auto ai_damage_meter = LoadTextureFromBFS("data/menu/ai_damage_meter.png");
		static auto ai_damage_meter_glow = LoadTextureFromBFS("data/menu/ai_damage_meter_glow.png");
		static auto ai_damage_meter_bg = LoadTextureFromBFS("data/menu/ai_damage_meter_bg.png");

		ProcessHealthBarAlpha();

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (fHealthBarAlpha[i] <= 0.0) continue;

			int alpha = 255*fHealthBarAlpha[i];
			if (alpha < 0) alpha = 0;
			if (alpha > 255) alpha = 255;

			Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,(uint8_t)alpha}, ai_damage_meter_bg);

			auto ply = GetPlayer(i);

			tNyaStringData data;
			data.x = fHealthTextX * GetAspectRatioInv();
			data.y = fHealthTextY;
			data.size = fHealthTextSize;
			data.SetColor(GetPaletteColor(18));
			data.a = alpha;
			//DrawStringFO2_Ingame12(data, GetStringNarrow(ply->sPlayerName.Get()));
			DrawStringFO2_Small(data, GetStringNarrow(ply->sPlayerName.Get()));

			float aspect = 402.0 / 42.0;

			float uv = 1 - GetCarDamage(ply->pCar);
			float uvGlow = 1 - fHealthBarGlow[i];
			float x1 = fHealthBarX;
			float y1 = fHealthBarY;
			float x2 = fHealthBarX + std::lerp(0, fHealthBarSize * aspect, 1 - GetCarDamage(ply->pCar));
			float x2Glow = fHealthBarX + std::lerp(0, fHealthBarSize * aspect, 1 - fHealthBarGlow[i]);
			float y2 = fHealthBarY + fHealthBarSize;
			if (x2 != x2Glow) DrawRectangle(x1 * GetAspectRatioInv(), x2Glow * GetAspectRatioInv(), y1, y2, {255,255,255,(uint8_t)alpha}, 0, ai_damage_meter_glow, 0, {0,0}, {uvGlow,1});
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,(uint8_t)alpha}, 0, ai_damage_meter, 0, {0,0}, {uv,1});
		}
	}
} HUD_DamageMeter;