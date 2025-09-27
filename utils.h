void WriteLog(const std::string& str) {
	static auto file = std::ofstream("FlatOutChloeCollection_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

bool bDebugLog = false;
void WriteLogDebug(const std::string& title, const std::string& str) {
	if (!bDebugLog) return;

	static auto file = std::ofstream("FlatOutChloeCollection_gcp_debug.log");

	file << "[" + title + "] ";
	file << str;
	file << "\n";
	file.flush();
}

auto GetStringWide(const std::string& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(string);
}

auto GetStringNarrow(const std::wstring& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(string);
}

std::string GetCustomSavePath(int id) {
	return std::format("Savegame/customsave{:03}.sav", id);
}

std::string GetAchievementSavePath(int id) {
	return std::format("Savegame/customsave{:03}.ach", id);
}

std::string FormatScore(int a1) {
	if (a1 < 1000) {
		return std::to_string(a1);
	}
	auto v4 = a1 / 1000;
	if (a1 >= 1000000) {
		return std::format("{},{:03},{:03}", a1 / 1000000, v4 % 1000, a1 % 1000);
	}
	return std::format("{},{:03}", v4, a1 % 1000);
}

std::string FormatGameTime(int ms, bool leadingZero = true) {
	if (ms < 0) ms = 0;
	std::string str = GetTimeFromMilliseconds(ms, true);
	str.pop_back(); // remove trailing zero
	// add leading zero
	if (leadingZero && ms < 60 * 10 * 1000) {
		str = "0" + str;
	}
	return str;
}

int32_t GetPlayerLapTime(Player* ply, int lapId) {
	if (lapId > ply->nCurrentLap) return 0;

	auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
	auto lap = score->nLapTimes[lapId];
	if (lap <= 0) return 0;
	if (lapId > 0) lap -= score->nLapTimes[lapId - 1];
	return lap;
}

int GetWreckingDerbyCrashScore(int player) {
	int score = 0;
	score += aCrashBonusesReceived[player][CRASHBONUS_SLAM] * 1;
	score += aCrashBonusesReceived[player][CRASHBONUS_POWERHIT] * 2;
	score += aCrashBonusesReceived[player][CRASHBONUS_BLASTOUT] * 3;
	return score;
}

int GetWreckingDerbyWreckScore(int player) {
	int score = 0;
	score += aCrashBonusesReceived[player][CRASHBONUS_WRECKED] * 5;
	return score;
}

int GetWreckingDerbyBonusScore(int position, bool finished) {
	if (!finished) {
		int playersAlive = 0;
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (!GetPlayer(i)->nIsRagdolled) playersAlive++;
		}

		if (playersAlive == 1) { return 15; }
		if (playersAlive == 2) { return 10; }
		if (playersAlive == 3) { return 5; }
		return 0;
	}

	if (position == 1) { return 15; }
	if (position == 2) { return 10; }
	if (position == 3) { return 5; }
	return 0;
}

int GetWreckingDerbyTotalScore(int player, int position, bool finished) {
	return GetWreckingDerbyCrashScore(player) + GetWreckingDerbyWreckScore(player) + GetWreckingDerbyBonusScore(position, finished);
}

std::vector<PlayerScoreRace*> GetSortedPlayerScores() {
	std::vector<PlayerScoreRace*> aScores;
	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		aScores.push_back(GetScoreManager()->aScores[i]);
	}
	if (bIsFragDerby) {
		std::sort(aScores.begin(), aScores.end(), [](PlayerScoreRace *a, PlayerScoreRace *b) {
			return FragDerby::nPlayerScore[a->nPlayerId] > FragDerby::nPlayerScore[b->nPlayerId];
		});
	}
	else if (bIsWreckingDerby) {
		std::sort(aScores.begin(), aScores.end(), [](PlayerScoreRace *a, PlayerScoreRace *b) {
			return GetWreckingDerbyTotalScore(a->nPlayerId, a->nPosition, a->bHasFinished || a->bIsDNF) > GetWreckingDerbyTotalScore(b->nPlayerId, b->nPosition, b->bHasFinished || b->bIsDNF);
		});
	}
	else {
		if (pGameFlow->nEventType == eEventType::RACE) {
			std::sort(aScores.begin(), aScores.end(), [](PlayerScoreRace *a, PlayerScoreRace *b) {
				if (a->bIsDNF && !b->bIsDNF) { return false; }
				if (b->bIsDNF && !a->bIsDNF) { return true; }
				if (a->bHasFinished && !b->bHasFinished) { return true; }
				if (b->bHasFinished && !a->bHasFinished) { return false; }
				if (a->bHasFinished && b->bHasFinished) {
					return a->nFinishTime < b->nFinishTime;
				}
				return a->nPosition < b->nPosition;
			});
		}
		else {
			std::sort(aScores.begin(), aScores.end(), [](PlayerScoreRace *a, PlayerScoreRace *b) {
				if (a->bIsDNF && !b->bIsDNF) { return false; }
				if (b->bIsDNF && !a->bIsDNF) { return true; }
				return a->nPosition < b->nPosition;
			});
		}
	}
	return aScores;
}

