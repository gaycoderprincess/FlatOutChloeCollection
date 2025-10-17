int nCurrentSaveSlot = -1;

enum ePlaytimeType {
	PLAYTIME_TOTAL,
	PLAYTIME_MENU,
	PLAYTIME_INGAME,
	PLAYTIME_INGAME_SINGLEPLAYER,
	PLAYTIME_INGAME_MULTIPLAYER,
	PLAYTIME_INGAME_SPLITSCREEN,
	PLAYTIME_INGAME_HOTSEAT,
	PLAYTIME_INGAME_CAREER,
	PLAYTIME_INGAME_SPLITSCREEN_CAREER,
	PLAYTIME_INGAME_CARNAGE,
	PLAYTIME_INGAME_RALLYMODE,
	PLAYTIME_INGAME_SINGLE,
	PLAYTIME_INGAME_ALLRACE,
	PLAYTIME_INGAME_RACE,
	PLAYTIME_INGAME_PONGRACE,
	PLAYTIME_INGAME_ARCADERACE,
	PLAYTIME_INGAME_BEATTHEBOMB,
	PLAYTIME_INGAME_LAPKNOCKOUT,
	PLAYTIME_INGAME_SPEEDTRAP,
	PLAYTIME_INGAME_TIMETRIAL,
	PLAYTIME_INGAME_ALLDERBY,
	PLAYTIME_INGAME_WRECKINGDERBY,
	PLAYTIME_INGAME_LMSDERBY,
	PLAYTIME_INGAME_FRAGDERBY,
	PLAYTIME_INGAME_STUNT,
	PLAYTIME_INGAME_STUNTSHOW,
	PLAYTIME_INGAME_SMASHYRACE,
	PLAYTIME_INGAME_DRIFT,
	NUM_PLAYTIME_TYPES,
};

const char* aPlaytimeTypeNames[] = {
	"Total",
	"Menus",
	"In-game",
	"Singleplayer",
	"Multiplayer",
	"Split Screen",
	"Hot Seat",
	"Career",
	"Split Screen Career",
	"Arcade Mode",
	"Rally Mode",
	"Single Events",
	"All Races",
	"Race",
	"Head-On Race",
	"Carnage Race",
	"Beat the Bomb",
	"Knockout",
	"Speedtrap",
	"Time Trial",
	"All Derbies",
	"Wrecking Derby",
	"Survivor Derby",
	"Frag Derby",
	"All Stunts",
	"Stunt Show",
	"Demolition",
	"Drift",
};

struct tCustomSaveStructure {
	struct {
		uint32_t bIsUnlocked : 1;
		uint32_t bIsPurchased : 1;
		uint8_t nSkinId;
		uint8_t nUpgrades[19];
		uint32_t nUpgradesValue;

		bool IsUpgradePurchased(int upgrade) {
			uint32_t bit = 1 << (upgrade % 8);
			return (nUpgrades[upgrade / 8] & bit) != 0;
		}
		void SetUpgradePurchased(int upgrade, bool purchased) {
			uint32_t bit = 1 << (upgrade % 8);
			if (purchased) {
				nUpgrades[upgrade / 8] |= bit;
			}
			else {
				nUpgrades[upgrade / 8] &= ~bit;
			}
		}
		bool IsAnyUpgradePurchased() {
			for (int i = 0; i < 19; i++) {
				if (nUpgrades[i]) return true;
			}
			return false;
		}
		void Clear() {
			memset(this,0,sizeof(*this));
		}
	} aCareerGarage[nMaxCars];
	bool tracksWon[nMaxTracks];
	struct tCareerClass {
		struct tCareerCup {
			bool bUnlocked : 1;
			uint8_t nPosition : 4;
			bool bWonAll : 1;
		} aCups[nNumCareerEvents];
		struct tCareerEvent {
			bool bUnlocked : 1;
			uint8_t nPosition : 4;
			uint32_t nTimeOrScore;
		} aEvents[nNumCareerEvents];
		tCareerCup Finals;
	} aCareerClasses[nNumCareerClasses];
	int8_t nCareerClass;
	bool bCareerClassUnlocked[nNumCareerClasses];
	int8_t nCareerCup;
	uint8_t nCareerCupNextEvent;
	int8_t nCareerEvent;
	struct tCareerCupPlayer {
		uint16_t points;
		uint8_t eventStockPep[2];
		int eventPosition[nNumCareerEventsPerCup];
		int eventPoints[nNumCareerEventsPerCup];

