class CHUD_ArcadeTutorial : public CIngameHUDElement {
public:

	tDrawPositions1080p gGamemodeLabel = {50,124,0.035};
	tDrawPositions1080p gGamemodeDescription = {50,220,0.035};

	virtual void Init() {
		nHUDLayer = eHUDLayer::OVERLAY;
	}

	virtual void Process() {
		if (!IsRaceHUDUp()) return;
		if (!ArcadeMode::bIsArcadeMode) return;
		if (pPlayerHost->nRaceTime != -3000) return;

		std::string gamemodeName, gamemodeDescription;
		if (bIsCarnageRace) {
			gamemodeName = "ARCADE RACE";
			gamemodeDescription = "Ultimate destruction is the name of the game.\nEarn points by wrecking competitors, destroying scenery, catching huge air and passing checkpoints.\nPassing checkpoints will also give you extra time.\nYour position gives you a score multiplier that affects everything, so try to keep in the lead.";
		}
		else if (bIsSmashyRace) {
			gamemodeName = "DEMOLITION";
			gamemodeDescription = "Cause extreme amounts of vandalism as quickly as possible!\nEarn points by crashing into as much scenery as you can before the time runs out.\nSome objects give more points than others, search for high-reward objects and larger groups if you can, track limits are a suggestion!";
		}

		DrawRectangle(0, 1, 0, 1, {0,0,0,200}); // background box

		static auto texture = LoadTextureFromBFS("data/global/overlay/raceresult_balk2.png");
		DrawRectangle(0, 1, 99 / 1080.0, (99 + 72) / 1080.0, {255,255,255,255}, 0, texture);
		tNyaStringData data;
		data.x = gGamemodeLabel.nPosX;
		data.y = gGamemodeLabel.nPosY;
		data.size = gGamemodeLabel.fSize;
		data.SetColor(GetPaletteColor(13));
		Draw1080pString(JUSTIFY_LEFT, data, gamemodeName, &DrawStringFO2_Small);
		data.x = gGamemodeDescription.nPosX;
		data.y = gGamemodeDescription.nPosY;
		data.size = gGamemodeDescription.fSize;
		data.SetColor(GetPaletteColor(17));
		Draw1080pString(JUSTIFY_LEFT, data, gamemodeDescription, &DrawStringFO2_Ingame12);
		data.x = 960;
		data.y = 960;
		data.size = 0.035;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_CENTER, data, "Press START button", &DrawStringFO2_Small);
	}
} HUD_ArcadeTutorial;