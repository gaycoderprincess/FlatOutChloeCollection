namespace ArcadeMode {
	bool bIsArcadeMode = false;

	bool bLastRaceCompleted = false;

	// todo car unlocks?
	struct tLUARace {
		std::string sName;
		int nLevel;
		int nLaps;
		int nCar;
		bool bIsArcadeRace : 1;
		bool bIsSmashySmash : 1;
		int nPointsToUnlock;
		float fUpgradeLevel;
		float fAIUpgradeLevel;
		int nAIHandicapLevel;
		int aGoalScores[3];
		int nPlatinumScore;
	};
	std::vector<tLUARace> aArcadeRaces;

	tLUARace* pCurrentEvent = nullptr;
	int nCurrentEventId = 0;
	uint32_t nCurrentEventScore = 0;

	tLUARace* luaDefs_currentRace = nullptr;

	void SetIsArcadeMode(bool apply) {
		bIsArcadeMode = apply;
	}

	void ProcessResultsFromLastRace() {
		// don't proceed if we quit the race
		if (!bLastRaceCompleted) return;

		if (pCurrentEvent->bIsArcadeRace || pCurrentEvent->bIsSmashySmash)  {
			if (nCurrentEventScore > gCustomSave.aArcadeCareerScores[nCurrentEventId]) {
				gCustomSave.aArcadeCareerScores[nCurrentEventId] = nCurrentEventScore;
				gCustomSave.Save();
			}
		}
	}

	void ProcessResultsFromLastRace_Prompted() {
		if (GetGameState() == GAME_STATE_MENU) {
			ProcessResultsFromLastRace();
		}
	}

	void OnTick() {
		if (pLoadingScreen) return;
		if (!bIsArcadeMode) return;

		if (GetGameState() == GAME_STATE_RACE) {
			auto ply = GetPlayerScore<PlayerScoreRace>(1);
			bLastRaceCompleted = ply->bHasFinished || ply->bIsDNF;
		}
	}

	void OnSave() {
		float eventsCompleted = 0;
		int eventsCompletedCount = 0;
		int eventsTotal = 0;
		for (auto& event : aArcadeRaces) {
			int points = gCustomSave.aArcadeCareerScores[eventsTotal];
			if (points >= event.aGoalScores[0]) {
				eventsCompleted += 1;
				eventsCompletedCount++;
			}
			else if (points >= event.aGoalScores[1]) {
				eventsCompleted += 0.5;
				eventsCompletedCount++;
			}
			else if (points >= event.aGoalScores[2]) {
				eventsCompleted += 0.25;
				eventsCompletedCount++;
			}
			eventsTotal++;
		}

		if (auto achievement = GetAchievement("COMPLETE_CARNAGE")) {
			achievement->fInternalProgress = eventsCompletedCount;
			achievement->fMaxInternalProgress = eventsTotal;
		}
		if (auto achievement = GetAchievement("COMPLETE_CARNAGE_GOLD")) {
			achievement->fInternalProgress = eventsCompleted;
			achievement->fMaxInternalProgress = eventsTotal;
		}
	}
}

void tCustomSaveStructure::CreateArcadeVerify() {
	memset(aArcadeRaceVerify, 0, sizeof(aArcadeRaceVerify));
	for (int i = 0; i < ArcadeMode::aArcadeRaces.size(); i++) {
		auto verify = &aArcadeRaceVerify[i];
		auto race = &ArcadeMode::aArcadeRaces[i];
		verify->car = race->nCar;
		verify->level = race->nLevel;
	}
}
void tCustomSaveStructure::WriteArcadeScore(int car, int level, int score) {
	if (score <= 0) return;

	for (int i = 0; i < ArcadeMode::aArcadeRaces.size(); i++) {
		auto race = &ArcadeMode::aArcadeRaces[i];
		if (race->nLevel != level) continue;
		if (race->nCar != car) continue;
		aArcadeCareerScores[i] = score;
		return;
	}
	WriteLog(std::format("Failed to write arcade score for {} on {} ({}pts), event possibly missing", GetCarName(car), GetTrackName(level), score));
}