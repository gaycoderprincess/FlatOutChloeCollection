enum eHandlingMode {
	HANDLING_NORMAL,
	HANDLING_PROFESSIONAL,
	HANDLING_HARDCORE,
	NUM_HANDLING_MODES
};

enum eHandlingDamage {
	HANDLINGDAMAGE_OFF,
	HANDLINGDAMAGE_REDUCED,
	HANDLINGDAMAGE_ON,
};

enum eSplitScreenControllerP1 {
	SPLITSCREEN_P1_KEYBOARD,
	SPLITSCREEN_P1_PAD1,
	SPLITSCREEN_P1_PAD2,
	SPLITSCREEN_P1_PAD3,
	SPLITSCREEN_P1_PAD4,
};

enum eSplitScreenController {
	SPLITSCREEN_PAD1,
	SPLITSCREEN_PAD2,
	SPLITSCREEN_PAD3,
	SPLITSCREEN_PAD4,
};

enum eCrashBonus {
	CRASHBONUS_CRASHFLYBY,
	CRASHBONUS_SUPERFLIP,
	CRASHBONUS_SLAM,
	CRASHBONUS_POWERHIT,
	CRASHBONUS_BLASTOUT,
	CRASHBONUS_RAGDOLLED,
	CRASHBONUS_WRECKED,
	NUM_CRASHBONUS_TYPES
};
int aCrashBonusesReceived[nMaxPlayers][NUM_CRASHBONUS_TYPES] = {};

uint32_t nMenuSoundtrack = 0;
uint32_t nIngameSoundtrack = 0;
uint32_t nIngameDerbySoundtrack = 0;
uint32_t nIngameStuntSoundtrack = 0;
uint32_t nTransmission = 0;
uint32_t nUseNewLoadingScreen = 1;
uint32_t nHandlingDamage = HANDLINGDAMAGE_REDUCED;
uint32_t nHandlingMode = HANDLING_NORMAL;
uint32_t nPlayerListDefaultState = 1;
uint32_t nFO2CarsEnabled = 1;
uint32_t nShowBonus = 1;
uint32_t nControllerSupport = 1;
uint32_t nHighCarCam = 0;
uint32_t nSplitScreenController[4] = { SPLITSCREEN_P1_KEYBOARD, SPLITSCREEN_PAD1, SPLITSCREEN_PAD2, SPLITSCREEN_PAD3 };
uint32_t nSplitScreenDamageMeter = 1;
uint32_t nPlayerColor = 0;
uint32_t nPlayerFlag = 0;
uint32_t nUseFO2Minimap = 1;
uint32_t nControllerLayout = 0;
uint32_t nFO2Weathers = 1;
uint32_t nMPInterpolation = 1;
uint32_t nDefaultTickRate = 1;
uint32_t nAllowCheats = 0;
uint32_t nJoinNotifs = 1;

uint32_t nMultiplayerHandlingMode = HANDLING_NORMAL;

bool bNoPreload = false;
bool bAsyncPreload = false;
bool bNoTextures = false;

bool bIsInMultiplayer = false;
bool bIsArcadeMode = false;
bool bIsQuickRace = false;
bool bIsInstantAction = false;
bool bIsCareerRace = false;
bool bIsCarnageRace = false;
bool bIsSmashyRace = false;
bool bIsFragDerby = false;
bool bIsTimeTrial = false;

namespace CarnageRace {
	enum eScoreType {
		SCORE_CRASH,
		SCORE_SCENERY,
		SCORE_CHECKPOINT,
		SCORE_AIRTIME,
		NUM_SCORE_TYPES
	};
	int nPlayerScoresByType[NUM_SCORE_TYPES] = {};
}

bool bIsWreckingDerby = false;
void SetIsWreckingDerby(bool apply) {
	bIsWreckingDerby = apply;
	NyaHookLib::Patch<uint8_t>(0x44906F + 2, apply ? 3 : 1); // enable nitro in derby
}

bool IsNitroEnabledInDerby() {
	return bIsWreckingDerby || bIsFragDerby;
}

bool bMultiplayerNitroOn = true;
bool bMultiplayerNitroRegen = false;
float fMultiplayerUpgradeLevel = 0.0;
float fMultiplayerDamageLevel = 1.0;

bool bIsTrackReversed = false;
int nTrackWeather = 1;

namespace FragDerby {
	int nPlayerScore[nMaxPlayers] = {};

	int GetTopDriverOfType(int type);
}

namespace CarnageRace {
	int nPlayerScore[nMaxPlayers] = {};
}

namespace CareerMode {
	bool IsCareerTimeTrial();
}

bool IsInSplitScreen() {
	if (pLoadingScreen) return false;
	if (GetGameState() != GAME_STATE_RACE && GetGameState() != GAME_STATE_REPLAY) return false;
	if (pGameFlow->nGameMode != eGameMode::SPLITSCREEN) return false;
	if (pGameFlow->nEventType == eEventType::STUNT) return false;
	return true;
}

bool IsInHalvedSplitScreen() {
	if (!IsInSplitScreen()) return false;
	return pGameFlow->nNumSplitScreenPlayers == 2;
}

bool IsInQuarteredSplitScreen() {
	if (!IsInSplitScreen()) return false;
	return pGameFlow->nNumSplitScreenPlayers > 2;
}