		bool IsStockPep(int eventId) {
			uint32_t bit = 1 << (eventId % 8);
			return (eventStockPep[eventId / 8] & bit) != 0;
		}
		void SetStockPep(int eventId, bool set) {
			uint32_t bit = 1 << (eventId % 8);
			if (set) {
				eventStockPep[eventId / 8] |= bit;
			}
			else {
				eventStockPep[eventId / 8] &= ~bit;
			}
		}
	} aCareerCupPlayers[nNumCareerMaxPlayers];
	int nGameCompletion;
	int nCupsPassed;
	int nCupsMax;
	int nCarsUnlocked;
	uint32_t bestLaps[nMaxTracks];
	uint32_t bestLapCars[nMaxTracks];
	uint32_t aArcadeCareerScores[nNumArcadeRaces];
	struct {
		uint16_t car;
		uint16_t level;
	} aArcadeRaceVerify[nNumArcadeRaces];
	uint32_t bestLapsReversed[nMaxTracks];
	uint32_t bestLapCarsReversed[nMaxTracks];
	uint64_t playtimeNew[NUM_PLAYTIME_TYPES];
	uint8_t handlingDamage;
	uint8_t handlingMode;
	uint8_t highCarCam;
	uint8_t playerFlag;
	uint32_t trackArcadeScores[nMaxTracks];
	uint8_t transmission;
	uint8_t playerColor;

	static inline bool bInitialized = false;
	static inline uint8_t aCupPlayersByPosition[nNumCareerMaxPlayers];
	static inline uint8_t aCupPlayerPosition[nNumCareerMaxPlayers];
	static inline uint8_t aCupLocalPlayerPlacements[nNumCareerMaxPlayers];
	static inline std::mutex sSaveMutex;

	void CalculateCupPlayersByPosition() {
		memset(aCupLocalPlayerPlacements,0,sizeof(aCupLocalPlayerPlacements));
		for (auto& pos : aCareerCupPlayers[0].eventPosition) {
			aCupLocalPlayerPlacements[pos]++;
		}

		struct tLeaderboardEntry {
			int playerId;
			int score;

			static bool compFunction(tLeaderboardEntry a, tLeaderboardEntry b) {
				if (a.score == b.score) return a.playerId < b.playerId;
				return a.score > b.score;
			}
		};
		std::vector<tLeaderboardEntry> players;

		for (int i = 0; i < nNumCareerMaxPlayers; i++) {
			// never display extra players for now as it'll crash the game til more ai are added, todo change this after
			players.push_back({i, i < 8 ? aCareerCupPlayers[i].points : -1});
		}
		sort(players.begin(), players.end(), tLeaderboardEntry::compFunction);
		for (int i = 0; i < nNumCareerMaxPlayers; i++) {
			aCupPlayersByPosition[i] = players[i].playerId;
			aCupPlayerPosition[players[i].playerId] = i;
		}
	}

	uint32_t GetArcadeCareerScore() {
		uint32_t score = 0;
		for (int i = 0; i < nNumArcadeRaces; i++) {
			score += aArcadeCareerScores[i];
		}
		return score;
	}
	void CreateArcadeVerify();
	void WriteArcadeScore(int car, int level, int score);
	void CheckArcadeVerify() {
		uint32_t data[nNumArcadeRaces];
		memcpy(data, aArcadeCareerScores, sizeof(data));
		memset(aArcadeCareerScores, 0, sizeof(aArcadeCareerScores)); // null all old data so random events don't get overridden

		bool hasData = false;
		for (int i = 0; i < nNumArcadeRaces; i++) {
			if (aArcadeRaceVerify[i].car != 0) hasData = true;
			if (aArcadeRaceVerify[i].level != 0) hasData = true;
		}
		if (!hasData) return;

		// write all saved events into any found equivalent in the current version's event list
		for (int i = 0; i < nNumArcadeRaces; i++) {
			auto verify = &aArcadeRaceVerify[i];
			WriteArcadeScore(verify->car, verify->level, data[i]);
		}

		// update verification data afterwards
		CreateArcadeVerify();
	}