bool DoesTrackSupportAI(int level) {
	return !DoesTrackValueExist(level, "NoAI");
}

bool DoesTrackSupportReversing(int level) {
	if (level == TRACK_FOREST1A) return true;
	if (level == TRACK_FOREST1C) return true;
	if (level == TRACK_TOWN3A) return true;
	if (level == TRACK_TOWN3B) return true;
	if (level == TRACK_TOWN3C) return true;
	if (level == TRACK_PIT1A) return true;
	if (level == TRACK_PIT3A) return true;
	if (level == TRACK_PIT3B) return true;
	if (level == TRACK_PIT3C) return true;
	//if (level == TRACK_RACING1A) return true; // same as 1c
	if (level == TRACK_RACING1B) return true;
	//if (level == TRACK_RACING1C) return true; // same as 1a
	if (level == TRACK_RACING2B) return true;
	if (level == TRACK_TOWN2A) return true;
	if (level == TRACK_TOWN2C) return true;
	if (level == TRACK_WINTER1A) return true;
	if (level == TRACK_WINTER1B) return true;
	if (level == TRACK_WINTER1C) return true;
	//if (level == TRACK_WINTER2A) return true; // same as 2c
	if (level == TRACK_WINTER2B) return true;
	//if (level == TRACK_WINTER2C) return true; // same as 2a
	// chloe collection tracks
	//if (level == TRACK_DESERT1B) return true; // half broken?
	if (level == TRACK_DESERT1C) return true;
	if (level == TRACK_FIELDS1B) return true;
	if (level == TRACK_FIELDS1C) return true;
	if (level == TRACK_FIELDS2A) return true;
	if (level == TRACK_FIELDS2B) return true;
	if (level == TRACK_FIELDS2C) return true;
	if (level == TRACK_FO2FOREST1A) return true;
	if (level == TRACK_RETRODEMO1A) return true;
	if (level == TRACK_RETRODEMO2A) return true;
	if (level == TRACK_RETRODEMO2B) return true;
	return false;

	// working ones:
	/*
	woodland havoc
	woodland challenge
	hayville havoc
	hayville haystakker
	hayville farmdash
	bunker challenge
	mound mayhem
	gravel supersmash
	gravel rock run
	bay amateur run
	bay challenge
	bay cup run
	fairgrass run
	whattahoo wham
	whattahoo roadrun
	redpine race dash
	whitefish rally
	redpine zero course
	whitefish ice run
	redpine run
	whitefish run
	*/

	// broken ones:
	/*
	finchley trial run
	pinewood crashcup
	woodland mayhem
	finchley challenge
	pinewood qualifier
	finchley timberdash
	pinewood challenge
	brad's pit crash
	bunker hill havoc
	brad's sandrun
	bunker bigbang run
	brad's super dustup
	fairgrass cup
	fairgrass challenge
	whattahoo rush hour
	*/
}

const char* GetCrashBonusName(int type) {
	switch (type) {
	case CRASHBONUS_CRASHFLYBY:
		return "CRASH FLY-BY";
	case CRASHBONUS_SUPERFLIP:
		return "SUPER FLIP";
	case CRASHBONUS_SLAM:
		return "SLAM";
	case CRASHBONUS_POWERHIT:
		return "POWER HIT";
	case CRASHBONUS_BLASTOUT:
		return "BLAST OUT!";
	case CRASHBONUS_RAGDOLLED:
		return "CRASH OUT!";
	case CRASHBONUS_WRECKED:
		return "WRECKED!";
	}
	return nullptr;
}

