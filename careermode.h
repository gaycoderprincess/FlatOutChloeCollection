namespace CareerMode {
	bool bIsCareerRace = false;
	bool bNextRaceCareerRace = false;
	bool bLastRaceCareerRace = false;
	bool bWasCareerCupJustFinished = false;
	std::string sLastCupName;
	int nLastCupNumRaces;

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
		std::vector<tCup> aCups;
		std::vector<tCup> aEvents;
		tCup Finals;
	} aLUACareerClasses[nNumCareerClasses];

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
		int playerPosition = gCustomSave.aCupPlayerPosition[0]+1;
		auto cup = GetCurrentCup();
		if (cup->aRaces[0].nLevel == TRACK_LONGJUMP) {
			playerPosition = 4;
			if (aPlayerResults[0].nFinishTime >= 100) playerPosition = 3;
			if (aPlayerResults[0].nFinishTime >= 200) playerPosition = 2;
			if (aPlayerResults[0].nFinishTime >= 250) playerPosition = 1;
		}
		if (cup->aRaces[0].nLevel == TRACK_HIGHJUMP) {
			playerPosition = 4;
			if (aPlayerResults[0].nFinishTime >= 100) playerPosition = 3;
			if (aPlayerResults[0].nFinishTime >= 250) playerPosition = 2;
			if (aPlayerResults[0].nFinishTime >= 300) playerPosition = 1;
		}
		if (cup->aRaces[0].nLevel == TRACK_BOWLING) {
			playerPosition = 4;
			if (aPlayerResults[0].nFinishTime >= 10) playerPosition = 3;
			if (aPlayerResults[0].nFinishTime >= 20) playerPosition = 2;
			if (aPlayerResults[0].nFinishTime >= 25) playerPosition = 1;
		}

		if (auto cup = GetCurrentSaveCup()) {
			if (!cup->nPosition || cup->nPosition > playerPosition) {
				cup->nPosition = playerPosition;
			}

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
			if (!cup->nPosition || cup->nPosition > playerPosition) {
				cup->nPosition = playerPosition;
			}
			cup->nTimeOrScore = aPlayerResults[0].nFinishTime;
		}

		sLastCupName = cup->sName;
		nLastCupNumRaces = cup->aRaces.size();
		if (nLastCupNumRaces > 1) {
			bWasCareerCupJustFinished = true;
		}

		if (playerPosition >= 1 && playerPosition <= 3) {
			pGameFlow->Profile.nMoney += cup->aCupWinnings[playerPosition-1];
			pGameFlow->Profile.nMoneyGained += cup->aCupWinnings[playerPosition-1];
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
			if (pGameFlow->nEventType == eEventType::RACE && results->bDNF) {
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

	void ProcessResultsFromLastRace_Prompted() {
		if (GetGameState() == GAME_STATE_MENU && !bPlayerResultsApplied) {
			ProcessResultsFromLastRace();
			bPlayerResultsApplied = true;
		}
	}

	void OnTick() {
		if (pLoadingScreen) return;
		int gameState = GetGameState();
		if (gameState == GAME_STATE_RACE || gameState == GAME_STATE_REPLAY) {
			if (bNextRaceCareerRace) {
				bIsCareerRace = true;
				bNextRaceCareerRace = false;
			}
			bLastRaceCareerRace = bIsCareerRace;
		}

		if (!bIsCareerRace) return;

		if (gameState == GAME_STATE_RACE || gameState == GAME_STATE_REPLAY) {
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

				if (pGameFlow->nSubEventType == eSubEventType::STUNT_LONGJUMP || pGameFlow->nSubEventType == eSubEventType::STUNT_HIGHJUMP) {
					results->nFinishTime = ((score->nStuntMetersScore[0] + score->nStuntMetersScore[1] + score->nStuntMetersScore[2]) * 0.01);
				}
				if (pGameFlow->nSubEventType == eSubEventType::STUNT_BOWLING) {
					results->nFinishTime = score->nStuntPointsScore[0] + score->nStuntPointsScore[1] + score->nStuntPointsScore[2];
				}
			}
		}
		ProcessResultsFromLastRace_Prompted();
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

	void OnSave() {
		float cupsCompleted = 0;
		int cupsCompletedCount = 0;
		int cupsTotal = 0;
		float eventsCompleted = 0;
		int eventsCompletedCount = 0;
		int eventsTotal = 0;
		for (int classId = 0; classId < 3; classId++) {
			auto careerClass = &CareerMode::aLUACareerClasses[classId];
			if (careerClass->aCups.empty()) continue;

			cupsTotal += careerClass->aCups.size()+1;
			eventsTotal += careerClass->aEvents.size();

			for (int i = 0; i < careerClass->aCups.size(); i++) {
				auto cup = &gCustomSave.aCareerClasses[classId].aCups[i];
				if (cup->nPosition >= 1 && cup->nPosition <= 3) cupsCompletedCount++;

				if (cup->nPosition == 1) cupsCompleted += 1;
				else if (cup->nPosition == 2) cupsCompleted += 0.5;
				else if (cup->nPosition == 3) cupsCompleted += 0.25;
			}
			for (int i = 0; i < careerClass->aEvents.size(); i++) {
				auto cup = &gCustomSave.aCareerClasses[classId].aEvents[i];
				if (cup->nPosition >= 1 && cup->nPosition <= 3) eventsCompletedCount++;

				if (cup->nPosition == 1) eventsCompleted += 1;
				else if (cup->nPosition == 2) eventsCompleted += 0.5;
				else if (cup->nPosition == 3) eventsCompleted += 0.25;
			}
			auto cup = &gCustomSave.aCareerClasses[classId].Finals;
			if (cup->nPosition >= 1 && cup->nPosition <= 3) cupsCompletedCount++;

			if (cup->nPosition == 1) cupsCompleted += 1;
			else if (cup->nPosition == 2) cupsCompleted += 0.5;
			else if (cup->nPosition == 3) cupsCompleted += 0.25;
		}

		if (auto achievement = GetAchievement("COMPLETE_CAREER")) {
			achievement->fInternalProgress = cupsCompletedCount;
			achievement->fMaxInternalProgress = cupsTotal;
		}
		if (auto achievement = GetAchievement("COMPLETE_CAREER_GOLD")) {
			achievement->fInternalProgress = cupsCompleted + eventsCompleted;
			achievement->fMaxInternalProgress = cupsTotal + eventsTotal;
		}

		gCustomSave.nGameCompletion = ((double)(cupsCompleted + eventsCompleted) / (double)(cupsTotal + eventsTotal)) * 100;
		gCustomSave.nCupsPassed = cupsCompletedCount;
		gCustomSave.nCupsMax = cupsTotal;

		gCustomSave.nCarsUnlocked = 0;
		for (auto& car : aDealerCars) {
			if (car.classId < 1 || car.classId > 3) {
				gCustomSave.nCarsUnlocked++;
				continue;
			}

			if (gCustomSave.bCareerClassUnlocked[car.classId-1]) gCustomSave.nCarsUnlocked++;
		}
	}

	void Init() {
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x440560, &GetAIHandicapLevelNew);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x440650, &GetAIUpgradeLevelNew);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4404A0, &GetNumLapsNew);
	}
}