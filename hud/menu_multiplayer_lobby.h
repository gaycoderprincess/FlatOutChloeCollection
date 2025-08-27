class CMenu_Multiplayer_Lobby : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_multiplayer_lobby"; }

	struct tPlayer {
		std::string name;
		int car = 1;
		bool ready = false;
		int ping = 0;
	} aPlayers[8] = {};
	int nNumPlayers = 0;

	struct tGameOption {
		std::string name;
		std::string value;
	};
	std::vector<tGameOption> aOptions;

	int nPlayerReadyX = 60;
	int nPlayerNameX = 260;
	int nPlayerCarX = 560;
	int nPlayerPingX = 860;

	int nPlayerStartY = 200;
	int nPlayerListSpacing = 48;
	float fPlayerListTextSize = 0.045;

	int nOptionX = 1150;
	int nOptionStartY = 200;
	int nOptionSpacing = 32;
	int nOptionSpacing2 = 48;
	float fOptionTextSize = 0.04;

	virtual void Init() {

	}

	void DrawPlayerInfo(int y, const std::string& ready, const std::string& name, const std::string& car, const std::string& ping) {
		tNyaStringData data;
		data.y = nPlayerStartY + y * nPlayerListSpacing;
		data.size = fPlayerListTextSize;
		if (y == 0) data.SetColor(GetPaletteColor(18));

		data.x = nPlayerReadyX;
		if (!ready.empty()) Draw1080pString(JUSTIFY_LEFT, data, ready, &DrawStringFO2_Ingame12);
		data.x = nPlayerNameX;
		Draw1080pString(JUSTIFY_LEFT, data, name, &DrawStringFO2_Ingame12);
		data.x = nPlayerCarX;
		Draw1080pString(JUSTIFY_LEFT, data, car, &DrawStringFO2_Ingame12);
		data.x = nPlayerPingX;
		Draw1080pString(JUSTIFY_LEFT, data, ping, &DrawStringFO2_Ingame12);
	}

	virtual void Process() {
		if (!bEnabled) return;
		if (!bIsInMultiplayer) return;

		int y = 0;
		DrawPlayerInfo(y++, "READY", "NAME", "CAR", "PING");
		for (auto& ply : aPlayers) {
			if (ply.name.empty()) continue;
			DrawPlayerInfo(y++, &ply == &aPlayers[0] ? "HOST" : (ply.ready ? "X" : ""), ply.name, GetCarName(ply.car), std::to_string(ply.ping));
		}

		tNyaStringData data;
		data.x = nOptionX;
		data.y = nOptionStartY;
		data.size = fOptionTextSize;
		for (auto& option : aOptions) {
			data.SetColor(GetPaletteColor(18));
			Draw1080pString(JUSTIFY_RIGHT, data, option.name, &DrawStringFO2_Ingame12);
			data.y += nOptionSpacing;
			data.SetColor(GetPaletteColor(17));
			Draw1080pString(JUSTIFY_RIGHT, data, option.value, &DrawStringFO2_Ingame12);
			data.y += nOptionSpacing2;
		}
	}
} Menu_Multiplayer_Lobby;