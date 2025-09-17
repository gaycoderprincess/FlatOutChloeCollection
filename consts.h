const int nNumCareerClasses = 4;
const int nNumCareerEvents = 16;
const int nNumCareerEventsPerCup = 16;
const int nNumCareerMaxPlayers = 12;
const int nNumArcadeRaces = 128;

const int nMaxPlayers = 32;
const int nMaxSplitscreenPlayers = 4;

const int nMaxCars = 256;
const int nMaxTracks = 256;

enum eTrackType {
	TRACKTYPE_FOREST = 1,
	TRACKTYPE_FIELDS = 2,
	TRACKTYPE_PIT = 3,
	TRACKTYPE_RACING = 4,
	TRACKTYPE_TOWN = 5,
	TRACKTYPE_WINTER = 6,
	TRACKTYPE_DESERT = 7,
	TRACKTYPE_ARENA = 8,
	NUM_RACE_TRACKTYPES,
	TRACKTYPE_DERBY = NUM_RACE_TRACKTYPES,
	TRACKTYPE_STUNT,
	NUM_TRACKTYPES
};