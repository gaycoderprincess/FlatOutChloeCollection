namespace CareerMode {
	bool bIsCareerRace = false;
	bool bLastRaceCareerRace = false;

	struct tLUAClass {
		struct tCup {
			struct tRace {
				int nLevel;
				int nLaps;
				int nAIHandicapLevel; // NOTE this can be -1 for derbies!!
			};

			std::string sName;
			int nAIUpgradeLevel;
			std::vector<tRace> aRaces;
			int aCupWinnings[3] = {1500,1000,750};
		};

		std::vector<int> aCarUnlocks;
		int nMoneyAward;
		std::vector<tCup> aCups;
		std::vector<tCup> aEvents;
	} aLUACareerClasses[4];

	tLUAClass* luaDefs_currentClass = nullptr;
	tLUAClass::tCup* luaDefs_currentCup = nullptr;

	const int aPointsPerPosition[nNumCareerMaxPlayers] = {
			10,
			8,
			6,
			5,
			4,
			3,
			2,
			1,
	};

	struct tPlayerResult {
		int nPosition;
		bool bFinished;
		bool bDNF;
		uint32_t nFinishTime;
	};
	tPlayerResult aPlayerResults[nNumCareerMaxPlayers];
	bool bPlayerResultsApplied = false;

	void SetIsCareerMode(bool apply) {
		bIsCareerRace = apply;
		NyaHookLib::Patch<uint8_t>(0x43F505, apply ? 0xEB : 0x74); // use career car
		NyaHookLib::Patch<uint8_t>(0x431B08, apply ? 0xEB : 0x75); // don't null upgrades
		// todo what does 0043BD71 do?
	}

	auto GetCurrentCup() {
		return &aLUACareerClasses[gCustomSave.nCareerClass-1].aCups[gCustomSave.nCareerCup-1];
	}

	auto GetCurrentRace() {
		return &aLUACareerClasses[gCustomSave.nCareerClass-1].aCups[gCustomSave.nCareerCup-1].aRaces[gCustomSave.nCareerCupNextEvent];
	}

	void OnCupFinished() {
		gCustomSave.nCareerCup = 0;
		gCustomSave.nCareerCupNextEvent = 0;
	}

	void ProcessResultsFromLastRace() {
		// don't proceed if we quit the race
		if (!aPlayerResults[0].bDNF && !aPlayerResults[0].bFinished) return;

		// todo change these checks after adding single events
		int eventNumber = gCustomSave.nCareerCupNextEvent++;
		for (int i = 0; i < nNumCareerMaxPlayers; i++) {
			auto results = &aPlayerResults[i];
			auto player = &gCustomSave.aCareerCupPlayers[i];
			player->eventPosition[eventNumber] = results->nPosition;
			if (results->bDNF) {
				player->eventPoints[eventNumber] = 0;
			}
			else {
				player->eventPoints[eventNumber] = aPointsPerPosition[results->nPosition-1];
				player->points += aPointsPerPosition[results->nPosition-1];
			}
		}
		gCustomSave.CalculateCupPlayersByPosition();
		if (gCustomSave.nCareerCupNextEvent >= GetCurrentCup()->aRaces.size()) {
			OnCupFinished();
		}

		SetIsCareerMode(false);
	}

	bool IsCupActive() {
		return gCustomSave.nCareerCup >= 0 && gCustomSave.nCareerCupNextEvent > 0;
	}

	void OnTick() {
		if (pLoadingScreen) return;
		if (GetGameState() == GAME_STATE_RACE) {
			bLastRaceCareerRace = bIsCareerRace;
		}

		if (!bIsCareerRace) return;

		if (GetGameState() == GAME_STATE_RACE) {
			bPlayerResultsApplied = false;
			memset(aPlayerResults, 0, sizeof(aPlayerResults));
			for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
				auto results = &aPlayerResults[i];
				auto player = GetPlayer(i);
				auto score = GetPlayerScore<PlayerScoreRace>(player->nPlayerId);
				results->bFinished = score->bHasFinished;
				results->bDNF = score->bIsDNF;
				results->nPosition = score->nPosition;
				results->nFinishTime = score->nFinishTime;
			}
		}
		if (GetGameState() == GAME_STATE_MENU && !bPlayerResultsApplied) {
			ProcessResultsFromLastRace();
			bPlayerResultsApplied = true;
		}
	}

	int __stdcall GetAIHandicapLevelNew(GameFlow* gameFlow) {
		if (bIsCareerRace) {
			int handicap = GetCurrentRace()->nAIHandicapLevel;
			if (handicap >= 1) return handicap;
		}

		int level = gameFlow->nLevel;
		if (DoesTrackValueExist(level, "AIHandicapLevel")) {
			return GetTrackValueNumber(level, "AIHandicapLevel");
		}

		// not dependent on car class
		switch (gameFlow->Profile.nCurrentClass) {
			case 1:
			default:
				return 1;
				break;
			case 2:
				return 3;
				break;
			case 3:
				return 5;
				break;
		}
	}

	int __stdcall GetAIUpgradeLevelNew(GameFlow* gameFlow) {
		if (bIsCareerRace) {
			return GetCurrentCup()->nAIUpgradeLevel;
		}

		int level = gameFlow->nLevel;
		if (DoesTrackValueExist(level, "AIUpgradeLevel")) {
			return GetTrackValueNumber(level, "AIUpgradeLevel");
		}
		return 1;
	}

	int __stdcall GetNumLapsNew(GameFlow* gameFlow) {
		if (bIsCareerRace) {
			return GetCurrentRace()->nLaps;
		}

		int level = gameFlow->nLevel;
		if (DoesTrackValueExist(level, "Laps")) {
			return GetTrackValueNumber(level, "Laps");
		}
		return 4;
	}

	void Init() {
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x440560, &GetAIHandicapLevelNew);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x440650, &GetAIUpgradeLevelNew);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4404A0, &GetNumLapsNew);
	}
}