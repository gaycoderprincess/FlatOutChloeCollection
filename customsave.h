void DoGameSave();

const int nNumCareerClasses = 4;
const int nNumCareerEvents = 16;
const int nNumCareerEventsPerCup = 16;
const int nNumCareerMaxPlayers = 12;

std::string GetCustomSavePath(int id) {
	return std::format("Savegame/customsave{:03}.sav", id);
}

struct tCustomSaveStructure {
	struct {
		uint32_t bIsLocked : 1;
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
		void ApplyUpgradesToCar() {
			pGameFlow->Profile.nNumCarUpgrades = 0;

			for (int i = 0; i < 19*8; i++) {
				if (IsUpgradePurchased(i)) {
					pGameFlow->Profile.aCarUpgrades[pGameFlow->Profile.nNumCarUpgrades++] = i;
				}
				if (pGameFlow->Profile.nNumCarUpgrades >= 40) break;
			}
		}
		void Clear() {
			memset(this,0,sizeof(*this));
		}
	} aCareerGarage[256];
	bool tracksWon[256];
	struct tCareerClass {
		struct tCareerCup {
			bool bUnlocked : 1;
			uint8_t nPosition : 4;
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
		int points;
		int eventPosition[nNumCareerEventsPerCup];
		int eventPoints[nNumCareerEventsPerCup];
	} aCareerCupPlayers[nNumCareerMaxPlayers];
	int nGameCompletion;
	int nCupsPassed;
	int nCupsMax;
	int nCarsUnlocked;

	static inline bool bInitialized = false;
	static inline uint8_t aCupPlayersByPosition[nNumCareerMaxPlayers];
	static inline uint8_t aCupPlayerPosition[nNumCareerMaxPlayers];
	static inline uint8_t aCupLocalPlayerPlacements[nNumCareerMaxPlayers];

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
			players.push_back({i, aCareerCupPlayers[i].points});
		}
		sort(players.begin(), players.end(), tLeaderboardEntry::compFunction);
		for (int i = 0; i < nNumCareerMaxPlayers; i++) {
			aCupPlayersByPosition[i] = players[i].playerId;
			aCupPlayerPosition[players[i].playerId] = i;
		}
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
	}
	void ApplyPlayerSettings() const {

	}
	void ReadPlayerSettings() {

	}
	void Clear() {
		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();
	}
	void Load(int saveSlot = -1) {
		if (saveSlot < 0) {
			saveSlot = pGameFlow->nSaveSlot;
			if (saveSlot < 0) {
				saveSlot = pGameFlow->Profile.nAutosaveSlot;
			}
		}

		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();

		auto file = std::ifstream(GetCustomSavePath(saveSlot+1), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		file.read((char*)this, sizeof(*this));

		// force unlock first career bits in case of save corruption
		bCareerClassUnlocked[0] = true;
		for (auto& data : aCareerClasses) {
			data.aCups[0].bUnlocked = true;
		}
	}
	void Save() {
		if (!bInitialized) return;

		int saveSlot = pGameFlow->nSaveSlot;
		if (saveSlot < 0) {
			saveSlot = pGameFlow->Profile.nAutosaveSlot;
		}

		if (saveSlot < 0) {
			MessageBoxA(0, "Trying to save to slot 0, this is a bug", "nya?!~", MB_ICONWARNING);
		}

		ReadPlayerSettings();

		auto file = std::ofstream(GetCustomSavePath(saveSlot+1), std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		file.write((char*)this, sizeof(*this));
	}
	void Delete(int slot) {
		auto save = GetCustomSavePath(slot+1);
		if (std::filesystem::exists(save)) {
			std::filesystem::remove(save);
		}
	}
} gCustomSave;

void ProcessPlayStats() {
	static CNyaTimer gTimer;
	gTimer.Process();

	if (pLoadingScreen) return;

	if (GetGameState() == GAME_STATE_RACE && GetPlayerScore<PlayerScoreRace>(1)->bHasFinished && pPlayerHost->GetNumPlayers() > 1) {
		bool changed = false;
		int track = pGameFlow->nLevel;

		// time trials should count for this
		if (pGameFlow->nSubEventType != eSubEventType::RACE_TIMETRIAL) {
			auto ply = GetPlayerScore<PlayerScoreRace>(1);
			if (ply->bHasFinished) {
				if (ply->nPosition == 1 && !gCustomSave.tracksWon[track]) {
					gCustomSave.tracksWon[track] = true;
					changed = true;
				}
			}
		}

		if (changed) {
			DoGameSave();
		}
	}

	static auto lastGameState = GetGameState();
	if (lastGameState == GAME_STATE_RACE && GetGameState() == GAME_STATE_MENU) {
		DoGameSave();
	}
	lastGameState = GetGameState();
}