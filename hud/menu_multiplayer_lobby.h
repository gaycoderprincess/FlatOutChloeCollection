class CMenu_Multiplayer_Lobby : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_multiplayer_lobby"; }

	struct tPlayer {
		std::string name;
		int car;
		bool ready;
	} aPlayers[8] = {};

	int nHandling = HANDLING_NORMAL;
	int nTrackId = 1;

	virtual void Init() {

	}

	virtual void Process() {
		if (!bEnabled) return;
		if (!bIsInMultiplayer) return;


	}
} Menu_Multiplayer_Lobby;