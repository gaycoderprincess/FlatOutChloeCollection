namespace CarnageRace {
	bool bIsCarnageRace = true;

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

	double fPlayerGivenTime = 120;
	double fCheckpointTimeBonus = 7;
	double fCheckpointTimeDecay = 0.75;

	struct tScoreEntry {
		std::string name;
		double points;
	};
	std::vector<tScoreEntry> aScoreHUD;

	double fPlayerTimeLeft = 0;
	int nPlayerScore = 0;
	int nPlayerPosition = 0;

	const double fScoreMaxTime = 5;
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
		if (pPlayer->nPlayerType != PLAYERTYPE_LOCAL) return;
		AddScore(GetCrashBonusName(type), GetCrashBonusPrice(type));
	}

	void SetIsCarnageRace(bool apply) {
		bIsCarnageRace = apply;

		// hud changes
		NyaHookLib::Patch<uint64_t>(0x454AFC, apply ? 0xE0A190000001EEE9 : 0xE0A1000001ED850F); // remove total time
		NyaHookLib::Patch<uint8_t>(0x4551E9, apply ? 0xEB : 0x74); // remove lap time title
		NyaHookLib::Patch<uint8_t>(0x45578E, apply ? 0xEB : 0x74); // remove lap time counter
	}

	void Init() {
		SetIsCarnageRace(true);
		ChloeEvents::OnCrashBonus.push_back(OnCrashBonus);
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

	void ProcessCheckpoints() {
		static int last = 0;
		if (pPlayerHost->nRaceTime < 0) {
			last = 0;
			return;
		}

		auto current = GetPlayer(0)->nCurrentSplit / 5;
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
		if (!bIsCarnageRace) return;
		if (GetGameState() != GAME_STATE_RACE) return;

		ProcessSceneryCrashes();
		ProcessAirtime();
		ProcessCheckpoints();
		if (pPlayerHost->nRaceTime < 0) {
			fPlayerTimeLeft = fPlayerGivenTime;
			aScoreHUD.clear();
			nPlayerScore = 0;
			return;
		}

		fPlayerTimeLeft -= gTimer.fDeltaTime;
		fScoreTimer -= gTimer.fDeltaTime;
		fCashoutNotifTimer -= gTimer.fDeltaTime;
		fCheckpointNotifTimer -= gTimer.fDeltaTime;

		if (fPlayerTimeLeft <= 0) {
			auto ply = GetPlayerScore<PlayerScoreRace>(1);
			ply->bIsDNF = true;
			GetPlayer(0)->nInputFlags.bHoldBrake = true;
		}

		if (!aScoreHUD.empty()) {
			if (fScoreTimer <= 0 || fPlayerTimeLeft <= 0 || IsPlayerWrecked(GetPlayer(0))) {
				CashOutCombo();
			}
		}
	}

	class CHUD_ArcadeRace : public CIngameHUDElement {
	public:

		tDrawPositions1080p gCheckpointBonus = {240, 85, 0.03};
		static inline tDrawPositions gBase = {0.008, 0.029, 0.042, 0, 0.034};
		float fElementTotalSpacing = 0.092;

		void DrawElement(int id, const std::string& title, const std::string& value) {
			tNyaStringData data;
			data.x = gBase.fPosX * GetAspectRatioInv();
			data.y = gBase.fPosY + id * fElementTotalSpacing;
			data.size = gBase.fSize;
			data.SetColor(GetPaletteColor(18));
			DrawStringFO2_Ingame12(data, title);
			data.y += gBase.fSpacingY;
			data.SetColor({255,255,255,255});
			DrawStringFO2_Ingame24(data, value);
		}

		virtual void Process() {
			if (!IsRaceHUDUp()) return;
			if (!bIsCarnageRace) return;

			std::string timeLeftString = GetTimeFromMilliseconds(fPlayerTimeLeft*1000, true);
			timeLeftString.pop_back();
			// leading zero
			if (fPlayerTimeLeft < 60 * 10) {
				timeLeftString = "0" + timeLeftString;
			}

			DrawElement(0, "TIME LEFT", timeLeftString);
			DrawElement(1, "SCORE", std::to_string(nPlayerScore));

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