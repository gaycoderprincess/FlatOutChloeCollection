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
	TRACKTYPE_FIELDS,
	TRACKTYPE_PIT,
	TRACKTYPE_RACING,
	TRACKTYPE_TOWN,
	TRACKTYPE_WINTER,
	TRACKTYPE_DESERT,
	TRACKTYPE_TOUGHTRUCKS,
	TRACKTYPE_ARENA,
	NUM_RACE_TRACKTYPES,
	TRACKTYPE_DERBY = NUM_RACE_TRACKTYPES,
	TRACKTYPE_STUNT,
	NUM_TRACKTYPES
};