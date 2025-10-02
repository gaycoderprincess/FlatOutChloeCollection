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

NyaDrawing::CNyaRGBA32 aPlayerColorsMultiplayer[] = {
		//{255,241,195,255}, // default off white
		{255,255,255,255}, // default off white
		{236,221,16,255}, // jack yellow
		{255,122,0,255}, // orange
		{247,34,27,255}, // katie red
		{131,24,28,255}, // lei red
		{186,186,186,255}, // sofia gray
		{100,100,100,255}, // jason gray
		{245,135,135,255}, // sally pink
		{219,100,193,255}, // chloe collection pink
		{255,0,154,255}, // jill pink
		{135,50,220,255}, // light purple
		{89,232,247,255}, // ray cyan
		{54,93,246,255}, // frank blue
		{50,50,165,255}, // neville blue
		{230,175,105,255}, // lewis cream
		{0,190,0,255}, // light green
		{0,115,0,255}, // dark green
};