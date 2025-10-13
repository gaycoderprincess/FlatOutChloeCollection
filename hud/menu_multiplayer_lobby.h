class CMenu_Multiplayer_Lobby : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_multiplayer_lobby"; }

	struct tPlayer {
		std::string name;
		int car = 1;
		std::string iconName;
		std::string icon2Name;
		int ping = 0;
		std::string carTitle;
		std::string flagName;
		int colorId = 0;
	} aPlayers[nMaxPlayers] = {};
	int nNumPlayersReady = 0;
	int nNumPlayers = 0;

	struct tGameOption {
		std::string name;
		std::string value;
	};
	std::vector<tGameOption> aOptions;

	int nTrackId = 1;

	int nPlayerReadyX = 60;
	int nPlayerNameX = 260;
	int nPlayerCarX = 560;
	int nPlayerPingX = 860;

	int nPlayerStartY = 220;
	int nPlayerListSpacing = 55;
	float fPlayerListTextSize = 0.042;

	int nOptionX = 1095;
	int nOptionStartY = 219;
	int nOptionSpacing = 30;
	int nOptionSpacing2 = 39;
	float fOptionTextSize = 0.04;

	float fTrackPreviewX = -0.149;
	int nTrackPreviewX = 160;

	int nFlagX = 215;
	int nFlagYOffset = 3;
	int nFlagSize = 16;

	int nIconX = 80;
	int nIconYOffset = 2;
	int nIconSize = 16;

	int nIcon2X = 141;

	void Init() override {
		PreloadTexture("data/menu/mpmenubg.tga");
		PreloadTexture("data/menu/flags.tga");
		PreloadTexture("data/menu/multiplayer.tga");
	}

	void DrawPlayerInfo(int y, const std::string& ready, const std::string& name, const std::string& car, const std::string& ping, int colorId = 0) const {
		tNyaStringData data;
		data.y = nPlayerStartY + y * nPlayerListSpacing;
		data.size = fPlayerListTextSize;
		if (y == 0) data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));

		data.x = nPlayerReadyX;
		if (!ready.empty()) Draw1080pString(JUSTIFY_LEFT, data, ready, &DrawStringFO2_Condensed12);
		data.x = nPlayerCarX;
		Draw1080pString(JUSTIFY_LEFT, data, car, &DrawStringFO2_Condensed12);
		data.x = nPlayerPingX;
		Draw1080pString(JUSTIFY_LEFT, data, ping, &DrawStringFO2_Condensed12);
		data.x = nPlayerNameX;
		if (y == 0) data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		else {
			data.SetColor(aPlayerColorsMultiplayer[colorId]);
			data.a = 255;
		}
		Draw1080pString(JUSTIFY_LEFT, data, name, &DrawStringFO2_Condensed12);
	}

	void DrawPlayerFlag(int y, const std::string& flagName) const {
		static auto textureFlags = LoadTextureFromBFS("data/menu/flags.tga");
		static auto dataFlags = LoadHUDData("data/menu/flags.bed", "flags");

		auto data = GetHUDData(dataFlags, flagName);
		if (!data) data = GetHUDData(dataFlags, "playerflag_0");
		if (!data) return;

		auto posY = nPlayerStartY + y * nPlayerListSpacing;
		posY += nFlagYOffset;
		Draw1080pSprite(JUSTIFY_LEFT, nFlagX - nFlagSize * 2, nFlagX + nFlagSize * 2, posY - nFlagSize, posY + nFlagSize, {255,255,255,255}, textureFlags, data->min, data->max);
	}

	void DrawPlayerIcon(int y, const std::string& iconName) const {
		if (iconName.empty()) return;

		static auto textureFlags = LoadTextureFromBFS("data/menu/multiplayer.tga");
		static auto dataFlags = LoadHUDData("data/menu/multiplayer.bed", "multiplayer");

		auto data = GetHUDData(dataFlags, iconName);
		if (!data) return;

		auto posY = nPlayerStartY + y * nPlayerListSpacing;
		posY += nIconYOffset;
		Draw1080pSprite(JUSTIFY_LEFT, nIconX - nIconSize, nIconX + nIconSize, posY - nIconSize, posY + nIconSize, {255,255,255,255}, textureFlags, data->min, data->max);
	}

	void DrawPlayerIcon2(int y, const std::string& iconName) const {
		if (iconName.empty()) return;

		static auto textureFlags = LoadTextureFromBFS("data/menu/multiplayer.tga");
		static auto dataFlags = LoadHUDData("data/menu/multiplayer.bed", "multiplayer");

		auto data = GetHUDData(dataFlags, iconName);
		if (!data) return;

		auto posY = nPlayerStartY + y * nPlayerListSpacing;
		posY += nIconYOffset;
		Draw1080pSprite(JUSTIFY_LEFT, nIcon2X - nIconSize, nIcon2X + nIconSize, posY - nIconSize, posY + nIconSize, {255,255,255,255}, textureFlags, data->min, data->max);
	}

	void Process() override {
		if (!bEnabled) return;
		if (!bIsInMultiplayer) return;

		static auto textureRight = LoadTextureFromBFS("data/menu/mpmenubg.tga");
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		CMenu_TrackSelect::DisplayTrackInfo(nTrackId, fTrackPreviewX, nTrackPreviewX);

		int y = 0;
		DrawPlayerInfo(y++, std::format("{}/{} READY", nNumPlayersReady, nNumPlayers), "NAME", "CAR", "PING");
		for (auto& ply : aPlayers) {
			if (ply.name.empty()) continue;
			DrawPlayerFlag(y, ply.flagName);
			DrawPlayerIcon(y, ply.iconName);
			DrawPlayerIcon2(y, ply.icon2Name);
			DrawPlayerInfo(y++, "", ply.name, ply.carTitle.empty() ? GetCarName(ply.car) : ply.carTitle, std::to_string(ply.ping), ply.colorId);
		}

		tNyaStringData data;
		data.x = nOptionX;
		data.y = nOptionStartY;
		data.size = fOptionTextSize;
		for (auto& option : aOptions) {
			data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
			Draw1080pString(JUSTIFY_RIGHT, data, option.name, &DrawStringFO2_Condensed12);
			data.y += nOptionSpacing;
			data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
			Draw1080pString(JUSTIFY_RIGHT, data, option.value, &DrawStringFO2_Condensed12);
			data.y += nOptionSpacing2;
		}
	}
} Menu_Multiplayer_Lobby;