NyaDrawing::CNyaRGBA32 GetMedalRGB(int medal) {
	switch (medal) {
	case 1:
		return {236,221,16, 255};
	case 2:
		return {186,186,186, 255};
	case 3:
		return {175, 100, 0, 255};
	case 4:
		return {30,160,9, 255};
	default:
		return {255,255,255,255};
	}
}

enum eControlBind {
	CONFIG_NONE,
	CONFIG_HANDBRAKE,
	CONFIG_HANDBRAKE_ALT,
	CONFIG_NITRO,
	CONFIG_GEARDOWN,
	CONFIG_GEARUP,
	CONFIG_PAUSE,
	CONFIG_STEERING,
	CONFIG_CAMERA,
	CONFIG_RAGDOLL,
	CONFIG_RESET,
	CONFIG_BRAKE,
	CONFIG_THROTTLE,
	CONFIG_LOOK,
	CONFIG_THROTTLEBRAKE,
	CONFIG_PLAYERLIST,
	CONFIG_SKIPMUSIC,
};

enum eExtraControllerButtons {
	CONTROLLER_BUTTON_RIGHT_UP = CONTROLLER_BUTTON_RIGHT_TRIGGER + 1,
	CONTROLLER_BUTTON_RIGHT_DOWN,
	CONTROLLER_BUTTON_RIGHT_LEFT,
	CONTROLLER_BUTTON_RIGHT_RIGHT,
	NUM_CONTROLLER_BUTTONS,
};

static int GetControllerBinding(int key) {
	// vanilla FOUC configs
	if (nControllerLayout == 0) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_EXTRA4: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_EXTRA1: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_A: return CONFIG_RAGDOLL;
			case CONTROLLER_BUTTON_B: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_X: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_Y: return CONFIG_RESET;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	if (nControllerLayout == 1) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_EXTRA4: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_RAGDOLL;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_EXTRA1: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_A: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_B: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_X: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_Y: return CONFIG_RESET;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	if (nControllerLayout == 2) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_EXTRA4: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_EXTRA1: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_A: return CONFIG_RAGDOLL;
			case CONTROLLER_BUTTON_B: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_X: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_Y: return CONFIG_RESET;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	if (nControllerLayout == 3) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_EXTRA4: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_RAGDOLL;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_EXTRA1: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_A: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_B: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_X: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_Y: return CONFIG_RESET;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	// FO1 xbox config
	if (nControllerLayout == 4) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_A: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_B: return CONFIG_RAGDOLL;
			case CONTROLLER_BUTTON_X: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_Y: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_EXTRA1: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_RESET;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_EXTRA4: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	// FO1 ps2 config
	if (nControllerLayout == 5) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_A: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_B: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_X: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_Y: return CONFIG_RESET;
			case CONTROLLER_BUTTON_EXTRA1: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_RAGDOLL;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_EXTRA4: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	// NFS-like config 1
	if (nControllerLayout == 6) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_EXTRA1: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_A: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_B: return CONFIG_RAGDOLL;
			case CONTROLLER_BUTTON_Y: return CONFIG_RESET;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_RIGHT_UP: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_RIGHT_DOWN: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	// NFS-like config 2
	if (nControllerLayout == 7) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_EXTRA1: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_A: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_X: return CONFIG_RAGDOLL; // changed nitro from B to X since speedbreaker isn't a thing
			case CONTROLLER_BUTTON_Y: return CONFIG_RESET;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_RIGHT_UP: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_RIGHT_DOWN: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	// GTA-like config
	if (nControllerLayout == 8) {
		switch (key) {
			case CONTROLLER_BUTTON_UP: return CONFIG_STEERING;
			case CONTROLLER_BUTTON_EXTRA4: return CONFIG_LOOK;
			case CONTROLLER_BUTTON_LEFT_TRIGGER: return CONFIG_BRAKE;
			case CONTROLLER_BUTTON_RIGHT_TRIGGER: return CONFIG_THROTTLE;
			case CONTROLLER_BUTTON_EXTRA2: return CONFIG_HANDBRAKE;
			case CONTROLLER_BUTTON_A: return CONFIG_HANDBRAKE_ALT;
			case CONTROLLER_BUTTON_B: return CONFIG_CAMERA;
			case CONTROLLER_BUTTON_X: return CONFIG_RAGDOLL;
			case CONTROLLER_BUTTON_Y: return CONFIG_RESET;
			case CONTROLLER_BUTTON_DIGITAL_LEFT: return CONFIG_PLAYERLIST;
			case CONTROLLER_BUTTON_RIGHT_UP: return CONFIG_GEARUP;
			case CONTROLLER_BUTTON_RIGHT_DOWN: return CONFIG_GEARDOWN;
			case CONTROLLER_BUTTON_SELECT: return CONFIG_SKIPMUSIC;
			case CONTROLLER_BUTTON_START: return CONFIG_PAUSE;
			default: return CONFIG_NONE;
		}
	}
	return CONFIG_NONE;
}

