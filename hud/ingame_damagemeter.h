void AddCrashBonus(Player* pPlayer, int type);

class CHUD_DamageMeter : public CIngameHUDElement {
public:
	int nPlayerId = 0;

	float fHealthBarAlpha[32];
	float fHealthBarGlow[32];
	double fHealthBarGlowTimer[32];
	float fHealthBarGlowSpeed = 0.2;
	bool bHealthBarMatches[32];

	std::vector<std::string> aCrashBonuses;
	double fCrashBonusTimer = 0;

	static constexpr float fCrashBonusTextX = 0.118 * (16.0 / 9.0);
	static constexpr float fCrashBonusTextY = 0.865;
	static constexpr float fCrashBonusTextSize = 0.03;
	static constexpr float fCrashBonusTextSize2 = 0.06;

	static constexpr float fCrashBonusFadeinStart = 2;
	static constexpr float fCrashBonusFadeinEnd = 1.75;
	static constexpr float fCrashBonusFadeinSpeed = 4;
	static constexpr float fCrashBonusFadeoutStart = 0.5;
	static constexpr float fCrashBonusFadeoutSpeed = 2;

	float GetScreenXOffset() {
		if (IsInQuarteredSplitScreen()) {
			if (nPlayerId == 1 || nPlayerId == 3) return 0.5;
		}
		return 0;
	}

	float GetScreenYOffset() {
		if (IsInQuarteredSplitScreen()) {
			if (nPlayerId == 0 || nPlayerId == 1) return -0.5;
		}
		if (IsInHalvedSplitScreen() && nPlayerId == 0) {
			return -0.5;
		}
		return 0;
	}

	void DrawCrashBonusNotif() {
		float fXOffset = GetScreenXOffset();
		float fYOffset = GetScreenYOffset();

		tNyaStringData data;
		data.x = fCrashBonusTextX * GetAspectRatioInv() + fXOffset;
		data.y = fCrashBonusTextY + fYOffset;
		data.size = fCrashBonusTextSize;
		data.XCenterAlign = true;
		//data.SetColor(GetPaletteColor(18));
		int a = 255;
		if (fCrashBonusTimer >= fCrashBonusFadeinEnd) {
			a = (fCrashBonusFadeinStart - fCrashBonusTimer) * fCrashBonusFadeinSpeed * 255;
			data.size = std::lerp(fCrashBonusTextSize2, fCrashBonusTextSize, a / 255.0);
		}
		if (fCrashBonusTimer <= fCrashBonusFadeoutStart) {
			a = fCrashBonusTimer * fCrashBonusFadeoutSpeed * 255;
		}
		data.a = a;
		DrawStringFO2_Italic24(data, aCrashBonuses[0]);
	}

	static constexpr float fHealthTextX = 0.027;
	static constexpr float fHealthTextY = 0.91;
	static constexpr float fHealthTextSize = 0.029;
	static constexpr float fHealthBarX = 0.0205;
	static constexpr float fHealthBarY = 0.941;
	static constexpr float fHealthBarSize = 0.0389;

