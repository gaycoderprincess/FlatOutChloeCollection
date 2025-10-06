namespace CarnageRace {
	GameRules::KeyValueArray RacePositionMultiplier("RacePositionMultiplier", 8);

	GameRules::KeyValue SceneryCrashScore("SceneryCrashScore");
	GameRules::KeyValue AirtimeScore("AirtimeScore");
	GameRules::KeyValue CheckpointScore("CheckpointScore");
	GameRules::KeyValue ComboMaxTime("ComboMaxTime");
	GameRules::KeyValue ComboMaxTimeFull("ComboMaxTimeFull");
	GameRules::KeyValue ComboMaxCount("ComboMaxCount");
	GameRules::KeyValue CashoutNotifMaxTime("CashoutNotifMaxTime");

	double fPlayerGivenTime;
	double fCheckpointTimeBonus;
	double fCheckpointTimeDecay;
	int nCheckpointInterval;

	struct tScoreEntry {
		std::string name;
		double points;
		int type;
	};
	std::vector<tScoreEntry> aScoreHUD;

	double fPlayerTimeLeft = 0;
	int nPlayerPosition = 0;

	double fScoreTimer = 0;

	double fCashoutNotifTimer = 0;
	int nCashoutNotifAmount = 0;

	double fCheckpointNotifTimer = 0;
	int nCheckpointNotifAmount = 0;

	void OnCheckpointPassed() {
		double timeToAdd = fCheckpointTimeBonus;
		for (int i = 0; i < GetPlayer(0)->nCurrentLap; i++) {
			timeToAdd *= fCheckpointTimeDecay;
		}
		fPlayerTimeLeft += timeToAdd;
		fCheckpointNotifTimer = 3;
		nCheckpointNotifAmount = timeToAdd;

		static auto sound = NyaAudio::LoadFile("data/sound/checkpoint.wav");
		if (sound) {
			NyaAudio::SetVolume(sound, nIngameSfxVolume / 100.0);
			NyaAudio::Play(sound);
		}
	}

	bool IsPlayerScoreLocallyControlled(Player* pPlayer) {
		if (!bIsInMultiplayer) return true;
		if (!ChloeNet::IsReplicatedPlayer(pPlayer)) return true;
		return false;
	}

	void AddScore(int type, std::string name, double points, int playerId) {
		auto plyPtr = GetPlayer(playerId-1);
		if (!IsPlayerScoreLocallyControlled(plyPtr)) return;
		if (IsPlayerWrecked(plyPtr)) return;

		auto ply = GetPlayerScore(playerId);
		if (ply->bHasFinished || ply->bIsDNF) return;

		if (playerId > 1) {
			nPlayerScore[playerId-1] += points * RacePositionMultiplier[GetPlayerScore(playerId)->nPosition-1];
			return;
		}

		if (ply->nPosition < nPlayerPosition || aScoreHUD.empty()) {
			nPlayerPosition = ply->nPosition;
		}
		fScoreTimer = ComboMaxTime;

		if (type == SCORE_SCENERY || type == SCORE_AIRTIME) {
			for (auto& score : aScoreHUD) {
				if (score.type == type) {
					score.points += points;
					return;
				}
			}
		}

		tScoreEntry entry;
		entry.name = name;
		entry.points = points;
		entry.type = type;
		aScoreHUD.push_back(entry);

		if (aScoreHUD.size() >= 10) {
			fScoreTimer = ComboMaxTimeFull;
			Achievements::AwardAchievement(GetAchievement("CARNAGE_FILL_BOARD"));
		}
		if (aScoreHUD.size() >= (int)ComboMaxCount) {
			fScoreTimer = 0.5;
		}
	}

	int GetCrashBonusPrice(int type) {
		switch (type) {
			case CRASHBONUS_SLAM:
				return 500;
			case CRASHBONUS_SUPERFLIP:
				return 750;
			case CRASHBONUS_POWERHIT:
				return 1000;
			case CRASHBONUS_BLASTOUT:
				return 1250;
			case CRASHBONUS_RAGDOLLED:
				return 1500;
			case CRASHBONUS_WRECKED:
				return 2000;
			default:
				return 0;
		}
	}

	void OnCrashBonus(Player* pPlayer, int type) {
		if (!bIsCarnageRace) return;
		AddScore(SCORE_CRASH, GetCrashBonusName(type), GetCrashBonusPrice(type), pPlayer->nPlayerId);
	}

	void SetIsCarnageRace(bool apply) {
		bIsCarnageRace = apply;
		fPlayerGivenTime = 120;
		fCheckpointTimeBonus = 7;
		fCheckpointTimeDecay = 0.75;
		nCheckpointInterval = 5;

		// hud changes
		NyaHookLib::Patch<uint8_t>(0x4551E9, apply ? 0xEB : 0x74); // remove lap time title
		NyaHookLib::Patch<uint8_t>(0x45578E, apply ? 0xEB : 0x74); // remove lap time counter
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4553FA, apply ? 0x4F0843 : 0x4F0810); // remove lap count title
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x455543, apply ? 0x4F0843 : 0x4F0810); // remove lap count number
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x455597, apply ? 0x4F0843 : 0x4F0810); // remove lap count max

		NyaHookLib::Patch(0x416748 + 2, apply ? -1 : -100); // minimum crash bonus interval, default -500
	}

	void CashOutCombo() {
		double pointsAwarded = 0;
		for (auto& score : aScoreHUD) {
			pointsAwarded += score.points * RacePositionMultiplier[nPlayerPosition-1];
			nPlayerScoresByType[score.type] += score.points * RacePositionMultiplier[nPlayerPosition-1];
		}
		nPlayerScore[0] += pointsAwarded;
		fCashoutNotifTimer = CashoutNotifMaxTime;
		nCashoutNotifAmount = pointsAwarded;
		aScoreHUD.clear();
	}

	void ProcessSceneryCrashes(int playerId) {
		static int last[nMaxPlayers] = {};
		if (pPlayerHost->nRaceTime < 0) {
			memset(last,0,sizeof(last));
			return;
		}

		int current = 0;
		auto ply = GetPlayer(playerId);
		for (int i = 0; i < 10; i++) {
			current += ply->pCar->aObjectsSmashed[i] * SceneryCrashScore;
		}
		if (current > last[playerId]) {
			AddScore(SCORE_SCENERY, "SCENERY CRASHES", current - last[playerId], ply->nPlayerId);
		}

		last[playerId] = current;
	}

	void ProcessAirtime(int playerId) {
		static int last[nMaxPlayers] = {};
		if (pPlayerHost->nRaceTime < 0) {
			memset(last,0,sizeof(last));
			return;
		}

		int current = 0;
		auto ply = GetPlayer(playerId);
		if (ply->pCar->fTimeInAir >= 0.5) {
			current = ((ply->pCar->fTimeInAir - 0.5) * 10) * AirtimeScore;
		}
		else {
			current = 0;
		}
		if (current > last[playerId]) {
			AddScore(SCORE_AIRTIME, "AIRTIME", (current - last[playerId]) * 10, ply->nPlayerId);
		}

		last[playerId] = current;
	}

	NyaVec3 GetCheckpointPosition() {
		auto split = GetPlayer(0)->nCurrentSplit;
		split += (nCheckpointInterval - (split % nCheckpointInterval));
		split %= pEnvironment->nNumSplitpoints;
		auto splitData = pEnvironment->aSplitpoints[split-1];
		return {splitData.fPosition[0], splitData.fPosition[1], splitData.fPosition[2]};
	}

	void ProcessCheckpoints(int playerId) {
		HUD_Minimap.gArcadeCheckpoint = GetCheckpointPosition();

		static int last[nMaxPlayers] = {};
		if (pPlayerHost->nRaceTime < 0) {
			memset(last,0,sizeof(last));
			return;
		}

		auto current = GetPlayer(playerId)->nCurrentSplit / nCheckpointInterval;
		if (current > last[playerId]) {
			AddScore(SCORE_CHECKPOINT, "CHECKPOINT!", CheckpointScore, playerId+1);
			OnCheckpointPassed();
		}
		last[playerId] = current;
	}

	void OnTick() {
		static CNyaRaceTimer gTimer;
		gTimer.Process();

		if (pLoadingScreen) return;
		if (!bIsCarnageRace) {
			memset(nPlayerScore, 0, sizeof(nPlayerScore));
			memset(nPlayerScoresByType, 0, sizeof(nPlayerScoresByType));
			return;
		}
		if (GetGameState() != GAME_STATE_RACE) return;

		if (!bIsArcadeMode) {
			nCheckpointInterval = DoesTrackValueExist(pGameFlow->nLevel, "IsFO2Track") ? 2 : 5;
		}

		if (bIsInMultiplayer) {
			for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
				auto ply = GetPlayer(i);
				if (IsPlayerScoreLocallyControlled(ply)) continue;
				nPlayerScore[i] = ChloeNet::GetReplicatedPlayerArcadeScore(ply);
			}
		}

		ArcadeMode::nCurrentEventScore = nPlayerScore[0];
		if (nPlayerScore[0] > gCustomSave.trackArcadeScores[pGameFlow->nLevel]) {
			gCustomSave.trackArcadeScores[pGameFlow->nLevel] = nPlayerScore[0];
		}

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayerScore(i+1);
			if (!ply->bHasFinished && !ply->bIsDNF) {
				ProcessSceneryCrashes(i);
				ProcessAirtime(i);
				ProcessCheckpoints(i);
			}
		}

		if (pPlayerHost->nRaceTime < 0) {
			fPlayerTimeLeft = fPlayerGivenTime;
			fScoreTimer = 0;
			fCashoutNotifTimer = 0;
			fCheckpointNotifTimer = 0;
			aScoreHUD.clear();
			memset(nPlayerScore, 0, sizeof(nPlayerScore));
			memset(nPlayerScoresByType, 0, sizeof(nPlayerScoresByType));
			return;
		}

		GetScoreManager()->nNumLaps = 10;

		fPlayerTimeLeft -= gTimer.fDeltaTime;
		fScoreTimer -= gTimer.fDeltaTime;
		fCashoutNotifTimer -= gTimer.fDeltaTime;
		fCheckpointNotifTimer -= gTimer.fDeltaTime;

		ArcadeMode::ProcessTimerTick(fPlayerTimeLeft*1000);

		auto ply = GetPlayerScore(1);
		if (fPlayerTimeLeft <= 0 && !ply->bHasFinished && !ply->bIsDNF) {
			ply->bHasFinished = true;
			ply->nFinishTime = pPlayerHost->nRaceTime;
		}

		if (!aScoreHUD.empty()) {
			if (fScoreTimer <= 0 || fPlayerTimeLeft <= 0 || IsPlayerWrecked(GetPlayer(0))) {
				CashOutCombo();
			}
		}
	}

	void Init() {
		ChloeEvents::CrashBonusEvent.AddHandler(OnCrashBonus);
		ChloeEvents::FinishFrameEvent.AddHandler(OnTick);
	}

	class CHUD_ArcadeRace : public CIngameHUDElement {
	public:

		tDrawPositions1080p gCheckpointBonus = {240, 85, 0.03};

		virtual void Process() {
			if (!IsRaceHUDUp()) return;
			if (!bIsCarnageRace) return;

			int timeLeft = fPlayerTimeLeft*1000;
			if (timeLeft < 0) timeLeft = 0;

			DrawElement(0, "TIME LEFT", FormatGameTime(timeLeft), timeLeft <= 4500 ? NyaDrawing::CNyaRGBA32(200,0,0,255) : NyaDrawing::CNyaRGBA32(255,255,255,255));
			//DrawElement(0, "TIME LEFT", timeLeftString, timeLeft <= 4500 ? GetPaletteColor(22) : NyaDrawing::CNyaRGBA32(255,255,255,255));
			DrawElement(1, "SCORE", FormatScore(nPlayerScore[0]));

			if (fCheckpointNotifTimer > 0) {
				tNyaStringData data;
				data.x = gCheckpointBonus.nPosX;
				data.y = gCheckpointBonus.nPosY;
				data.size = gCheckpointBonus.fSize;
				data.SetColor(GetPaletteColor(13));
				Draw1080pString(JUSTIFY_LEFT, data, std::format("+{} SECONDS", nCheckpointNotifAmount), &DrawStringFO2_Condensed12);
			}
		}
	} HUD_ArcadeRace;

	class CHUD_ArcadeCombo : public CIngameHUDElement {
	public:

		static inline tDrawPositions1080p gScoreHUD = {1700,350,0.033,20,32};

		void DrawScoreHUDEntry(int id, const std::string& left, const std::string& right) {
			int color1 = id == 0 ? 13 : 18;
			int color2 = id == 0 ? 13 : 17;

			tNyaStringData data;
			data.x = gScoreHUD.nPosX;
			data.y = gScoreHUD.nPosY + gScoreHUD.nSpacingY * id;
			data.size = gScoreHUD.fSize;
			data.XRightAlign = true;
			data.SetColor(GetPaletteColor(color1));
			Draw1080pString(JUSTIFY_RIGHT, data, left, &DrawStringFO2_Condensed12);
			data.x += gScoreHUD.nSpacingX;
			data.XRightAlign = false;
			data.SetColor(GetPaletteColor(color2));
			Draw1080pString(JUSTIFY_RIGHT, data, right, &DrawStringFO2_Condensed12);
		}

		std::string GetNthString(int num) {
			std::string str = std::to_string(num);
			if (num != 11 && num % 10 == 1) str += "ST";
			else if (num != 12 && num % 10 == 2) str += "ND";
			else if (num != 13 && num % 10 == 3) str += "RD";
			else str += "TH";
			return str;
		}

		virtual void Process() {
			if (!IsRaceHUDUp()) return;
			if (!bIsCarnageRace) return;

			int id = 0;
			if (!aScoreHUD.empty()) {
				DrawScoreHUDEntry(id++, std::format("POSITION ({})", GetNthString(nPlayerPosition)), std::format("{}X", (int)RacePositionMultiplier[nPlayerPosition-1]));
			}
			else if (fCashoutNotifTimer > 0) {
				DrawScoreHUDEntry(id++, "TOTAL", std::format("+{}", nCashoutNotifAmount));
			}
			for (auto& score : aScoreHUD) {
				DrawScoreHUDEntry(id++, score.name, std::to_string((int)score.points));
			}
		}
	} HUD_ArcadeCombo;
}