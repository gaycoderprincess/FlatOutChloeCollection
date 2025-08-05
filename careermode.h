namespace CareerMode {
	bool bIsCareerRace = false;
	bool bNextRaceCareerRace = false;
	bool bLastRaceCareerRace = false;

	struct tLUAClass {
		struct tCup {
			struct tRace {
				int nLevel;
				int nLaps;
				int nAIHandicapLevel; // NOTE this can be -1 for derbies!!
				bool bIsDerby : 1;
				bool bIsTimeTrial : 1;
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
		tCup Finals;
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
		bNextRaceCareerRace = apply;
		NyaHookLib::Patch<uint8_t>(0x43F505, apply ? 0xEB : 0x74); // use career car
		NyaHookLib::Patch<uint8_t>(0x431B08, apply ? 0xEB : 0x75); // don't null upgrades
		// todo what does 0043BD71 do?
	}

	auto GetCurrentCup() {
		if (gCustomSave.nCareerEvent > 0) return &aLUACareerClasses[gCustomSave.nCareerClass-1].aEvents[gCustomSave.nCareerEvent-1];
		if (gCustomSave.nCareerCup == 64) return &aLUACareerClasses[gCustomSave.nCareerClass-1].Finals;
		return &aLUACareerClasses[gCustomSave.nCareerClass-1].aCups[gCustomSave.nCareerCup-1];
	}

	auto GetCurrentRace() {
		return &GetCurrentCup()->aRaces[gCustomSave.nCareerCupNextEvent];
	}

	tCustomSaveStructure::tCareerClass::tCareerCup* GetCurrentSaveCup() {
		if (gCustomSave.nCareerEvent > 0) return nullptr;
		if (gCustomSave.nCareerCup == 64) return &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1].Finals;
		return &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1].aCups[gCustomSave.nCareerCup-1];
	}

	tCustomSaveStructure::tCareerClass::tCareerEvent* GetCurrentSaveCupAssociatedEvent() {
		if (gCustomSave.nCareerCup == 64) return nullptr;
		return &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1].aEvents[gCustomSave.nCareerCup-1];
	}

	tCustomSaveStructure::tCareerClass::tCareerEvent* GetCurrentSaveEvent() {
		if (gCustomSave.nCareerEvent == 0) return nullptr;
		return &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1].aEvents[gCustomSave.nCareerEvent-1];
	}

	void OnCupFinished() {
		if (auto cup = GetCurrentSaveCup()) {
			cup->nPosition = gCustomSave.aCupPlayerPosition[0]+1;

			// unlock the next cup if finished 3rd or higher
			if (cup->nPosition >= 1 && cup->nPosition <= 3) {
				auto nextCup = cup + 1;
				nextCup->bUnlocked = true;

				// unlock associated event with this cup
				if (auto event = GetCurrentSaveCupAssociatedEvent()) {
					event->bUnlocked = true;
				}

				// unlock next class after finals are done
				if (gCustomSave.nCareerCup == 64 && gCustomSave.nCareerClass < 4) {
					gCustomSave.bCareerClassUnlocked[gCustomSave.nCareerClass] = true;
				}
			}
		}
		else if (auto cup = GetCurrentSaveEvent()) {
			cup->nPosition = gCustomSave.aCupPlayerPosition[0]+1;
			cup->nTimeOrScore = aPlayerResults[0].nFinishTime;
		}

		// unlock finals after last cup gets finished
		auto careerClass = &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1];
		auto careerClassSave = &aLUACareerClasses[gCustomSave.nCareerClass-1];
		auto lastCup = &careerClass->aCups[careerClassSave->aCups.size()-1];
		if (lastCup->nPosition >= 1 && lastCup->nPosition <= 3) {
			careerClass->Finals.bUnlocked = true;
		}

		gCustomSave.nCareerCup = 0;
		gCustomSave.nCareerCupNextEvent = 0;
		gCustomSave.nCareerEvent = 0;
	}

	void ProcessResultsFromLastRace() {
		SetIsCareerMode(false);

		// don't proceed if we quit the race
		if (!aPlayerResults[0].bDNF && !aPlayerResults[0].bFinished) return;

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
	}

	bool IsCupActive() {
		return gCustomSave.nCareerCup >= 0 && gCustomSave.nCareerCupNextEvent > 0;
	}

	void OnTick() {
		if (pLoadingScreen) return;
		if (GetGameState() == GAME_STATE_RACE) {
			if (bNextRaceCareerRace) {
				bIsCareerRace = true;
				bNextRaceCareerRace = false;
			}
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
		if (bNextRaceCareerRace) {
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
		if (bNextRaceCareerRace) {
			return GetCurrentCup()->nAIUpgradeLevel;
		}

		int level = gameFlow->nLevel;
		if (DoesTrackValueExist(level, "AIUpgradeLevel")) {
			return GetTrackValueNumber(level, "AIUpgradeLevel");
		}
		return 1;
	}

	int __stdcall GetNumLapsNew(GameFlow* gameFlow) {
		if (bNextRaceCareerRace) {
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