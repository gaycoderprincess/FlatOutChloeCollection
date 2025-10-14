namespace ArcadeMode {
	bool bAllUnlocked = false;

	void PlayTimeTick() {
		// load the audio file multiple times so it won't cancel the previous tick to play the next tick
		static int counter = 0;
		static NyaAudio::NyaSound aSounds[16] = {};
		if (!aSounds[0]) {
			for (int i = 0; i < 16; i++) {
				aSounds[i] = NyaAudio::LoadFile("data/sound/time_bleep.wav");
			}
		}
		auto sound = aSounds[counter++];
		if (counter >= 16) counter = 0;

		if (sound) {
			NyaAudio::SetVolume(sound, nIngameSfxVolume / 100.0);
			NyaAudio::Play(sound);
		}
	}
	void ProcessTimerTick(int32_t timeLeft) {
		if (timeLeft < 0) return;
		if (!bIsFragDerby && IsPlayerWrecked(GetPlayer(0))) return;

		static CNyaTimer gTimer;
		gTimer.Process();

		static int32_t lastTimeLeft = 0;
		if (timeLeft == lastTimeLeft) return;
		lastTimeLeft = timeLeft;

		if (timeLeft > 5000) {
			gTimer.fTotalTime = 0.5;
			return;
		}

		double frequency = 1.0;
		if (timeLeft <= 5000) {
			frequency = 0.5;
		}
		if (timeLeft <= 3000) {
			frequency = 0.25;
		}
		if (timeLeft <= 1000) {
			frequency = 0.1;
		}
		if (gTimer.fTotalTime >= frequency) {
			PlayTimeTick();
			while (gTimer.fTotalTime >= frequency) {
				gTimer.fTotalTime -= frequency;
			}
		}
	}

	bool bLastRaceCompleted = false;

	// todo car unlocks?
	struct tLUARace {
		std::string sName;
		int nLevel;
		int nLaps;
		int nCar;
		bool bTrackReversed : 1;
		bool bIsArcadeRace : 1;
		bool bIsSmashySmash : 1;
		bool bIsFragDerby : 1;
		int nPointsToUnlock;
		float fUpgradeLevel;
		float fAIUpgradeLevel;
		int nAIHandicapLevel;
		int aGoalScores[3];
		int nPlatinumScore;
		int nCommunityScore;
		std::string sCommunityScoreName;
	};
	std::vector<tLUARace> aArcadeRaces;

	tLUARace* pCurrentEvent = nullptr;
	int nCurrentEventId = 0;
	uint32_t nCurrentEventScore = 0;

	tLUARace* luaDefs_currentRace = nullptr;

	void SetIsArcadeMode(bool apply) {
		bIsArcadeMode = apply;
		NyaHookLib::Patch<uint8_t>(0x45B0F2, apply ? 0xEB : 0x75); // disable vanilla game tutorials
	}

	void ProcessResultsFromLastRace() {
		// don't proceed if we quit the race
		if (!bLastRaceCompleted) {
			nCurrentEventScore = 0;
			return;
		}

		if (nCurrentEventScore > gCustomSave.aArcadeCareerScores[nCurrentEventId]) {
			gCustomSave.aArcadeCareerScores[nCurrentEventId] = nCurrentEventScore;
			gCustomSave.Save();
		}
		nCurrentEventScore = 0;
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
			auto ply = GetPlayerScore(1);
			bLastRaceCompleted = ply->bHasFinished || ply->bIsDNF;
		}
	}

	void OnSave(int saveSlot) {
		float eventsCompleted = 0;
		int eventsCompletedCount = 0;
		int eventsCompletedAuthor = 0;
		int eventsTotal = 0;
		for (auto& event : aArcadeRaces) {
			int points = gCustomSave.aArcadeCareerScores[eventsTotal];
			if (points >= event.aGoalScores[0]) {
				eventsCompleted += 1;
				eventsCompletedCount++;
				if (points >= event.nPlatinumScore) {
					eventsCompletedAuthor++;
				}
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
		if (auto achievement = GetAchievement("COMPLETE_CARNAGE_AUTHOR")) {
			achievement->fInternalProgress = eventsCompletedAuthor;
			achievement->fMaxInternalProgress = eventsTotal;
		}
	}

	ChloeHook Init([]() {
		ChloeEvents::SaveCreatedEvent.AddHandler(OnSave);
		ChloeEvents::FinishFrameEvent.AddHandler(OnTick);
	});
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

	// hacks for changed event ids
	if (car == CAR_FO2SWITCHBLADE && level == TRACK_FO2FOREST2B) level = TRACK_FO2FOREST2C;
	if (car == CAR_RETROSLIDER && level == TRACK_RETRODEMO1B) level = TRACK_RETRODEMO2A;

	for (int i = 0; i < ArcadeMode::aArcadeRaces.size(); i++) {
		auto race = &ArcadeMode::aArcadeRaces[i];
		if (race->nLevel != level) continue;
		if (race->nCar != car) continue;
		aArcadeCareerScores[i] = score;
		return;
	}
	WriteLog(std::format("Failed to write arcade score for {} on {} ({}pts), event possibly missing", GetCarName(car), GetTrackName(level), score));
}