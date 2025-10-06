class CHUD_Tutorial : public CIngameHUDElement {
public:

	tDrawPositions1080p gGamemodeLabel = {50,124,0.035};
	tDrawPositions1080p gGamemodeDescription = {50,220,0.04};

	void Init() override {
		nHUDLayer = eHUDLayer::OVERLAY;
	}

	bool bMenuUpInNormalMode = false;

	void Process() override {
		if (pPlayerHost->nRaceTime != -3000) return;
		if (!bIsInMultiplayer) {
			if (bIsCarnageRace || bIsSmashyRace) bMenuUpInNormalMode = true;
		}
		if (!bIsArcadeMode && !bMenuUpInNormalMode) return;
		bMenuUpInNormalMode = false;

		std::string gamemodeName, gamemodeDescription;
		if (bIsCarnageRace) {
			gamemodeName = "ARCADE RACE";
			gamemodeDescription = "Ultimate destruction is the name of the game.\nEarn points by wrecking competitors, destroying scenery, catching huge air and passing checkpoints.\nPassing checkpoints will also give you extra time.\nYour position gives you a score multiplier that affects everything, so try to keep in the lead.";
		}
		else if (bIsSmashyRace) {
			gamemodeName = "DEMOLITION";
			gamemodeDescription = "Cause extreme amounts of vandalism as quickly as possible!\nEarn points by crashing into as much scenery as you can before the time runs out.\nSome objects give more points than others.\nTrack limits are a suggestion, search for high-reward objects and larger groups if you can.";
		}
		else if (bIsWreckingDerby) {
			gamemodeName = "WRECKING DERBY";
			gamemodeDescription = "You must win by causing as much destruction as possible.\nYou get bonus points for surviving in the top 3, so try to stay alive.\nYour nitro meter indicates the damage your car receives, though you can still use nitro if you have any.\nYou lose if the damage reaches the top of the meter.";
		}
		else if (bIsFragDerby) {
			gamemodeName = "FRAG DERBY";
			gamemodeDescription = "Frag derbies are about going for the kill. Wreck opponents to earn points.\nGet the most frags with the current life to be the Frag Streaker and earn double scores.\nSurvive the longest with the current life to earn extra score as the Survivor.";
		}
		else if (pGameFlow->nEventType == eEventType::DERBY) {
			gamemodeName = "SURVIVOR DERBY";
			gamemodeDescription = "You can only win by destroying all the other cars.\nYour nitro meter indicates the damage your car receives.\nYou lose if the damage reaches the top of the meter.";
		}

		DrawRectangle(0, 1, 0, 1, {0,0,0,200}); // background box

		static auto texture = LoadTextureFromBFS("data/global/overlay/raceresult_balk2.tga");
		DrawRectangle(0, 1, 99 / 1080.0, (99 + 72) / 1080.0, {255,255,255,255}, 0, texture);
		tNyaStringData data;
		data.x = gGamemodeLabel.nPosX;
		data.y = gGamemodeLabel.nPosY;
		data.size = gGamemodeLabel.fSize;
		data.SetColor(GetPaletteColor(COLOR_INGAMEMENU_TITLE));
		Draw1080pString(JUSTIFY_LEFT, data, gamemodeName, &DrawStringFO2_Italic24);
		data.x = gGamemodeDescription.nPosX;
		data.y = gGamemodeDescription.nPosY;
		data.size = gGamemodeDescription.fSize;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_LEFT, data, gamemodeDescription, &DrawStringFO2_Regular10);
		data.x = 960;
		data.y = 960;
		data.size = 0.035;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_CENTER, data, "Press START button", &DrawStringFO2_Italic24);
	}
} HUD_Tutorial;