	tCustomSaveStructure() {
		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();
	}
	void SetDefaultPlayerSettings() {
		bCareerClassUnlocked[0] = true;
		for (auto& data : aCareerClasses) {
			data.aCups[0].bUnlocked = true;
		}
		aCareerGarage[CAR_PEPPER].bIsUnlocked = true;
		aCareerGarage[CAR_BLOCKER].bIsUnlocked = true;
		aCareerGarage[CAR_RETROBEETLE].bIsUnlocked = true;
		handlingDamage = HANDLINGDAMAGE_REDUCED;
		handlingMode = HANDLING_NORMAL;
	}
	void ApplyPlayerSettings() const {
		nHandlingDamage = handlingDamage;
		nHandlingMode = handlingMode;
		nHighCarCam = highCarCam;
		nPlayerFlag = playerFlag;
		nTransmission = transmission;
		nPlayerColor = playerColor;
	}
	void ReadPlayerSettings() {
		handlingDamage = nHandlingDamage;
		handlingMode = nHandlingMode;
		highCarCam = nHighCarCam;
		playerFlag = nPlayerFlag;
		transmission = nTransmission;
		playerColor = nPlayerColor;
	}
	void Clear() {
		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();
		ApplyPlayerSettings();
		ChloeEvents::SaveClearedEvent.OnHit();
	}
	void Load(int saveSlot = -1, bool loadAll = true) {
		if (saveSlot < 0) {
			saveSlot = nCurrentSaveSlot;
			if (saveSlot < 0) {
				MessageBoxA(0, "Trying to save to slot 0, this is a bug", "nya?!~", MB_ICONWARNING);
			}
		}

		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();

		auto file = std::ifstream(GetCustomSavePath(saveSlot+1), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		file.read((char*)this, sizeof(*this));
		file.close();

		CheckArcadeVerify();

		if (loadAll) {
			ApplyPlayerSettings();
			ChloeEvents::SaveLoadedEvent.OnHit(saveSlot+1);
		}
	}
	void Save(bool saveAll = true) {
		sSaveMutex.lock();

		if (!bInitialized) {
			sSaveMutex.unlock();
			return;
		}

		int saveSlot = nCurrentSaveSlot;
		if (saveSlot < 0) {
			MessageBoxA(0, "Trying to save to slot 0, this is a bug", "nya?!~", MB_ICONWARNING);
		}

		ReadPlayerSettings();
		CreateArcadeVerify();

		auto file = std::ofstream(GetCustomSavePath(saveSlot+1), std::ios::out | std::ios::binary);
		if (!file.is_open()) {
			sSaveMutex.unlock();
			return;
		}

		file.write((char*)this, sizeof(*this));
		file.close();

		if (saveAll) ChloeEvents::SaveCreatedEvent.OnHit(saveSlot+1);

		sSaveMutex.unlock();
	}
	void Delete(int slot) {
		auto save = GetCustomSavePath(slot+1);
		if (std::filesystem::exists(save)) {
			std::filesystem::remove(save);
		}
		ChloeEvents::SaveDeletedEvent.OnHit(slot+1);
	}
} gCustomSave;

void ProcessPlayStats() {
	static CNyaTimer gTimer;
	gTimer.Process();

	if (pLoadingScreen) return;

	if (GetGameState() == GAME_STATE_RACE && pPlayerHost->GetNumPlayers() > 1) {
		bool changed = false;
		int track = pGameFlow->nLevel;

		if (!IsInSplitScreen() && !bIsTimeTrial && !bIsArcadeMode && !bIsCarnageRace) {
			auto ply = GetPlayerScore(1);
			if (ply->bHasFinished || ((bIsWreckingDerby || bIsFragDerby) && ply->bIsDNF)) {
				bool won = ply->nPosition == 1;
				if (bIsWreckingDerby || bIsFragDerby) won = GetSortedPlayerScores()[0] == ply;
				if (won && !gCustomSave.tracksWon[track]) {
					gCustomSave.tracksWon[track] = true;
					changed = true;
				}
			}
		}

		if (changed) {
			gCustomSave.Save();
		}
	}

	if (GetGameState() == GAME_STATE_RACE) {
		bool changed = false;
		int track = pGameFlow->nLevel;

		if (pGameFlow->nEventType == eEventType::RACE) {
			for (int j = 0; j < pPlayerHost->GetNumPlayers(); j++) {
				auto ply = GetPlayer(j);
				if (ply->nPlayerType != PLAYERTYPE_LOCAL) continue;

				auto bestLaps = bIsTrackReversed ? gCustomSave.bestLapsReversed : gCustomSave.bestLaps;
				auto bestLapCars = bIsTrackReversed ? gCustomSave.bestLapCarsReversed : gCustomSave.bestLapCars;

				for (int i = 0; i < ply->nCurrentLap; i++) {
					auto lap = GetPlayerLapTime(ply, i);
					if (lap <= 0) continue;

					if (!bestLaps[track] || lap < bestLaps[track]) {
						bestLaps[track] = lap;
						bestLapCars[track] = ply->nCarId;
						ChloeEvents::NewLapRecordEvent.OnHit(ply, lap);
						changed = true;
					}
				}
			}
		}

		if (changed) {
			std::thread([]() { gCustomSave.Save(false); }).detach();
		}
	}

	if (gTimer.fTotalTime > 1) {
		if (GetGameState() == GAME_STATE_RACE) {
			gCustomSave.playtimeNew[PLAYTIME_INGAME]++;

			auto gameMode = pGameFlow->nGameMode;
			if (bIsInMultiplayer) gameMode = eGameMode::JOIN;
			switch (gameMode) {
				case eGameMode::SINGLEPLAYER:
					gCustomSave.playtimeNew[PLAYTIME_INGAME_SINGLEPLAYER]++;
					break;
				case eGameMode::HOST:
				case eGameMode::JOIN:
					gCustomSave.playtimeNew[PLAYTIME_INGAME_MULTIPLAYER]++;
					break;
				case eGameMode::SPLITSCREEN:
					gCustomSave.playtimeNew[pGameFlow->nEventType == eEventType::STUNT ? PLAYTIME_INGAME_HOTSEAT : PLAYTIME_INGAME_SPLITSCREEN]++;
					break;
				default:
					break;
			}

			if (bIsCareerRace) {
				gCustomSave.playtimeNew[IsInSplitScreen() ? PLAYTIME_INGAME_SPLITSCREEN_CAREER : PLAYTIME_INGAME_CAREER]++;
			}
			else if (bIsArcadeMode) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_CARNAGE]++;
			}
			else if (bIsQuickRace || bIsInstantAction) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_SINGLE]++;
			}

			if (pGameFlow->nEventType == eEventType::DERBY) {
				if (bIsFragDerby) {
					gCustomSave.playtimeNew[PLAYTIME_INGAME_FRAGDERBY]++;
				}
				else if (bIsWreckingDerby) {
					gCustomSave.playtimeNew[PLAYTIME_INGAME_WRECKINGDERBY]++;
				}
				else {
					gCustomSave.playtimeNew[PLAYTIME_INGAME_LMSDERBY]++;
				}
				gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLDERBY]++;
			}
			else {
				if (pGameFlow->nEventType == eEventType::STUNT) {
					gCustomSave.playtimeNew[PLAYTIME_INGAME_STUNT]++;
				}
				else if (bIsTimeTrial) {
					gCustomSave.playtimeNew[PLAYTIME_INGAME_TIMETRIAL]++;
					gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
				}
				else if (bIsSmashyRace) {
					gCustomSave.playtimeNew[PLAYTIME_INGAME_SMASHYRACE]++;
				}
				else if (bIsCarnageRace) {
					gCustomSave.playtimeNew[PLAYTIME_INGAME_ARCADERACE]++;
					gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
				}
				else {
					gCustomSave.playtimeNew[PLAYTIME_INGAME_RACE]++;
					gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
				}
			}
		}
		else if (GetGameState() == GAME_STATE_MENU) {
			gCustomSave.playtimeNew[PLAYTIME_MENU]++;
		}
		gCustomSave.playtimeNew[PLAYTIME_TOTAL] = gCustomSave.playtimeNew[PLAYTIME_MENU] + gCustomSave.playtimeNew[PLAYTIME_INGAME];

		gTimer.fTotalTime -= 1;
	}

	static auto lastGameState = GetGameState();
	if ((lastGameState == GAME_STATE_RACE || lastGameState == GAME_STATE_REPLAY) && GetGameState() == GAME_STATE_MENU) {
		gCustomSave.Save();
	}
	lastGameState = GetGameState();
}

ChloeHook Hook_PlayStats([]() {
	ChloeEvents::FinishFrameEvent.AddHandler(ProcessPlayStats);
});