int GetControllerKeyForAction(int action) {
	for (int i = 0; i < NUM_CONTROLLER_BUTTONS; i++) {
		if (GetControllerBinding(i) == action) return i;
	}
	return NUM_CONTROLLER_BUTTONS;
}

int FO2ToNyaControllerKey(int key) {
	switch (key) {
		case CONTROLLER_BUTTON_A: return NYA_PAD_KEY_A;
		case CONTROLLER_BUTTON_B: return NYA_PAD_KEY_B;
		case CONTROLLER_BUTTON_X: return NYA_PAD_KEY_X;
		case CONTROLLER_BUTTON_Y: return NYA_PAD_KEY_Y;
		case CONTROLLER_BUTTON_EXTRA1: return NYA_PAD_KEY_LB;
		case CONTROLLER_BUTTON_EXTRA2: return NYA_PAD_KEY_RB;
		case CONTROLLER_BUTTON_EXTRA3: return NYA_PAD_KEY_L3;
		case CONTROLLER_BUTTON_EXTRA4: return NYA_PAD_KEY_R3;
		case CONTROLLER_BUTTON_START: return NYA_PAD_KEY_START;
		case CONTROLLER_BUTTON_SELECT: return NYA_PAD_KEY_SELECT;
		case CONTROLLER_BUTTON_UP: return NYA_PAD_KEY_LSTICK_Y_NEG;
		case CONTROLLER_BUTTON_DOWN: return NYA_PAD_KEY_LSTICK_Y_POS;
		case CONTROLLER_BUTTON_LEFT: return NYA_PAD_KEY_LSTICK_X_NEG;
		case CONTROLLER_BUTTON_RIGHT: return NYA_PAD_KEY_LSTICK_X_POS;
		case CONTROLLER_BUTTON_RIGHT_UP: return NYA_PAD_KEY_RSTICK_Y_NEG;
		case CONTROLLER_BUTTON_RIGHT_DOWN: return NYA_PAD_KEY_RSTICK_Y_POS;
		case CONTROLLER_BUTTON_RIGHT_LEFT: return NYA_PAD_KEY_RSTICK_X_NEG;
		case CONTROLLER_BUTTON_RIGHT_RIGHT: return NYA_PAD_KEY_RSTICK_X_POS;
		case CONTROLLER_BUTTON_DIGITAL_UP: return NYA_PAD_KEY_DPAD_UP;
		case CONTROLLER_BUTTON_DIGITAL_DOWN: return NYA_PAD_KEY_DPAD_DOWN;
		case CONTROLLER_BUTTON_DIGITAL_LEFT: return NYA_PAD_KEY_DPAD_LEFT;
		case CONTROLLER_BUTTON_DIGITAL_RIGHT: return NYA_PAD_KEY_DPAD_RIGHT;
		case CONTROLLER_BUTTON_LEFT_TRIGGER: return NYA_PAD_KEY_LT;
		case CONTROLLER_BUTTON_RIGHT_TRIGGER: return NYA_PAD_KEY_RT;
		default: return NYA_PAD_KEY_NONE;
	}
}

int GetNyaControllerKeyForAction(int action) {
	return FO2ToNyaControllerKey(GetControllerKeyForAction(action));
}

struct tWeather {
	int id;
	std::string name;
};
std::vector<tWeather> aTrackWeathers[nMaxTracks];