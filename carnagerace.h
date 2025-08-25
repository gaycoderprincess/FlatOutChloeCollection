namespace CarnageRace {
	bool bIsCarnageRace = false;

	struct tScoreEntry {
		std::string name;
		double points;
	};
	std::vector<tScoreEntry> aScoreHUD;

	int nPlayerScore = 0;

	const double fScoreMaxTime = 5;
	double fScoreTimer = 0;

	double fCashoutNotifTimer = 0;
	int nCashoutNotifAmount = 0;

	void AddScore(std::string name, double points) {
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
		aScoreHUD.push_back({GetCrashBonusName(type), GetCrashBonusPrice(type)});
	}

	void Init() {
		ChloeEvents::OnCrashBonus.push_back(OnCrashBonus);
	}

	CHUDElement::tDrawPositions1080p gScoreHUD = {0,0,0.03,0,16};
	CHUDElement::tDrawPositions1080p gScoreHUD2 = {0,0};

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

	void DrawScoreHUDEntry(int id, const std::string& left, const std::string& right) {
		int color1 = id == 0 ? 13 : 18;
		int color2 = id == 0 ? 13 : 17;

		tNyaStringData data;
		data.x = gScoreHUD.nPosX;
		data.y = gScoreHUD.nPosY;
		data.size = gScoreHUD.fSize;
		data.XRightAlign = true;
		data.SetColor(GetPaletteColor(color1));
		CHUDElement::Draw1080pString(CHUDElement::JUSTIFY_RIGHT, data, left, &DrawStringFO2_Ingame12);
		data.x = gScoreHUD2.nPosX;
		data.XRightAlign = false;
		data.SetColor(GetPaletteColor(color2));
		CHUDElement::Draw1080pString(CHUDElement::JUSTIFY_RIGHT, data, right, &DrawStringFO2_Ingame12);
	}

	std::string GetNthString(int num) {
		std::string str = std::to_string(num);
		if (num != 11 && num % 10 == 1) str += "st";
		else if (num != 12 && num % 10 == 2) str += "nd";
		else if (num != 13 && num % 10 == 3) str += "rd";
		else str += "th";
		return str;
	}

	void CashOutCombo() {
		double pointsAwarded = 0;
		auto ply = GetPlayerScore<PlayerScoreRace>(1);
		for (auto& score : aScoreHUD) {
			pointsAwarded += score.points * aRacePositionMultiplier[ply->nPosition-1];
		}
		nPlayerScore += pointsAwarded;
		fCashoutNotifTimer = fScoreMaxTime;
		nCashoutNotifAmount = pointsAwarded;
		aScoreHUD.clear();
	}

	int nSceneryCrashScore = 15;
	int nAirtimeScore = 10;
	int nCheckpointScore = 200;

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
			current = ((ply->pCar->fTimeInAir - 0.5) * 100) * nAirtimeScore;
		}
		else {
			current = 0;
		}
		if (current > last) {
			AddScore("AIRTIME", current - last);
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
		if (pPlayerHost->nRaceTime < 0) {
			aScoreHUD.clear();
			nPlayerScore = 0;
			return;
		}

		int id = 0;
		if (!aScoreHUD.empty()) {
			auto ply = GetPlayerScore<PlayerScoreRace>(1);
			DrawScoreHUDEntry(id++, std::format("POSITION ({}{})", ply->nPosition, GetNthString(ply->nPosition)), std::format("{}X", aRacePositionMultiplier[ply->nPosition-1]));
		}
		else if (fCashoutNotifTimer > 0) {
			DrawScoreHUDEntry(id++, "TOTAL", std::format("+{}", nCashoutNotifAmount));
		}
		for (auto& score : aScoreHUD) {
			DrawScoreHUDEntry(id++, score.name, std::to_string((int)score.points));
		}

		fScoreTimer -= gTimer.Process();
		fCashoutNotifTimer -= gTimer.Process();

		// todo cash out at the end of the race
		if (fScoreTimer <= 0) {
			CashOutCombo();
		}
	}
}