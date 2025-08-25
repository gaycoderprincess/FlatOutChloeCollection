namespace QuickRace {
	bool bIsQuickRace = false;

	enum eNitroLevel {
		NITRO_0,
		NITRO_100,
		NITRO_INFINITE,
		NUM_NITRO_LEVELS
	};

	float fDamageLevel = 1.0;
	float fUpgradeLevel = 0.0;
	int nNitroLevel = NITRO_100;
	int nNumLaps = 3;
}