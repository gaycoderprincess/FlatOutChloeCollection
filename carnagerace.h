namespace CarnageRace {
	int aRacePositionMultiplier[] = {
			5,
			4,
			3,
			3,
			2,
			2,
			1,
			1,
	};

	int nSceneryCrashScore = 15;
	int nAirtimeScore = 10;
	int nCheckpointScore = 200;

	double fPlayerGivenTime;
	double fCheckpointTimeBonus;
	double fCheckpointTimeDecay;
	int nCheckpointInterval;

	struct tScoreEntry {
		std::string name;
		double points;
	};
	std::vector<tScoreEntry> aScoreHUD;

	double fPlayerTimeLeft = 0;
	int nPlayerScore = 0;
	int nPlayerPosition = 0;

	const double fScoreMaxTime = 4;
	const double fScoreMaxTimeFull = 2;
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

	void AddScore(std::string name, double points) {
		if (IsPlayerWrecked(GetPlayer(0))) return;

		auto ply = GetPlayerScore<PlayerScoreRace>(1);
		if (ply->nPosition < nPlayerPosition || aScoreHUD.empty()) {
			nPlayerPosition = ply->nPosition;
		}
		fScoreTimer = fScoreMaxTime;

		if (name == "SCENERY CRASHES" || name == "AIRTIME") {
			for (auto& score : aScoreHUD) {
				if (score.name == name) {
					score.points += points;
					return;
				}
			}
		}

		tScoreEntry entry;
		entry.name = name;
		entry.points = points;
		aScoreHUD.push_back(entry);

		if (aScoreHUD.size() >= 10) {
			fScoreTimer = fScoreMaxTimeFull;
			Achievements::AwardAchievement(GetAchievement("CARNAGE_FILL_BOARD"));
		}
		if (aScoreHUD.size() >= 15) {
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
		if (pPlayer->nPlayerType != PLAYERTYPE_LOCAL) return;
		AddScore(GetCrashBonusName(type), GetCrashBonusPrice(type));
	}

	void SetIsCarnageRace(bool apply) {
		bIsCarnageRace = apply;
		fPlayerGivenTime = 120;
		fCheckpointTimeBonus = 7;
		fCheckpointTimeDecay = 0.75;
		nCheckpointInterval = 5;

		// hud changes
		NyaHookLib::Patch<uint64_t>(0x454AFC, apply ? 0xE0A190000001EEE9 : 0xE0A1000001ED850F); // remove total time
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
			pointsAwarded += score.points * aRacePositionMultiplier[nPlayerPosition-1];
		}
		nPlayerScore += pointsAwarded;
		fCashoutNotifTimer = fScoreMaxTime;
		nCashoutNotifAmount = pointsAwarded;
		aScoreHUD.clear();
	}

	void ProcessSceneryCrashes() {
		static int last = 0;
		if (pPlayerHost->nRaceTime < 0) {
			last = 0;
			return;
		}

		int current = 0;
		auto ply = GetPlayer(0);
		for (int i = 0; i < 10; i++) {
			current += ply->pCar->aObjectsSmashed[i] * nSceneryCrashScore;
		}
		if (current > last) {
			AddScore("SCENERY CRASHES", current - last);
		}

		last = current;
	}

	void ProcessAirtime() {
		static int last = 0;
		if (pPlayerHost->nRaceTime < 0) {
			last = 0;
			return;
		}

		int current = 0;
		auto ply = GetPlayer(0);
		if (ply->pCar->fTimeInAir >= 0.5) {
			current = ((ply->pCar->fTimeInAir - 0.5) * 10) * nAirtimeScore;
		}
		else {
			current = 0;
		}
		if (current > last) {
			AddScore("AIRTIME", (current - last) * 10);
		}

		last = current;
	}

	NyaVec3 GetCheckpointPosition() {
		auto split = GetPlayer(0)->nCurrentSplit;
		split += (nCheckpointInterval - (split % nCheckpointInterval));
		split %= pEnvironment->nNumSplitpoints;
		auto splitData = pEnvironment->aSplitpoints[split-1];
		return {splitData.fPosition[0], splitData.fPosition[1], splitData.fPosition[2]};
	}

	void ProcessCheckpoints() {
		HUD_Minimap.gArcadeCheckpoint = GetCheckpointPosition();

		static int last = 0;
		if (pPlayerHost->nRaceTime < 0) {
			last = 0;
			return;
		}

		auto current = GetPlayer(0)->nCurrentSplit / nCheckpointInterval;
		if (current > last) {
			AddScore("CHECKPOINT!", nCheckpointScore);
			OnCheckpointPassed();
		}
		last = current;
	}

	void OnTick() {
		static CNyaRaceTimer gTimer;
		gTimer.Process();

		if (pLoadingScreen) return;
		if (!bIsCarnageRace) {
			nPlayerScore = 0;
			return;
		}
		if (GetGameState() != GAME_STATE_RACE) return;

		ArcadeMode::nCurrentEventScore = nPlayerScore;

		auto ply = GetPlayerScore<PlayerScoreRace>(1);
		if (!ply->bHasFinished && !ply->bIsDNF) {
			ProcessSceneryCrashes();
			ProcessAirtime();
			ProcessCheckpoints();
		}

		if (pPlayerHost->nRaceTime < 0) {
			fPlayerTimeLeft = fPlayerGivenTime;
			fScoreTimer = 0;
			fCashoutNotifTimer = 0;
			fCheckpointNotifTimer = 0;
			aScoreHUD.clear();
			nPlayerScore = 0;
			return;
		}

		GetScoreManager()->nNumLaps = 10;

		fPlayerTimeLeft -= gTimer.fDeltaTime;
		fScoreTimer -= gTimer.fDeltaTime;
		fCashoutNotifTimer -= gTimer.fDeltaTime;
		fCheckpointNotifTimer -= gTimer.fDeltaTime;

		ArcadeMode::ProcessTimerTick(fPlayerTimeLeft*1000);

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
			DrawElement(1, "SCORE", FormatScore(nPlayerScore));

			if (fCheckpointNotifTimer > 0) {
				tNyaStringData data;
				data.x = gCheckpointBonus.nPosX;
				data.y = gCheckpointBonus.nPosY;
				data.size = gCheckpointBonus.fSize;
				data.SetColor(GetPaletteColor(13));
				Draw1080pString(JUSTIFY_LEFT, data, std::format("+{} SECONDS", nCheckpointNotifAmount), &DrawStringFO2_Ingame12);
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
			Draw1080pString(JUSTIFY_RIGHT, data, left, &DrawStringFO2_Ingame12);
			data.x += gScoreHUD.nSpacingX;
			data.XRightAlign = false;
			data.SetColor(GetPaletteColor(color2));
			Draw1080pString(JUSTIFY_RIGHT, data, right, &DrawStringFO2_Ingame12);
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
				DrawScoreHUDEntry(id++, std::format("POSITION ({})", GetNthString(nPlayerPosition)), std::format("{}X", aRacePositionMultiplier[nPlayerPosition-1]));
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