	CNyaRaceTimer gTimer;
	void ProcessHealthBarAlpha() {
		auto localPlayer = GetPlayer(nPlayerId);

		int closestPlayerId = -1;
		float closestPlayerDist = 9999;
		bool playerJustHit = false;
		auto playerPos = localPlayer->pCar->GetMatrix()->p;
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (i == nPlayerId) continue;

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
			if (IsPlayerWrecked(ply)) continue;
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
		if (closestPlayerDist < 15) targetAlpha = closestPlayerId;

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

	void Init() override {
		PreloadTexture("data/global/overlay/ai_damage_meter.png");
		PreloadTexture("data/global/overlay/ai_damage_meter_glow.png");
		PreloadTexture("data/global/overlay/ai_damage_meter_bg.png");

		// once guard for damagemeter player2
		static bool bInited = false;
		if (bInited) return;
		bInited = true;

		ChloeEvents::CrashBonusEvent.AddHandler(AddCrashBonus);
	}
	void Reset() override {
		memset(fHealthBarAlpha,0,sizeof(fHealthBarAlpha));
		memset(fHealthBarGlow,0,sizeof(fHealthBarGlow));
		memset(fHealthBarGlowTimer,0,sizeof(fHealthBarGlowTimer));
	}
	void Process() override {
		if (!IsRaceHUDUp()) return;
		if (pGameFlow->nEventType == eEventType::STUNT) return;
		auto ply = GetPlayer(nPlayerId);
		if (!ply || ply->nPlayerType != PLAYERTYPE_LOCAL) return;
		if (!nSplitScreenDamageMeter && IsInSplitScreen()) return;

		float fXOffset = GetScreenXOffset();
		float fYOffset = GetScreenYOffset();
		int nXOffset = fXOffset * 1920;
		int nYOffset = fYOffset * 1080;

		static auto ai_damage_meter = LoadTextureFromBFS("data/global/overlay/ai_damage_meter.png");
		static auto ai_damage_meter_glow = LoadTextureFromBFS("data/global/overlay/ai_damage_meter_glow.png");
		static auto ai_damage_meter_bg = LoadTextureFromBFS("data/global/overlay/ai_damage_meter_bg.png");

		ProcessHealthBarAlpha();

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (fHealthBarAlpha[i] <= 0.0) continue;

			int alpha = 255*fHealthBarAlpha[i];
			if (alpha < 0) alpha = 0;
			if (alpha > 255) alpha = 255;

			Draw1080pSprite(JUSTIFY_LEFT, 0+nXOffset, 1920+nXOffset, 0+nYOffset, 1080+nYOffset, {255,255,255,(uint8_t)alpha}, ai_damage_meter_bg);

			auto ply = GetPlayer(i);

			tNyaStringData data;
			data.x = fHealthTextX * GetAspectRatioInv() + fXOffset;
			data.y = fHealthTextY + fYOffset;
			data.size = fHealthTextSize;
			data.SetColor(GetPaletteColor(COLOR_INGAME_YELLOW));
			data.a = alpha;
			//DrawStringFO2_Condensed12(data, GetStringNarrow(ply->sPlayerName.Get()));
			DrawStringFO2_Italic24(data, GetStringNarrow(ply->sPlayerName.Get()));

			float aspect = 402.0 / 42.0;

			float uv = 1 - GetCarDamage(ply->pCar);
			float uvGlow = 1 - fHealthBarGlow[i];
			float x1 = fHealthBarX;
			float y1 = fHealthBarY;
			float x2 = fHealthBarX + std::lerp(0, fHealthBarSize * aspect, 1 - GetCarDamage(ply->pCar));
			float x2Glow = fHealthBarX + std::lerp(0, fHealthBarSize * aspect, 1 - fHealthBarGlow[i]);
			float y2 = fHealthBarY + fHealthBarSize;
			if (x2 != x2Glow) DrawRectangle(x1 * GetAspectRatioInv() + fXOffset, x2Glow * GetAspectRatioInv() + fXOffset, y1+fYOffset, y2+fYOffset, {255,255,255,(uint8_t)alpha}, 0, ai_damage_meter_glow, 0, {0,0}, {uvGlow,1});
			DrawRectangle(x1 * GetAspectRatioInv() + fXOffset, x2 * GetAspectRatioInv() + fXOffset, y1+fYOffset, y2+fYOffset, {255,255,255,(uint8_t)alpha}, 0, ai_damage_meter, 0, {0,0}, {uv,1});
		}

		gTimer.Process();
		if (fCrashBonusTimer > 0) {
			fCrashBonusTimer -= gTimer.fDeltaTime;
			// increase speed if there's more than one
			if (aCrashBonuses.size() > 1) {
				fCrashBonusTimer -= gTimer.fDeltaTime * 2;
				if (fCrashBonusTimer < fCrashBonusFadeinEnd && fCrashBonusTimer > fCrashBonusFadeoutStart) {
					fCrashBonusTimer = fCrashBonusFadeoutStart;
				}
			}
			// play next in queue
			if (fCrashBonusTimer <= 0) {
				if (!aCrashBonuses.empty()) aCrashBonuses.erase(aCrashBonuses.begin());
				if (!aCrashBonuses.empty()) fCrashBonusTimer = fCrashBonusFadeinStart;
			}
		}
		else if (!aCrashBonuses.empty()) {
			fCrashBonusTimer = fCrashBonusFadeinStart;
		}

		if (pPlayerHost->nRaceTime <= 0) {
			fCrashBonusTimer = 0;
			aCrashBonuses.clear();
		}

		if (!aCrashBonuses.empty()) {
			DrawCrashBonusNotif();
		}
	}
} HUD_DamageMeter[nMaxSplitscreenPlayers] = {};

void AddCrashBonus(Player* pPlayer, int type) {
	auto str = GetCrashBonusName(type);
	if (!str) return;

	int playerId = pPlayer->nPlayerId-1;
	aCrashBonusesReceived[playerId][type]++;
	if (GetPlayer(playerId)->nPlayerType == PLAYERTYPE_LOCAL) {
		if (type != CRASHBONUS_SUPERFLIP && type != CRASHBONUS_WRECKED) {
			GetAchievement("BLAST_ALL")->fInternalProgress += 1;
			if (bIsInMultiplayer) GetAchievement("BLAST_MP")->fInternalProgress += 1;
		}
		if (type == CRASHBONUS_WRECKED && bIsInMultiplayer) {
			Achievements::AwardAchievement(GetAchievement("WRECK_MP"));
		}

		if (bIsInMultiplayer) {
			ChloeNet::OnCrashBonus(type);
		}

		if (nShowBonus) {
			HUD_DamageMeter[playerId].aCrashBonuses.push_back(str);
		}
	}
}