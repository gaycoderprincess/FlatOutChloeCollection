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

namespace CareerMode {
	bool bIsCareerRace = false;
	bool bNextRaceCareerRace = false;
	bool bLastRaceCareerRace = false;
}