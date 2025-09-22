class CMenu_TrackSelect : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_trackselect"; }

	static inline const char* aTrackTypeNames[] = {
		"FOREST",
		"FIELD",
		"PIT",
		"RACE",
		"TOWN",
		"WINTER",
		"DESERT",
		"TOUGH TRUCKS",
		"BONUS",
		"DERBY",
		"STUNT",
	};

	enum eNewGameType {
		GAMETYPE_RACE,
		GAMETYPE_ARCADERACE,
		GAMETYPE_DERBY_LMS,
		GAMETYPE_DERBY_WRECKING,
		GAMETYPE_DERBY_FRAG,
		GAMETYPE_STUNT,
	};

	static inline int aGameTypesSinglePlayer[] = {
			GAMETYPE_RACE,
			GAMETYPE_ARCADERACE,
			GAMETYPE_DERBY_LMS,
			GAMETYPE_DERBY_WRECKING,
			GAMETYPE_DERBY_FRAG,
			GAMETYPE_STUNT,
	};

	static inline int aGameTypesSplitScreen[] = {
			GAMETYPE_RACE,
			GAMETYPE_DERBY_LMS,
			GAMETYPE_DERBY_WRECKING,
	};

	static inline int aGameTypesMultiplayer[] = {
			GAMETYPE_RACE,
			GAMETYPE_ARCADERACE,
			GAMETYPE_DERBY_LMS,
			GAMETYPE_DERBY_WRECKING,
			GAMETYPE_DERBY_FRAG,
	};

	enum eDamageLevel {
		DAMAGE_0,
		DAMAGE_50,
		DAMAGE_100,
		DAMAGE_200,
		DAMAGE_500,
		DAMAGE_1000,
		NUM_DAMAGE_LEVELS
	};

	enum eUpgradeLevel {
		UPGRADE_0,
		UPGRADE_50,
		UPGRADE_100,
		NUM_UPGRADE_LEVELS
	};

	enum eCarClass {
		CARCLASS_ANY,
		CARCLASS_BRONZE,
		CARCLASS_SILVER,
		CARCLASS_GOLD,
		CARCLASS_SAMEASHOST,
		NUM_CAR_CLASSES
	};

	enum eAIClass {
		AI_CLASS_BRONZE,
		AI_CLASS_SILVER,
		AI_CLASS_GOLD,
		AI_CLASS_SAMEASHOST,
		NUM_AI_CLASSES
	};

	enum eMultiplayerNitro {
		MULTIPLAYER_NITRO_0,
		MULTIPLAYER_NITRO_100,
		MULTIPLAYER_NITRO_100_REGEN,
		NUM_MULTIPLAYER_NITRO_LEVELS
	};

	struct tOption {
		std::string name;
		int* value;
	};

	static inline int nDummyValue = 0;

	static inline int nGameType = GAMETYPE_RACE;
	static inline int nTrackType = TRACKTYPE_FOREST;
	static inline int nTrack = 0;
	static inline int nTrackReversed = 0;
	static inline int nLaps = 3;
	static inline int nDamage = DAMAGE_100;
	static inline int nNitro = QuickRace::NITRO_100;
	static inline int nUpgrades = UPGRADE_0;

	static inline int nTimeTrialProps = 1;
	static inline int nTimeTrial3LapMode = 0;

	static inline int nMultiplayerNitro = MULTIPLAYER_NITRO_100;
	static inline int nMultiplayerHandling = 0;
	static inline int nMultiplayerCarClass = CARCLASS_ANY;
	static inline int nMultiplayerAIClass = 0;
	static inline int nMultiplayerAICount = 0;

	static inline tOption aOptionsQuickRace[] = {
			{"GAME TYPE", &nGameType},
			{"TRACK TYPE", &nTrackType},
			{"TRACK", &nTrack},
			{"LAPS", &nLaps},
			{"REVERSED", &nTrackReversed},
			{"DAMAGE", &nDamage},
			{"NITRO", &nNitro},
			{"UPGRADES", &nUpgrades},
			{"", nullptr},
			{"GO RACE", nullptr},
			{"*END*", nullptr},
	};

	static inline tOption aOptionsMultiplayer[] = {
			{"GAME TYPE", &nGameType},
			{"TRACK TYPE", &nTrackType},
			{"TRACK", &nTrack},
			{"LAPS", &nLaps},
			{"CAR CLASS", &nMultiplayerCarClass},
			{"DAMAGE", &nDamage},
			{"NITRO", &nMultiplayerNitro},
			{"UPGRADES", &nUpgrades},
			{"HANDLING", &nMultiplayerHandling},
			{"AI CLASS", &nMultiplayerAIClass},
			{"AI COUNT", &nMultiplayerAICount},
			{"*END*", nullptr},
	};

	static inline tOption aOptionsMultiplayerCreate[] = {
			{"GAME TYPE", &nGameType},
			{"TRACK TYPE", &nTrackType},
			{"TRACK", &nTrack},
			{"LAPS", &nLaps},
			{"CAR CLASS", &nMultiplayerCarClass},
			{"DAMAGE", &nDamage},
			{"NITRO", &nMultiplayerNitro},
			{"UPGRADES", &nUpgrades},
			{"HANDLING", &nMultiplayerHandling},
			{"", nullptr},
			{"APPLY SETTINGS", nullptr},
			{"*END*", nullptr},
	};

	static inline tOption aOptionsHotSeat[] = {
			{"TRACK", &nTrack},
			{"UPGRADES", &nUpgrades},
			{"", nullptr},
			{"GO RACE", nullptr},
			{"*END*", nullptr},
	};

	static inline tOption aOptionsTimeTrial[] = {
			{"TRACK TYPE", &nTrackType},
			{"TRACK", &nTrack},
			{"REVERSED", &nTrackReversed},
			{"NITRO", &nNitro},
			{"UPGRADES", &nUpgrades},
			{"PROPS", &nTimeTrialProps},
			{"MODE", &nTimeTrial3LapMode},
			{"", nullptr},
			{"GO RACE", nullptr},
			{"*END*", nullptr},
	};

	static inline tOption aOptionsInstantAction[] = {
			{"GAME TYPE", &nDummyValue},
			{"TRACK", &nDummyValue},
			{"LAPS", &nDummyValue},
			{"CAR", &nDummyValue},
			{"UPGRADES", &nDummyValue},
			{"", nullptr},
			{"GO RACE", nullptr},
			{"*END*", nullptr},
	};

	static inline bool IsMultiplayerMenu() {
		return aOptions == aOptionsMultiplayer || aOptions == aOptionsMultiplayerCreate;
	}

	static inline tOption* aOptions = aOptionsQuickRace;
	static inline bool bSplitScreen = false;
	static inline bool bMultiplayerCreateGame = false;

	static int GetGameType() {
		if (aOptions == aOptionsInstantAction) {
			auto& event = InstantAction::gEvent;
			if (event.bFragDerby) return GAMETYPE_DERBY_FRAG;
			else if (event.bWreckingDerby) return GAMETYPE_DERBY_WRECKING;
			else if (event.bArcadeRace) return GAMETYPE_ARCADERACE;
			return event.bDerbyTrack ? GAMETYPE_DERBY_LMS : GAMETYPE_RACE;
		}
		if (aOptions == aOptionsHotSeat) return GAMETYPE_STUNT;
		if (aOptions == aOptionsTimeTrial) return GAMETYPE_RACE;
		if (bSplitScreen) return aGameTypesSplitScreen[nGameType];
		if (IsMultiplayerMenu()) return aGameTypesMultiplayer[nGameType];
		return aGameTypesSinglePlayer[nGameType];
	}

	static int GetGameTypeCount() {
		if (aOptions == aOptionsHotSeat) return 1;
		if (bSplitScreen) return sizeof(aGameTypesSplitScreen)/sizeof(aGameTypesSplitScreen[0]);
		if (IsMultiplayerMenu()) return sizeof(aGameTypesMultiplayer)/sizeof(aGameTypesMultiplayer[0]);
		return sizeof(aGameTypesSinglePlayer)/sizeof(aGameTypesSinglePlayer[0]);
	}

	static eEventType GetGameMode() {
		switch (GetGameType()) {
			case GAMETYPE_RACE:
			case GAMETYPE_ARCADERACE:
			default:
				return eEventType::RACE;
			case GAMETYPE_DERBY_LMS:
			case GAMETYPE_DERBY_WRECKING:
			case GAMETYPE_DERBY_FRAG:
				return eEventType::DERBY;
			case GAMETYPE_STUNT:
				return eEventType::STUNT;
		}
	}

	static std::string GetGameModeString() {
		switch (GetGameType()) {
			case GAMETYPE_RACE:
			case GAMETYPE_ARCADERACE:
			default:
				return "RACE";
			case GAMETYPE_DERBY_LMS:
			case GAMETYPE_DERBY_WRECKING:
			case GAMETYPE_DERBY_FRAG:
				return "DERBY";
			case GAMETYPE_STUNT:
				return "STUNT";
		}
	}

	bool DoesEventHaveAI() const {
		if (aOptions == aOptionsTimeTrial) return false;
		if (bSplitScreen) return false;
		if (IsMultiplayerMenu() && nMultiplayerAICount <= 0) return false;
		return true;
	}

	struct tTrackEntry {
		int level;
		bool reversed;
	};
	std::vector<tTrackEntry> GetTrackSelection() const {
		std::vector<tTrackEntry> aTracks;
		for (int i = 1; i < GetNumTracks()+1; i++) {
			if (!DoesTrackExist(i)) continue;
			if (DoesEventHaveAI() && !DoesTrackSupportAI(i)) continue;
			if (GetTrackValueNumber(i, "TrackType") == nTrackType) {
				aTracks.push_back({i, false});
			}
		}
		if (IsMultiplayerMenu()) {
			for (int i = 1; i < GetNumTracks() + 1; i++) {
				if (!DoesTrackExist(i)) continue;
				if (nMultiplayerAICount > 0 && !DoesTrackSupportReversing(i)) continue;
				if (GetTrackValueNumber(i, "TrackType") == nTrackType) {
					aTracks.push_back({i, true});
				}
			}
		}
		return aTracks;
	}

	int GetTrackId() const {
		if (aOptions == aOptionsInstantAction) return InstantAction::gEvent.nLevel;

		auto aTracks = GetTrackSelection();
		if (aTracks.empty()) {
			WriteLog(std::format("No tracks available for category {}!", aTrackTypeNames[nTrackType-1]));
			return 1;
		}
		if (nTrack <= 0 || nTrack >= aTracks.size()) {
			if (IsMultiplayerMenu()) nTrackReversed = aTracks[0].reversed;
			return aTracks[0].level;
		}
		if (IsMultiplayerMenu()) nTrackReversed = aTracks[nTrack].reversed;
		return aTracks[nTrack].level;
	}

	static float GetDamageLevel() {
		switch (nDamage) {
			case DAMAGE_0:
			default:
				return 0;
			case DAMAGE_50:
				return 0.5;
			case DAMAGE_100:
				return 1.0;
			case DAMAGE_200:
				return 2.0;
			case DAMAGE_500:
				return 5.0;
			case DAMAGE_1000:
				return 10.0;
		}
	}

	static float GetUpgradeLevel() {
		switch (nUpgrades) {
			case UPGRADE_0:
			default:
				return 0;
			case UPGRADE_50:
				return 0.5;
			case UPGRADE_100:
				return 1.0;
		}
	}

	int nCursorY = 0;
	void CheckOptionBounds(const int* changedValue) {
		if (nGameType < 0) nGameType = GetGameTypeCount()-1;
		if (nGameType >= GetGameTypeCount()) nGameType = 0;

		if (GetGameMode() == eEventType::RACE) {
			if (nTrackType < TRACKTYPE_FOREST) nTrackType = NUM_RACE_TRACKTYPES - 1;
			if (nTrackType >= NUM_RACE_TRACKTYPES) nTrackType = TRACKTYPE_FOREST;
		}
		else if (GetGameMode() == eEventType::DERBY) {
			nTrackType = TRACKTYPE_DERBY;
		}
		else if (GetGameMode() == eEventType::STUNT) {
			nTrackType = TRACKTYPE_STUNT;
		}

		if (changedValue == &nGameType || changedValue == &nTrackType) {
			nTrack = 0;
		}

		auto tracks = GetTrackSelection();
		if (nTrack < 0) nTrack = tracks.size()-1;
		if (nTrack >= tracks.size()) nTrack = 0;

		// reset lap count
		if (changedValue == &nGameType || changedValue == &nTrackType || changedValue == &nTrack) {
			nLaps = DoesTrackValueExist(tracks[nTrack].level, "Laps") ? GetTrackValueNumber(tracks[nTrack].level, "Laps") : 3;
		}

		if (nLaps < 1) nLaps = 1;
		if (nLaps > 10) nLaps = 10;

		if (GetGameMode() == eEventType::DERBY) {
			if (nDamage < DAMAGE_50) nDamage = DAMAGE_50;
			if (nDamage >= NUM_DAMAGE_LEVELS) nDamage = NUM_DAMAGE_LEVELS-1;
		}
		else {
			if (nDamage < 0) nDamage = 0;
			if (nDamage >= NUM_DAMAGE_LEVELS) nDamage = NUM_DAMAGE_LEVELS-1;
		}

		if (nNitro < 0) nNitro = 0;
		if (nNitro >= QuickRace::NUM_NITRO_LEVELS) nNitro = QuickRace::NUM_NITRO_LEVELS-1;

		if (nMultiplayerNitro < 0) nMultiplayerNitro = 0;
		if (nMultiplayerNitro >= NUM_MULTIPLAYER_NITRO_LEVELS) nMultiplayerNitro = NUM_MULTIPLAYER_NITRO_LEVELS;

		if (nUpgrades < 0) nUpgrades = 0;
		if (nUpgrades >= NUM_UPGRADE_LEVELS) nUpgrades = NUM_UPGRADE_LEVELS-1;

		if (nMultiplayerCarClass < 0) nMultiplayerCarClass = NUM_CAR_CLASSES-1;
		if (nMultiplayerCarClass >= NUM_CAR_CLASSES) nMultiplayerCarClass = 0;

		if (nMultiplayerHandling < 0) nMultiplayerHandling = NUM_HANDLING_MODES-1;
		if (nMultiplayerHandling >= NUM_HANDLING_MODES) nMultiplayerHandling = 0;

		if (nMultiplayerAIClass < 0) nMultiplayerAIClass = NUM_AI_CLASSES-1;
		if (nMultiplayerAIClass >= NUM_AI_CLASSES) nMultiplayerAIClass = 0;

		if (nMultiplayerAICount < 0) nMultiplayerAICount = 0;
		if (nMultiplayerAICount > 7) nMultiplayerAICount = 7;

		nTrackReversed = nTrackReversed == 1 || nTrackReversed == -1;
		nTimeTrialProps = nTimeTrialProps == 1 || nTimeTrialProps == -1;
		nTimeTrial3LapMode = nTimeTrial3LapMode == 1 || nTimeTrial3LapMode == -1;

		if (DoesEventHaveAI() && !DoesTrackSupportReversing(GetTrackId())) nTrackReversed = 0;
	}

	bool IsOptionValid(int option) const {
		if (aOptions[option].name.empty()) return false;
		if (DoesEventHaveAI() && aOptions[option].value == &nTrackReversed && !DoesTrackSupportReversing(GetTrackId())) return false;
		auto gameType = GetGameType();
		if (gameType == GAMETYPE_ARCADERACE && aOptions[option].value == &nLaps) return false;
		if (gameType != GAMETYPE_RACE && gameType != GAMETYPE_ARCADERACE && gameType != GAMETYPE_DERBY_WRECKING && gameType != GAMETYPE_DERBY_FRAG) {
			if (aOptions[option].value == &nNitro) return false;
			if (aOptions[option].value == &nMultiplayerNitro) return false;
		}
		if (GetGameMode() != eEventType::RACE) {
			if (aOptions[option].value == &nTrackType) return false;
			if (aOptions[option].value == &nLaps) return false;
			if (aOptions[option].value == &nTrackReversed) return false;
		}
		if (GetGameMode() == eEventType::STUNT) {
			if (aOptions[option].value == &nDamage) return false;
		}
		return true;
	}

	int GetNumOptions() const {
		int count = 0;
		while (true) {
			count++;
			if (aOptions[count].name == "*END*") return count;
		};
	}

	void MoveLeft() override {
		if (!aOptions[nCursorY].value) return;

		(*aOptions[nCursorY].value)--;
		CheckOptionBounds(aOptions[nCursorY].value);
	}
	void MoveRight() override {
		if (!aOptions[nCursorY].value) return;

		(*aOptions[nCursorY].value)++;
		CheckOptionBounds(aOptions[nCursorY].value);
	}
	void MoveUp() override {
		do {
			nCursorY--;
			if (nCursorY < 0) {
				nCursorY = GetNumOptions()-1;
			}
		} while (!IsOptionValid(nCursorY));
	}
	void MoveDown() override {
		do {
			nCursorY++;
			if (aOptions[nCursorY].name == "*END*") {
				nCursorY = 0;
			}
		} while (!IsOptionValid(nCursorY));
	}

	void Init() override {
		PreloadTexture("data/menu/trackselectbg_left.png");
		PreloadTexture("data/menu/trackselectbg_right.png");
		PreloadTexture("data/menu/track_icons.dds");
		PreloadTexture("data/menu/track_icons_2.dds");
		PreloadTexture("data/menu/track_name_icons.dds");
		PreloadTexture("data/menu/common.dds");
	}

	static inline tDrawPositions1080p gEvent = {1445, 460, 55};
	static inline tDrawPositions1080p gMap = {1445, 580, 80};
	static inline tDrawPositions1080p gOptions = {64, 250, 0.04, 600, 50};
	static inline tDrawPositions1080p gOptionsHover = {33, 226, 55, 890, 50};
	static inline float fArrowX = 0.21;
	static inline float fArrowYOffset = 0.003;
	static inline float fArrowSize = 0.02;

	static inline std::string mapPath = "data/tracks/forest/textures/map_forest1A.tga";
	static void SetMapPath(const std::string& str) {
		mapPath = str;
	}

	void ApplyOptions() const {
		if (aOptions == aOptionsHotSeat) {
			QuickRace::nNitroLevel = QuickRace::NITRO_100;
			QuickRace::fUpgradeLevel = GetUpgradeLevel();
			QuickRace::fDamageLevel = 0.0;
			QuickRace::nNumLaps = 3;
		}
		else if (aOptions == aOptionsTimeTrial) {
			QuickRace::nNitroLevel = QuickRace::NITRO_100;
			QuickRace::fUpgradeLevel = GetUpgradeLevel();
			QuickRace::fDamageLevel = 0.0;
			QuickRace::nNumLaps = nTimeTrial3LapMode ? 3 : 10;
		}
		else {
			QuickRace::nNitroLevel = nNitro;
			QuickRace::fUpgradeLevel = GetUpgradeLevel();
			QuickRace::fDamageLevel = GetGameMode() == eEventType::STUNT ? 1.0 : GetDamageLevel();
			QuickRace::nNumLaps = nLaps;
		}
	}

	static inline tDrawPositions gCarName = {0.304, 0.28, 0.062};
	static inline float fCarNameAspect = 171.0 / 39.0;

	static inline tDrawPositions1080p gLevelPB = {1445, 852, 0.04};

	static std::string GetTrackLogoPath(int track) {
		auto str = GetTrackValueString(track, "TrackPath"); // data/Tracks/Forest/Forest1/
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
		bool bIsFO2 = str.find("fo2") != std::string::npos;
		str.pop_back(); // remove trailing /
		// only leave the folder name
		while (str.find('/') != std::string::npos) {
			str.erase(str.begin());
		}
		if (str.starts_with("arena")) return "arena";
		if (str.starts_with("derby")) return "derby";
		if (str.starts_with("stunt")) return "stunt";
		if (bIsFO2) return "fo2" + str;
		return str;
	}

	static void DisplayTrackInfo(int trackId, float fXOffset = 0, int nXOffset = 0) {
		static auto textureTracks = LoadTextureFromBFS("data/menu/track_icons.dds");
		static auto textureTracks2 = LoadTextureFromBFS("data/menu/track_icons_2.dds");
		static auto textureTrackLogos = LoadTextureFromBFS("data/menu/track_name_icons.dds");
		static auto trackIcons = LoadHUDData("data/menu/track_icons.bed", "track_icons");
		static auto trackIcons2 = LoadHUDData("data/menu/track_icons_2.bed", "track_icons_2");
		auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(trackId, "Image"));
		auto trackIconImage = textureTracks;
		if (!trackIcon) {
			trackIcon = GetHUDData(trackIcons2, GetTrackValueString(trackId, "Image"));
			trackIconImage = textureTracks2;
		}
		
		static auto textureCarLogos = LoadTextureFromBFS("data/menu/track_name_icons.dds");
		static std::vector<tHUDData> gCarLogos = LoadHUDData("data/menu/track_name_icons.bed", "track_name_icons");

		auto bakCarName = gCarName;
		auto bakEvent = gEvent;
		auto bakMap = gMap;

		gCarName.fPosX += fXOffset;
		gEvent.nPosX += nXOffset;
		gMap.nPosX += nXOffset;

		if (auto logo = GetHUDData(gCarLogos, GetTrackLogoPath(trackId))) {
			DrawRectangle(1.0 - (gCarName.fPosX + gCarName.fSize * fCarNameAspect) * GetAspectRatioInv(),
						  1.0 - gCarName.fPosX * GetAspectRatioInv(), gCarName.fPosY,
						  gCarName.fPosY + gCarName.fSize, {255, 255, 255, 255}, 0, textureCarLogos, 0, logo->min,
						  logo->max);
		}

		auto data = gEvent;
		float x1 = data.nPosX - data.fSize * 1.5;
		float y1 = data.nPosY - data.fSize;
		float x2 = data.nPosX + data.fSize * 1.5;
		float y2 = data.nPosY + data.fSize;
		DoJustify(JUSTIFY_RIGHT, x1, y1);
		DoJustify(JUSTIFY_RIGHT, x2, y2);
		DrawRectangle(x1, x2, y1, y2, {255,255,255,255}, 0, trackIconImage, 0, trackIcon->min, trackIcon->max);
		if (!mapPath.empty()) {
			auto trackMap = LoadTextureFromBFS(mapPath.c_str());

			int x1 = gMap.nPosX - gMap.fSize;
			int x2 = gMap.nPosX + gMap.fSize;
			int y1 = gMap.nPosY - gMap.fSize;
			int y2 = gMap.nPosY + gMap.fSize;
			Draw1080pSprite(JUSTIFY_RIGHT, x1, x2, y1, y2, {255,255,255,255}, trackMap);
		}

		gCarName = bakCarName;
		gEvent = bakEvent;
		gMap = bakMap;
	}

	void Reset() override {
		nCursorY = 0;
		aOptions = aOptionsQuickRace;
		bSplitScreen = false;
	}

	std::string sStuntPB;

	void Process() override {
		static CNyaTimer gTimer;
		gTimer.Process();

		if (!bEnabled) return;

		static auto textureLeft = LoadTextureFromBFS("data/menu/trackselectbg_left.png");
		static auto textureRight = LoadTextureFromBFS("data/menu/trackselectbg_right.png");
		static auto textureCommon = LoadTextureFromBFS("data/menu/common.dds");
		static auto commonData = LoadHUDData("data/menu/common.bed", "common");

		Draw1080pSprite(JUSTIFY_LEFT, gOptionsHover.nPosX, gOptionsHover.nPosX + gOptionsHover.nSpacingX, gOptionsHover.nPosY + (nCursorY * gOptionsHover.nSpacingY), gOptionsHover.nPosY + (nCursorY * gOptionsHover.nSpacingY) + gOptionsHover.fSize, {0,0,0,200}, nullptr);

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		int trackId = GetTrackId();
		DisplayTrackInfo(trackId);
		if (GetGameType() == GAMETYPE_ARCADERACE || GetGameType() == GAMETYPE_DERBY_FRAG) {
			tNyaStringData data;
			data.x = gLevelPB.nPosX;
			data.y = gLevelPB.nPosY;
			data.size = gLevelPB.fSize;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_RIGHT, data, std::format("PERSONAL BEST: {}", gCustomSave.trackArcadeScores[trackId] ? FormatScore(gCustomSave.trackArcadeScores[trackId]) : "N/A"), &DrawStringFO2_Ingame12);
		}
		else if (!sStuntPB.empty()) {
			tNyaStringData data;
			data.x = gLevelPB.nPosX;
			data.y = gLevelPB.nPosY;
			data.size = gLevelPB.fSize;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_RIGHT, data, std::format("PERSONAL BEST: {}", sStuntPB), &DrawStringFO2_Ingame12);
		}
		else if (nTrackReversed && gCustomSave.bestLapsReversed[trackId]) {
			tNyaStringData data;
			data.x = gLevelPB.nPosX;
			data.y = gLevelPB.nPosY;
			data.size = gLevelPB.fSize;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_RIGHT, data, std::format("BEST LAP: {} ({})", FormatGameTime(gCustomSave.bestLapsReversed[trackId], false), GetCarName(gCustomSave.bestLapCarsReversed[trackId]+1)), &DrawStringFO2_Ingame12);
		}
		else if (!nTrackReversed && gCustomSave.bestLaps[trackId]) {
			tNyaStringData data;
			data.x = gLevelPB.nPosX;
			data.y = gLevelPB.nPosY;
			data.size = gLevelPB.fSize;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_RIGHT, data, std::format("BEST LAP: {} ({})", FormatGameTime(gCustomSave.bestLaps[trackId], false), GetCarName(gCustomSave.bestLapCars[trackId]+1)), &DrawStringFO2_Ingame12);
		}

		for (int y = 0; aOptions[y].name != "*END*"; y++) {
			auto& option = aOptions[y];
			if (option.name.empty()) continue;

			bool valid = IsOptionValid(y);

			tNyaStringData data;
			data.x = gOptions.nPosX;
			data.y = gOptions.nPosY + gOptions.nSpacingY * y;
			data.size = gOptions.fSize;
			if (!valid) {
				data.SetColor(127,127,127,255);
			}
			if (valid) data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
			Draw1080pString(JUSTIFY_LEFT, data, option.name == "APPLY SETTINGS" && bMultiplayerCreateGame ? "CREATE GAME" : option.name, &DrawStringFO2_Ingame12);
			data.x += gOptions.nSpacingX;
			data.XCenterAlign = true;
			if (valid) data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));

			if (option.name == "GO RACE" || option.name == "APPLY SETTINGS") break;
			if (!option.value) continue;

			auto gameType = GetGameType();

			int value = *option.value;
			std::string valueName = std::to_string(value);
			if (aOptions == aOptionsInstantAction) {
				auto& event = InstantAction::gEvent;
				if (option.name == "GAME TYPE") {
					switch (gameType) {
						case GAMETYPE_RACE:
						default:
							valueName = "RACE";
							break;
						case GAMETYPE_ARCADERACE:
							valueName = "ARCADE RACE";
							break;
						case GAMETYPE_DERBY_LMS:
							valueName = "SURVIVOR DERBY";
							break;
						case GAMETYPE_DERBY_WRECKING:
							valueName = "WRECKING DERBY";
							break;
						case GAMETYPE_DERBY_FRAG:
							valueName = "FRAG DERBY";
							break;
						case GAMETYPE_STUNT:
							valueName = "STUNT";
							break;
					}
				}
				if (option.name == "TRACK") {
					valueName = GetTrackName(event.nLevel);
					if (event.bLevelReversed) valueName = "REVERSED " + valueName;
				}
				if (option.name == "LAPS") {
					valueName = std::format("{}", event.nNumLaps);
				}
				if (option.name == "CAR") {
					valueName = GetCarName(event.nCar);
				}
				if (option.name == "UPGRADES") {
					valueName = std::format("{}%", (int)(event.fUpgradeLevel*100));
				}
			}
			else {
				if (option.value == &nGameType) {
					switch (gameType) {
						case GAMETYPE_RACE:
						default:
							valueName = "RACE";
							break;
						case GAMETYPE_ARCADERACE:
							valueName = "ARCADE RACE";
							break;
						case GAMETYPE_DERBY_LMS:
							valueName = "SURVIVOR DERBY";
							break;
						case GAMETYPE_DERBY_WRECKING:
							valueName = "WRECKING DERBY";
							break;
						case GAMETYPE_DERBY_FRAG:
							valueName = "FRAG DERBY";
							break;
						case GAMETYPE_STUNT:
							valueName = "STUNT";
							break;
					}
				}
				else if (option.value == &nTrackType) {
					valueName = aTrackTypeNames[value-1];
				}
				else if (option.value == &nTrack) {
					valueName = GetTrackValueString(trackId, "Name");
					if (IsMultiplayerMenu() && nTrackReversed) valueName = "REV " + valueName;
				}
				else if (option.value == &nLaps) {
					if (GetGameMode() != eEventType::RACE || GetGameType() == GAMETYPE_ARCADERACE) valueName = "N/A";
				}
				else if (option.value == &nDamage) {
					if (GetGameMode() == eEventType::STUNT) valueName = "N/A";
					else switch (value) {
						case DAMAGE_0:
						default:
							valueName = "0x";
							break;
						case DAMAGE_50:
							valueName = "0.5x";
							break;
						case DAMAGE_100:
							valueName = "1x";
							break;
						case DAMAGE_200:
							valueName = "2x";
							break;
						case DAMAGE_500:
							valueName = "5x";
							break;
						case DAMAGE_1000:
							valueName = "10x";
							break;
					}
				}
				else if (option.value == &nNitro) {
					if (gameType != GAMETYPE_RACE && gameType != GAMETYPE_ARCADERACE && gameType != GAMETYPE_DERBY_WRECKING && gameType != GAMETYPE_DERBY_FRAG) valueName = "N/A";
					else switch (value) {
						case QuickRace::NITRO_0:
						default:
							valueName = "0x";
							break;
						case QuickRace::NITRO_100:
							valueName = "1x";
							break;
						case QuickRace::NITRO_INFINITE:
							valueName = "INFINITE";
							break;
					}
				}
				else if (option.value == &nMultiplayerNitro) {
					if (gameType != GAMETYPE_RACE && gameType != GAMETYPE_ARCADERACE && gameType != GAMETYPE_DERBY_WRECKING && gameType != GAMETYPE_DERBY_FRAG) valueName = "N/A";
					else switch (value) {
						case MULTIPLAYER_NITRO_0:
						default:
							valueName = "OFF";
							break;
						case MULTIPLAYER_NITRO_100:
							valueName = "ON";
							break;
						case MULTIPLAYER_NITRO_100_REGEN:
							valueName = "ON + REGEN";
							break;
					}
				}
				else if (option.value == &nUpgrades) {
					switch (value) {
						case UPGRADE_0:
						default:
							valueName = "0%";
							break;
						case UPGRADE_50:
							valueName = "50%";
							break;
						case UPGRADE_100:
							valueName = "100%";
							break;
					}
				}
				else if (option.value == &nTimeTrialProps) {
					switch (value) {
						case 0:
						default:
							valueName = "OFF";
							break;
						case 1:
							valueName = "ON";
							break;
					}
				}
				else if (option.value == &nTimeTrial3LapMode) {
					switch (value) {
						case 0:
						default:
							valueName = "HOTLAPS";
							break;
						case 1:
							valueName = "3 LAP RUN";
							break;
					}
				}
				else if (option.value == &nMultiplayerHandling) {
					switch (value) {
						case HANDLING_NORMAL:
						default:
							valueName = "NORMAL";
							break;
						case HANDLING_PROFESSIONAL:
							valueName = "PROFESSIONAL";
							break;
						case HANDLING_HARDCORE:
							valueName = "HARDCORE";
							break;
					}
				}
				else if (option.value == &nMultiplayerAIClass) {
					switch (value) {
						case AI_CLASS_BRONZE:
						default:
							valueName = "BRONZE";
							break;
						case AI_CLASS_SILVER:
							valueName = "SILVER";
							break;
						case AI_CLASS_GOLD:
							valueName = "GOLD";
							break;
						case AI_CLASS_SAMEASHOST:
							valueName = "SAME AS HOST";
							break;
					}
				}
				else if (option.value == &nMultiplayerCarClass) {
					switch (value) {
						case CARCLASS_ANY:
						default:
							valueName = "ANY";
							break;
						case CARCLASS_BRONZE:
							valueName = "BRONZE";
							break;
						case CARCLASS_SILVER:
							valueName = "SILVER";
							break;
						case CARCLASS_GOLD:
							valueName = "GOLD";
							break;
						case CARCLASS_SAMEASHOST:
							valueName = "SAME AS HOST";
							break;
					}
				}
				else if (option.value == &nTrackReversed) {
					switch (value) {
						case 0:
						default:
							valueName = "OFF";
							break;
						case 1:
							valueName = "ON";
							break;
					}
				}
			}
			Draw1080pString(JUSTIFY_LEFT, data, valueName, &DrawStringFO2_Ingame12);

			if (y == nCursorY && aOptions != aOptionsInstantAction) {
				DoJustify(JUSTIFY_LEFT, data.x, data.y);

				static auto arrowLeft = GetHUDData(commonData, "nuolivasen");
				static auto arrowRight = GetHUDData(commonData, "nuolioikea");

				float arrowAspect = 16.0 / 29.0;
				float posLeft = data.x - (fArrowX * GetAspectRatioInv());
				float posRight = data.x + (fArrowX * GetAspectRatioInv());
				data.y += fArrowYOffset;
				DrawRectangle(posLeft - (fArrowSize * arrowAspect * GetAspectRatioInv()), posLeft + (fArrowSize * arrowAspect * GetAspectRatioInv()), data.y - fArrowSize, data.y + fArrowSize, {255,255,255,255}, 0, textureCommon, 0, arrowLeft->min, arrowLeft->max);
				DrawRectangle(posRight - (fArrowSize * arrowAspect * GetAspectRatioInv()), posRight + (fArrowSize * arrowAspect * GetAspectRatioInv()), data.y - fArrowSize, data.y + fArrowSize, {255,255,255,255}, 0, textureCommon, 0, arrowRight->min, arrowRight->max);
			}
		}
	}
} Menu_TrackSelect;