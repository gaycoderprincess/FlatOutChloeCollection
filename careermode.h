namespace CareerMode {
	bool bWasCareerCupJustFinished = false;
	std::string sLastCupName;
	int nLastCupNumRaces;
	int nLastCupAward;
	int nLastRaceAwardTotal = 0;

	struct tLUAClass {
		struct tCup {
			struct tRace {
				int nLevel;
				int nLaps;
				int nAIHandicapLevel; // NOTE this can be -1 for derbies!!
				bool bReversed : 1;
				bool bIsDerby : 1;
				bool bIsTimeTrial : 1;
				int nTimeTrialCar;
			};

			std::string sName;
			float fAIUpgradeLevel;
			std::vector<tRace> aRaces;
			int aCupWinnings[3] = {1500,1000,750};
			std::vector<int> aCarUnlocks;
		};

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
		int aSceneryBonuses[10];
		int aCrashBonuses[NUM_CRASHBONUS_TYPES];
	};
	tPlayerResult aPlayerResults[nNumCareerMaxPlayers];
	bool bPlayerResultsApplied = false;

	tLUAClass::tCup* GetCurrentCup() {
		if (gCustomSave.nCareerClass < 1 || gCustomSave.nCareerClass > 3) return nullptr;
		if (gCustomSave.nCareerEvent > aLUACareerClasses[gCustomSave.nCareerClass-1].aEvents.size()) return nullptr;
		if (gCustomSave.nCareerCup != 64 && gCustomSave.nCareerCup > aLUACareerClasses[gCustomSave.nCareerClass-1].aCups.size()) return nullptr;

		if (gCustomSave.nCareerEvent > 0) return &aLUACareerClasses[gCustomSave.nCareerClass-1].aEvents[gCustomSave.nCareerEvent-1];
		if (gCustomSave.nCareerCup == 64) return &aLUACareerClasses[gCustomSave.nCareerClass-1].Finals;
		if (gCustomSave.nCareerCup > 0) return &aLUACareerClasses[gCustomSave.nCareerClass-1].aCups[gCustomSave.nCareerCup-1];
		return nullptr;
	}

	tLUAClass::tCup::tRace* GetCurrentRace() {
		auto cup = GetCurrentCup();
		if (!cup) return nullptr;
		if (gCustomSave.nCareerCupNextEvent >= cup->aRaces.size()) return nullptr;
		return &cup->aRaces[gCustomSave.nCareerCupNextEvent];
	}

	tCustomSaveStructure::tCareerClass::tCareerCup* GetCurrentSaveCup() {
		if (gCustomSave.nCareerEvent > 0) return nullptr;
		if (gCustomSave.nCareerCup == 64) return &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1].Finals;
		if (gCustomSave.nCareerCup > 0) return &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1].aCups[gCustomSave.nCareerCup-1];
		return nullptr;
	}

	tCustomSaveStructure::tCareerClass::tCareerEvent* GetCurrentSaveCupAssociatedEvent() {
		if (gCustomSave.nCareerCup == 64) return nullptr;
		if (gCustomSave.nCareerCup > 0) return &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1].aEvents[gCustomSave.nCareerCup-1];
		return nullptr;
	}

	tCustomSaveStructure::tCareerClass::tCareerEvent* GetCurrentSaveEvent() {
		if (gCustomSave.nCareerEvent == 0) return nullptr;
		return &gCustomSave.aCareerClasses[gCustomSave.nCareerClass-1].aEvents[gCustomSave.nCareerEvent-1];
	}

	void SetIsCareerMode(bool apply) {
		bIsCareerRace = apply;
		NyaHookLib::Patch<uint8_t>(0x43F505, apply ? 0xEB : 0x74); // use career car
		NyaHookLib::Patch<uint64_t>(0x43F490, apply ? 0x448B909090909090 : 0x448B000004BC850F); // use sp player spawning for splitscreen career
	}

	void SetIsCareerModeTimeTrial(bool apply) {
		bIsCareerRace = apply;
	}

	int GetCrashBonusPrice(int type) {
		switch (type) {
			case CRASHBONUS_SUPERFLIP:
				return 200;
			case CRASHBONUS_SLAM:
				return 100;
			case CRASHBONUS_POWERHIT:
				return 200;
			case CRASHBONUS_BLASTOUT:
				return 300;
			case CRASHBONUS_WRECKED:
				return 1500;
			case CRASHBONUS_RAGDOLLED:
				return 400;
			default:
				return 0;
		}
	}

	void OnRaceFinished() {
		int numLocalPlayers = pGameFlow->nGameMode == eGameMode::SPLITSCREEN ? pGameFlow->nNumSplitScreenPlayers : 1;
		for (int j = 0; j < numLocalPlayers; j++) {
			for (int i = 0; i < 10; i++) {
				auto cashSceneryBonuses = aPlayerResults[j].aSceneryBonuses[i] * fBonusTypePrice[i];
				pGameFlow->Profile.nMoney += cashSceneryBonuses;
				pGameFlow->Profile.nMoneyGained += cashSceneryBonuses;
				nLastRaceAwardTotal += cashSceneryBonuses;
			}
			for (int i = 0; i < NUM_CRASHBONUS_TYPES; i++) {
				auto cashCrashBonuses = aPlayerResults[j].aCrashBonuses[i] * GetCrashBonusPrice(i);
				pGameFlow->Profile.nMoney += cashCrashBonuses;
				pGameFlow->Profile.nMoneyGained += cashCrashBonuses;
				nLastRaceAwardTotal += cashCrashBonuses;
			}
		}

		if (auto achievement = GetAchievement("CASH_DESTRUCTION")) {
			if (nLastRaceAwardTotal > achievement->fInternalProgress) achievement->fInternalProgress = nLastRaceAwardTotal;
		}
		nLastRaceAwardTotal = 0;
	}

	std::vector<int> aNewlyUnlockedCars;

	void GetStuntTargets(int level, int* out) {
		switch (level) {
			case TRACK_LONGJUMP:
				out[0] = 250;
				out[1] = 200;
				out[2] = 100;
				break;
			case TRACK_HIGHJUMP:
				out[0] = 300;
				out[1] = 250;
				out[2] = 100;
				break;
			case TRACK_BOWLING:
				out[0] = 20;
				out[1] = 15;
				out[2] = 10;
				break;
			default:
				break;
		}
	}

	void OnCupFinished() {
		int playerPosition = gCustomSave.aCupPlayerPosition[0]+1;
		auto cup = GetCurrentCup();
		int targets[3] = {0,0,0};
		GetStuntTargets(cup->aRaces[0].nLevel, targets);
		if (targets[0]) {
			playerPosition = 4;
			if (aPlayerResults[0].nFinishTime >= targets[2]) playerPosition = 3;
			if (aPlayerResults[0].nFinishTime >= targets[1]) playerPosition = 2;
			if (aPlayerResults[0].nFinishTime >= targets[0]) playerPosition = 1;
		}

		if (auto cup = GetCurrentSaveCup()) {
			if (!cup->nPosition || cup->nPosition > playerPosition) {
				cup->nPosition = playerPosition;
			}

			if (gCustomSave.aCareerCupPlayers[0].points == GetCurrentCup()->aRaces.size() * 10) cup->bWonAll = true;

			// unlock the next cup if finished 3rd or higher
			if (cup->nPosition >= 1 && cup->nPosition <= 3) {
				for (auto& car : GetCurrentCup()->aCarUnlocks) {
					if (!gCustomSave.aCareerGarage[car].bIsUnlocked) {
						aNewlyUnlockedCars.push_back(car);
						gCustomSave.aCareerGarage[car].bIsUnlocked = true;
					}
				}

				auto nextCup = cup + 1;
				nextCup->bUnlocked = true;

				// unlock associated event with this cup
				if (auto event = GetCurrentSaveCupAssociatedEvent()) {
					event->bUnlocked = true;
				}

				// unlock next class after finals are done
				if (gCustomSave.nCareerCup == 64 && gCustomSave.nCareerClass < 4) {
					bool stockPep = true;
					for (int i = 0; i < GetCurrentCup()->aRaces.size(); i++) {
						if (!gCustomSave.aCareerCupPlayers[0].IsStockPep(i)) stockPep = false;
					}
					if (cup->nPosition == 1 && stockPep) {
						Achievements::AwardAchievement(GetAchievement("WIN_CUP_PEPPER"));
					}
					//if (!gCustomSave.bCareerClassUnlocked[gCustomSave.nCareerClass]) {
					//	nNewlyUnlockedClass = gCustomSave.nCareerClass+1;
					//}
					gCustomSave.bCareerClassUnlocked[gCustomSave.nCareerClass] = true;
				}
			}
		}
		else if (auto cup = GetCurrentSaveEvent()) {
			if (!cup->nPosition || cup->nPosition > playerPosition) {
				cup->nPosition = playerPosition;
				cup->nTimeOrScore = aPlayerResults[0].nFinishTime;
			}
			else if (!cup->nTimeOrScore || aPlayerResults[0].nFinishTime > cup->nTimeOrScore) {
				cup->nTimeOrScore = aPlayerResults[0].nFinishTime;
			}
		}

		sLastCupName = cup->sName;
		nLastCupNumRaces = cup->aRaces.size();
		nLastCupAward = 0;
		if (nLastCupNumRaces > 1) {
			bWasCareerCupJustFinished = true;
		}

		if (playerPosition >= 1 && playerPosition <= 3) {
			nLastCupAward = cup->aCupWinnings[playerPosition-1];
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
		// don't proceed if we quit the race
		if (!aPlayerResults[0].bDNF && !aPlayerResults[0].bFinished) return;

		int eventNumber = gCustomSave.nCareerCupNextEvent++;
		if (pGameFlow->nGameMode == eGameMode::SPLITSCREEN) {
			// add up and average all players in splitscreen career
			auto player = &gCustomSave.aCareerCupPlayers[0];
			for (int i = 0; i < pGameFlow->nNumSplitScreenPlayers; i++) {
				auto results = &aPlayerResults[i];
				player->eventPosition[eventNumber] += results->nPosition;
				if (pGameFlow->nEventType == eEventType::RACE && results->bDNF) {
					player->eventPosition[eventNumber] += 8;
				}
				else {
					player->eventPoints[eventNumber] += aPointsPerPosition[results->nPosition-1];
				}
			}
			// average out the results, this might end up weird
			player->eventPosition[eventNumber] /= pGameFlow->nNumSplitScreenPlayers;
			player->eventPoints[eventNumber] /= pGameFlow->nNumSplitScreenPlayers;
			player->points += player->eventPoints[eventNumber];

			// calculate the rest as normal AI
			for (int i = pGameFlow->nNumSplitScreenPlayers; i < nNumCareerMaxPlayers; i++) {
				auto results = &aPlayerResults[i];
				auto player = &gCustomSave.aCareerCupPlayers[i];
				player->eventPosition[eventNumber] = results->nPosition;
				if (pGameFlow->nEventType == eEventType::RACE && results->bDNF) {
					player->eventPosition[eventNumber] = 8;
					player->eventPoints[eventNumber] = 0;
				}
				else {
					player->eventPoints[eventNumber] = aPointsPerPosition[results->nPosition-1];
				}
				player->points += player->eventPoints[eventNumber];
			}
		}
		else for (int i = 0; i < nNumCareerMaxPlayers; i++) {
			auto results = &aPlayerResults[i];
			auto player = &gCustomSave.aCareerCupPlayers[i];
			player->eventPosition[eventNumber] = results->nPosition;
			if (pGameFlow->nEventType == eEventType::RACE && results->bDNF) {
				player->eventPosition[eventNumber] = 8;
				player->eventPoints[eventNumber] = 0;
			}
			else {
				player->eventPoints[eventNumber] = aPointsPerPosition[results->nPosition-1];
			}
			player->points += player->eventPoints[eventNumber];
		}
		gCustomSave.aCareerCupPlayers[0].SetStockPep(eventNumber, pGameFlow->Profile.nCarType + 1 == CAR_PEPPER && !gCustomSave.aCareerGarage[CAR_PEPPER].IsAnyUpgradePurchased());
		gCustomSave.CalculateCupPlayersByPosition();
		OnRaceFinished();
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

	bool IsCareerTimeTrial() {
		if (bIsCareerRace && GetCurrentRace() && GetCurrentRace()->bIsTimeTrial) {
			return true;
		}
		return false;
	}

	void OnTick() {
		if (IsCareerTimeTrial()) {
			if (auto ply = GetPlayer(0)) {
				ply->nStartPosition = 1;
			}
		}

		if (pLoadingScreen) return;
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
				if (pGameFlow->nEventType == eEventType::RACE) {
					memcpy(results->aSceneryBonuses, player->pCar->aObjectsSmashed, sizeof(results->aSceneryBonuses));
					memcpy(results->aCrashBonuses, aCrashBonusesReceived[i], sizeof(results->aCrashBonuses));
				}
				else {
					memset(results->aSceneryBonuses, 0, sizeof(results->aSceneryBonuses));
					memset(results->aCrashBonuses, 0, sizeof(results->aCrashBonuses)); // no crash bonuses for derbies, ruins the economy
				}

				if (pGameFlow->nSubEventType == eSubEventType::STUNT_LONGJUMP || pGameFlow->nSubEventType == eSubEventType::STUNT_HIGHJUMP) {
					results->nFinishTime = ((score->nStuntMetersScore[0] + score->nStuntMetersScore[1] + score->nStuntMetersScore[2]) * 0.01);
				}
				if (pGameFlow->nSubEventType == eSubEventType::STUNT_BOWLING) {
					results->nFinishTime = score->nStuntPointsScore[0] + score->nStuntPointsScore[1] + score->nStuntPointsScore[2];
				}
			}
		}
	}

	int __stdcall GetAIHandicapLevelNew(GameFlow* gameFlow) {
		if (bIsArcadeMode) {
			int handicap = ArcadeMode::pCurrentEvent->nAIHandicapLevel;
			if (handicap >= 1) return handicap;
		}
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

	float GetAIUpgradeLevel() {
		if (IsCareerTimeTrial()) return 0.0;

		if (bIsInMultiplayer) {
			return fMultiplayerUpgradeLevel;
		}
		if (bIsArcadeMode) {
			return ArcadeMode::pCurrentEvent->fAIUpgradeLevel;
		}
		if (bIsQuickRace) {
			return QuickRace::fUpgradeLevel;
		}
		if (bIsInstantAction) {
			return InstantAction::fUpgradeLevel;
		}
		if (bIsCareerRace) {
			return GetCurrentCup()->fAIUpgradeLevel;
		}
		return 0.0;

		int level = pGameFlow->nLevel;
		if (DoesTrackValueExist(level, "AIUpgradeLevel")) {
			auto upgrade = GetTrackValueNumber(level, "AIUpgradeLevel") / 6.0;
			if (upgrade > 1.0) return 1.0;
			return upgrade;
		}
		return 0.0;
	}

	float GetPlayerUpgradeLevel() {
		if (IsCareerTimeTrial()) return 0.0;

		if (bIsInMultiplayer) {
			return fMultiplayerUpgradeLevel;
		}
		if (bIsArcadeMode) {
			return ArcadeMode::pCurrentEvent->fUpgradeLevel;
		}
		return GetAIUpgradeLevel();
	}

	int nForceNumLaps = -1;

	int __stdcall GetNumLapsNew(GameFlow* gameFlow) {
		if (bIsArcadeMode) {
			return ArcadeMode::pCurrentEvent->nLaps;
		}
		if (bIsQuickRace) {
			return QuickRace::nNumLaps;
		}
		if (bIsInstantAction) {
			return InstantAction::nNumLaps;
		}
		if (bIsCareerRace) {
			return GetCurrentRace()->nLaps;
		}
		if (nForceNumLaps > 0) {
			auto value = nForceNumLaps;
			nForceNumLaps = -1;
			return value;
		}

		int level = gameFlow->nLevel;
		if (DoesTrackValueExist(level, "Laps")) {
			return GetTrackValueNumber(level, "Laps");
		}
		return 4;
	}

	void OnSave(int saveSlot) {
		float cupsCompleted = 0;
		int cupsCompletedCount = 0;
		int cupsCompletedAllWon = 0;
		int cupsTotal = 0;
		float eventsCompleted = 0;
		int eventsCompletedCount = 0;
		int eventsTotal = 0;
		for (int classId = 0; classId < 3; classId++) {
			auto careerClass = &aLUACareerClasses[classId];
			if (careerClass->aCups.empty()) continue;

			cupsTotal += careerClass->aCups.size()+1;
			eventsTotal += careerClass->aEvents.size();

			for (int i = 0; i < careerClass->aCups.size(); i++) {
				auto cup = &gCustomSave.aCareerClasses[classId].aCups[i];
				if (cup->nPosition >= 1 && cup->nPosition <= 3) cupsCompletedCount++;
				if (cup->bWonAll) cupsCompletedAllWon++;

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
		if (auto achievement = GetAchievement("COMPLETE_CAREER_GOLD_WIN")) {
			achievement->fInternalProgress = cupsCompletedAllWon;
			achievement->fMaxInternalProgress = cupsTotal;
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
			if (car.carId >= 200 && car.carId < 300) {
				gCustomSave.nCarsUnlocked++;
				continue;
			}

			if (gCustomSave.aCareerGarage[car.carId].bIsUnlocked) gCustomSave.nCarsUnlocked++;
		}
	}

	void OnLoad(int saveSlot) {
		// retroactively unlock cars based on cups completed
		for (int classId = 0; classId < 3; classId++) {
			auto careerClass = &aLUACareerClasses[classId];
			if (careerClass->aCups.empty()) continue;

			for (int cupId = 0; cupId < careerClass->aCups.size(); cupId++) {
				auto& cup = gCustomSave.aCareerClasses[classId].aCups[cupId];
				if (cup.nPosition && cup.nPosition <= 3) {
					for (auto& car : careerClass->aCups[cupId].aCarUnlocks) {
						gCustomSave.aCareerGarage[car].bIsUnlocked = true;
					}
				}
			}

			auto& cup = gCustomSave.aCareerClasses[classId].Finals;
			if (cup.nPosition && cup.nPosition <= 3) {
				for (auto& car : careerClass->Finals.aCarUnlocks) {
					gCustomSave.aCareerGarage[car].bIsUnlocked = true;
				}
			}
		}
	}

	uint32_t nNumSplitscreenCars = 2;
	uintptr_t GetSplitscreenAICountASM_jmp = 0x43F0CC;
	void __attribute__((naked)) GetSplitscreenAICountASM() {
		__asm__ (
			"mov edi, %1\n"
			"mov [esp+0x1C], edi\n"
			"mov edi, [ebp+0x12C]\n"
			"mov [esp+0x14], edi\n"
			"mov [ebp+0x28F4], edx\n\t"
			"jmp %0\n\t"
				:
				:  "m" (GetSplitscreenAICountASM_jmp), "m" (nNumSplitscreenCars)
		);
	}

	std::string OnTrack_CompleteCareerGoldWin(Achievements::CAchievement* pThis) {
		std::string str;
		for (int classId = 0; classId < 3; classId++) {
			auto careerClass = &CareerMode::aLUACareerClasses[classId];
			if (careerClass->aCups.empty()) continue;

			for (int i = 0; i < careerClass->aCups.size(); i++) {
				auto cup = &gCustomSave.aCareerClasses[classId].aCups[i];
				if (!cup->bWonAll) {
					if (!str.empty()) str += ", ";
					str += careerClass->aCups[i].sName;
				}
			}
		}
		return "Remaining: " + str;
	}

	void Init() {
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x440560, &GetAIHandicapLevelNew);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4404A0, &GetNumLapsNew);

		// splitscreen career stuff
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43F0B8, &GetSplitscreenAICountASM);

		ChloeEvents::SaveCreatedEvent.AddHandler(OnSave);
		ChloeEvents::SaveLoadedEvent.AddHandler(OnLoad);
		ChloeEvents::FinishFrameEvent.AddHandler(OnTick);

		Achievements::GetAchievement("COMPLETE_CAREER_GOLD_WIN")->pTrackFunction = OnTrack_CompleteCareerGoldWin;
	}
}