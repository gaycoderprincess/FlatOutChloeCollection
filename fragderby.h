namespace FragDerby {
	// db values
	const int RespawnTime = 5000;
	const int FragScore_Base = 1500;
	const int FragScore_ResetCost = 1500;
	const float FragScore_StreakMultiplier = 2;
	const int FragScore_SurvivorBonusPerSecond = 50;
	float WreckedMass = 100;

	//Get the most frags with the current life to be the Frag Streaker and earn double scores.
	//Survive the longest with the current life to earn extra score as the Survivor.

	double fPlayerGivenTime;

	int nPlayerWrecksThisLife[nMaxPlayers] = {};
	double fPlayerTimeAlive[nMaxPlayers] = {};
	double fPlayerTimeDead[nMaxPlayers] = {};
	double fPlayerSurvivorTick[nMaxPlayers] = {};
	double fGameTimeLeft = 0;
	int nStreakerId = -1;

	int GetHighestKillstreak() {
		int wrecks = 0;
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (nPlayerWrecksThisLife[i] > wrecks) {
				wrecks = nPlayerWrecksThisLife[i];
			}
		}
		return wrecks;
	}

	int GetSurvivorID() {
		float highestSurvival = 0;
		int id = -1;
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (fPlayerTimeAlive[i] > highestSurvival) {
				highestSurvival = fPlayerTimeAlive[i];
				id = i;
			}
		}
		if (highestSurvival <= 0) return -1;
		// only return survivor if they're the only one remaining
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (i == id) continue;
			if (fPlayerTimeAlive[i] == highestSurvival) {
				return -1;
			}
		}
		return id;
	}

	float GetPlayerScoreMultiplier(int player) {
		if (player == nStreakerId) return 2;
		return 1;
	}

	int GetCrashBonusPrice(int type) {
		switch (type) {
			case CRASHBONUS_SLAM:
				return 100;
			case CRASHBONUS_SUPERFLIP:
				return 250;
			case CRASHBONUS_POWERHIT:
				return 250;
			case CRASHBONUS_BLASTOUT:
				return 500;
			case CRASHBONUS_RAGDOLLED:
				return 0;
			case CRASHBONUS_WRECKED:
				return 1500;
			default:
				return 0;
		}
	}

	void OnCrashBonus(Player* pPlayer, int type) {
		if (!bIsFragDerby) return;
		if (fGameTimeLeft <= 0) return;
		int id = pPlayer->nPlayerId-1;
		if (type == CRASHBONUS_WRECKED && !pPlayer->nIsRagdolled) {
			if (nStreakerId == id) {
				if (pPlayer->nPlayerType != PLAYERTYPE_LOCAL) AddTopBarNotif(std::format("{}\nscores yet another frag! ({} frags)", GetStringNarrow(GetPlayer(id)->sPlayerName.Get()), nPlayerWrecksThisLife[id]+1));
			}
			else if (GetHighestKillstreak() < nPlayerWrecksThisLife[id] + 1) {
				nStreakerId = id;
				if (pPlayer->nPlayerType != PLAYERTYPE_LOCAL) AddTopBarNotif(std::format("{}\nis on a frag streak! ({} frags)", GetStringNarrow(GetPlayer(id)->sPlayerName.Get()), nPlayerWrecksThisLife[id]+1));
			}
			nPlayerWrecksThisLife[id]++;
		}
		nPlayerScore[id] += GetCrashBonusPrice(type) * GetPlayerScoreMultiplier(id);
	}

	void SetIsFragDerby(bool apply) {
		bIsFragDerby = apply;
		fPlayerGivenTime = 60 * 5;

		// hud changes
		NyaHookLib::Patch<uint64_t>(0x454AFC, apply ? 0xE0A190000001EEE9 : 0xE0A1000001ED850F); // remove total time
		NyaHookLib::Patch<uint8_t>(0x4551E9, apply ? 0xEB : 0x74); // remove lap time title
		NyaHookLib::Patch<uint8_t>(0x45578E, apply ? 0xEB : 0x74); // remove lap time counter
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4553FA, apply ? 0x4F0843 : 0x4F0810); // remove lap count title
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x455543, apply ? 0x4F0843 : 0x4F0810); // remove lap count number
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x455597, apply ? 0x4F0843 : 0x4F0810); // remove lap count max

		NyaHookLib::Patch(0x416748 + 2, apply ? -1 : -100); // minimum crash bonus interval, default -500
		NyaHookLib::Patch<uint8_t>(0x44906F + 2, apply ? 3 : 1); // enable nitro in derby
		NyaHookLib::Patch<uint8_t>(0x454304, apply ? 0xEB : 0x75); // disable fail on ragdoll
		NyaHookLib::Patch<uint8_t>(0x4087D5, apply ? 0xEB : 0x74); // disable ai bork on ragdoll
	}

	void Reset() {
		fGameTimeLeft = fPlayerGivenTime;
		memset(nPlayerScore,0,sizeof(nPlayerScore));
		memset(nPlayerWrecksThisLife,0,sizeof(nPlayerWrecksThisLife));
		memset(fPlayerTimeAlive,0,sizeof(fPlayerTimeAlive));
		memset(fPlayerTimeDead,0,sizeof(fPlayerTimeDead));
		memset(fPlayerSurvivorTick,0,sizeof(fPlayerSurvivorTick));
		nStreakerId = -1;
	}

	void ProcessPlayer(double delta, int player) {
		if (fGameTimeLeft <= 0) return;

		if (player == GetSurvivorID()) {
			fPlayerSurvivorTick[player] += delta;
			if (fPlayerSurvivorTick[player] >= 1) {
				nPlayerScore[player] += FragScore_SurvivorBonusPerSecond;
				fPlayerSurvivorTick[player] -= 1;
			}
		}
		else {
			fPlayerSurvivorTick[player] = 0;
		}

		auto ply = GetPlayer(player);
		ply->pCar->Performance.Engine.fHealth = ply->nIsRagdolled ? 0.0 : 1.0; // engine smoke based entirely on wrecked or not
		if (ply->nIsRagdolled) {
			fPlayerTimeAlive[player] = 0;

			if (nStreakerId == player) {
				nStreakerId = -1;

				if (ply->nPlayerType != PLAYERTYPE_LOCAL) {
					AddTopBarNotif(std::format("{}\nfrag streak ended! ({} frags)", GetStringNarrow(GetPlayer(player)->sPlayerName.Get()), nPlayerWrecksThisLife[player]));
				}
			}

			nPlayerWrecksThisLife[player] = 0;

			fPlayerTimeDead[player] += delta;
			if (fPlayerTimeDead[player] > RespawnTime*0.001) {
				ply->ResetCar(ply, 0);
				ply->pCar->FixPart(eCarFixPart::BODY);
				ply->pCar->FixPart(eCarFixPart::ENGINE);
				ply->pCar->FixPart(eCarFixPart::UNK_2);
				ply->pCar->FixPart(eCarFixPart::UNK_3);
				ply->pCar->FixPart(eCarFixPart::WHEELS);
				ply->pCar->FixPart(eCarFixPart::SUSPENSION);
				ply->pCar->FixPart(eCarFixPart::UNK_6);
				ply->pCar->FixPart(eCarFixPart::WINDOWS);
				ply->pCar->FixPart(eCarFixPart::LIGHTS);

				auto start = pEnvironment->aStartpoints[ply->nStartPosition-1];
				Car::Reset(ply->pCar, start.fPosition, start.fMatrix);
			}
		}
		else {
			fPlayerTimeDead[player] = 0;
			fPlayerTimeAlive[player] += delta;
		}
	}

	void OnTick() {
		static int lastSurvivor = -1;

		static CNyaRaceTimer gTimer;
		gTimer.Process();

		if (pLoadingScreen) return;
		if (!bIsFragDerby) {
			Reset();
			return;
		}
		if (GetGameState() != GAME_STATE_RACE) return;

		ArcadeMode::nCurrentEventScore = nPlayerScore[0];

		if (pPlayerHost->nRaceTime < 0) {
			Reset();
			lastSurvivor = -1;
			return;
		}

		int survivor = GetSurvivorID();
		if (survivor > 0 && survivor != lastSurvivor) {
			AddTopBarNotif(std::format("{}\nhas survived the longest!", GetStringNarrow(GetPlayer(survivor)->sPlayerName.Get())));
		}
		lastSurvivor = GetSurvivorID();

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			ProcessPlayer(gTimer.fDeltaTime, i);
		}

		GetScoreManager()->nNumLaps = 10;

		fGameTimeLeft -= gTimer.fDeltaTime;

		ArcadeMode::ProcessTimerTick(fGameTimeLeft*1000);

		auto ply = GetPlayerScore<PlayerScoreRace>(1);
		if (fGameTimeLeft <= 0 && !ply->bHasFinished && !ply->bIsDNF) {
			ply->bHasFinished = true;
			ply->nFinishTime = pPlayerHost->nRaceTime;
		}
	}

	class CHUD_FragDerby : public CIngameHUDElement {
	public:

		static inline double fPopupTime = 0;
		static inline std::string sPopupTitle;
		static inline std::string sPopupString;

		static void TriggerPopup(const std::string& str1, const std::string& str2) {
			fPopupTime = 4;
			sPopupTitle = str1;
			sPopupString = str2;
		}

		static inline float fPopupSize = 0.055;
		static inline float fPopupSpacing = 0.07;
		static void DrawPopup(float y, const std::string& title, const std::string& value, const NyaDrawing::CNyaRGBA32 rgb = {255,255,255,255}) {
			tNyaStringData data;
			data.x = 0.5;
			data.y = gElementBase.fPosY + y * fElementTotalSpacing;
			data.size = fPopupSize;
			data.XCenterAlign = true;
			data.SetColor(GetPaletteColor(COLOR_INGAME_YELLOW));
			data.a = rgb.a;
			DrawStringFO2_Ingame10(data, title);
			data.y += fPopupSpacing;
			data.SetColor(rgb);
			DrawStringFO2_Ingame10(data, value);
		}

		static inline float fPlayerHealthIconOffset = 1.3;
		static inline float fPlayerHealthIconSize = 0.17;
		static void DrawPlayerHealthIcon(Player* ply) {
			if (ply->nIsRagdolled) return;
			if (ply->nPlayerType == PLAYERTYPE_LOCAL) return;

			static auto tex = LoadTextureFromBFS("data/global/overlay/derby_health_indicator.png");

			auto mat = *ply->pCar->GetMatrix();
			mat.p += mat.y * fPlayerHealthIconOffset;
			auto drawPos = Get3DTo2D(mat);
			if (drawPos.z <= 0) return;

			NyaDrawing::CNyaRGBA32 color = {255,255,255,255};
			auto damage = ply->pCar->GetDerbyDamage();
			if (damage <= 0.0) damage = 0.0;
			if (damage >= 1.0) damage = 1.0;
			if (damage < 0.5) {
				color.r = std::lerp(64, 255, damage * 2);
				color.g = std::lerp(255, 200, damage * 2);
				color.b = std::lerp(64, 60, damage * 2);
			}
			else {
				color.r = std::lerp(255, 255, damage - 0.5);
				color.g = std::lerp(200, 0, damage - 0.5);
				color.b = std::lerp(60, 0, damage - 0.5);
			}

			float fSize = fPlayerHealthIconSize / drawPos.z;
			DrawRectangle(drawPos.x - fSize * GetAspectRatioInv(), drawPos.x + fSize * GetAspectRatioInv(), drawPos.y - fSize, drawPos.y + fSize, color, 0, tex);
		}

		static inline float fPlayerIconOffset = 1.7;
		static inline float fPlayerIconSize = 0.25;
		static void DrawPlayerIcon(const std::string& icon, Player* ply) {
			static auto tex = LoadTextureFromBFS("data/global/overlay/frag_derby_symbols.png");
			static auto texData = LoadHUDData("data/global/overlay/frag_derby_symbols.bed", "frag_derby_symbols");

			auto mat = *ply->pCar->GetMatrix();
			mat.p += mat.y * fPlayerHealthIconOffset;
			mat.p.z += fPlayerIconOffset - fPlayerHealthIconOffset;
			auto drawPos = Get3DTo2D(mat);
			if (drawPos.z <= 0) return;

			float fSize = fPlayerIconSize / drawPos.z;
			auto symbol = GetHUDData(texData, icon);
			DrawRectangle(drawPos.x - fSize * GetAspectRatioInv(), drawPos.x + fSize * GetAspectRatioInv(), drawPos.y - fSize, drawPos.y + fSize, {255,255,255,255}, 0, tex, 0, symbol->min, symbol->max);
		}

		virtual void Process() {
			static bool isStreaker = false;
			static bool isSurvivor = false;

			if (!IsRaceHUDUp()) return;
			if (!bIsFragDerby) return;

			if (nStreakerId >= 0 && nStreakerId == GetSurvivorID()) {
				DrawPlayerIcon("frag_derby_symbol_combinated", GetPlayer(nStreakerId));
			}
			else {
				if (nStreakerId >= 0) {
					DrawPlayerIcon("frag_derby_symbol_fragstreak", GetPlayer(nStreakerId));
				}
				if (GetSurvivorID() >= 0) {
					DrawPlayerIcon("frag_derby_symbol_survivor", GetPlayer(GetSurvivorID()));
				}
			}

			for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
				DrawPlayerHealthIcon(GetPlayer(i));
			}

			int timeLeft = fGameTimeLeft*1000;
			if (timeLeft < 0) timeLeft = 0;

			DrawElement(0, "TIME LEFT", FormatGameTime(timeLeft), timeLeft <= 4500 ? NyaDrawing::CNyaRGBA32(200,0,0,255) : NyaDrawing::CNyaRGBA32(255,255,255,255));
			DrawElement(1, "SCORE", FormatScore(nPlayerScore[0]));

			if (!isStreaker && nStreakerId == 0) {
				TriggerPopup("STREAKER", "You score 2X points\nfor each frag until wrecked!");
			}
			if (!isSurvivor && GetSurvivorID() == 0) {
				TriggerPopup("SURVIVOR", "You score 50 bonus points\nper second until wrecked!");
			}

			isStreaker = nStreakerId == 0;
			isSurvivor = GetSurvivorID() == 0;

			static CNyaRaceTimer gTimer;
			gTimer.Process();
			if (GetPlayer(0)->nIsRagdolled) {
				DrawPopup(6.5, "RESPAWN IN", std::to_string((int)(RespawnTime - (int)fPlayerTimeDead[0]*1000)/1000), {255,255,255,255});
			}
			else if (fPopupTime > 0) {
				NyaDrawing::CNyaRGBA32 rgb = {255,255,255,255};
				if (fPopupTime < 0.5) rgb.a = std::lerp(0, 255, fPopupTime * 2);
				if (fPopupTime > 3.75) rgb.a = std::lerp(0, 255, (4 - fPopupTime) * 4);
				DrawPopup(6.5, sPopupTitle, sPopupString, rgb);
				fPopupTime -= gTimer.fDeltaTime;
			}
		}
	} HUD_FragDerby;

	class CHUD_FragDerbyFade : public CIngameHUDElement {
	public:

		virtual void Init() {
			nHUDLayer = eHUDLayer::FADE;
		}

		virtual void Process() {
			if (!IsRaceHUDUp()) return;
			if (!bIsFragDerby) return;
			if (fGameTimeLeft <= 0) return;

			auto ply = GetPlayer(0);
			double respawnTime = RespawnTime * 0.001;
			if (ply->nIsRagdolled && fPlayerTimeDead[0] >= respawnTime - 0.5) {
				float alpha = (0.5 - (respawnTime - fPlayerTimeDead[0])) * 2;
				DrawRectangle(0,1,0,1,{0,0,0,(uint8_t)(alpha*255)});
			}
			else if (!ply->nIsRagdolled && pPlayerHost->nRaceTime >= RespawnTime && fPlayerTimeAlive[0] <= 0.5) {
				float alpha = (0.5 - fPlayerTimeAlive[0]) * 2;
				DrawRectangle(0,1,0,1,{0,0,0,(uint8_t)(alpha*255)});
			}
		}
	} HUD_FragDerbyFade;

	void OnPlayerReset(Player* pPlayer) {
		if (pPlayer->nPlayerId == 1) {
			HUD_FragDerby.TriggerPopup("", "Reset: -1500 points");
		}
		nPlayerScore[pPlayer->nPlayerId-1] -= FragScore_ResetCost;
	}

	void Init() {
		ChloeEvents::CrashBonusEvent.AddHandler(OnCrashBonus);
		ChloeEvents::FinishFrameEvent.AddHandler(OnTick);
		ChloeEvents::PlayerResetEvent.AddHandler(OnPlayerReset);
	}
}