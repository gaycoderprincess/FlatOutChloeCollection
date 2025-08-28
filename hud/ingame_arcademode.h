class CHUD_ArcadeMode : public CIngameHUDElement {
public:

	tDrawPositions1080p gCheckpointBonus = {240, 85, 0.03};

	double fEarnPopupTime = 0;
	std::string sEarnPopupString;
	NyaDrawing::CNyaRGBA32 nEarnPopupRGB;

	static NyaDrawing::CNyaRGBA32 GetMedalRGB(int medal) {
		switch (medal) {
			case 1:
				return {236,221,16, 255};
			case 2:
				return {186,186,186, 255};
			case 3:
				return {175, 100, 0, 255};
			case 4:
				return {30,160,9, 255};
			default:
				return {255,255,255,255};
		}
	}

	void TriggerEarnPopup(const std::string& str, int medal) {
		fEarnPopupTime = 4;
		sEarnPopupString = str;
		nEarnPopupRGB = GetMedalRGB(medal);

		static auto sound = NyaAudio::LoadFile("data/sound/trophy.wav");
		if (sound) {
			NyaAudio::SetVolume(sound, nIngameSfxVolume / 100.0);
			NyaAudio::Play(sound);
		}
	}

	tDrawPositions1080p gGamemodeLabel = {50,124,0.035};
	tDrawPositions1080p gGamemodeDescription = {50,220,0.035};

	void DrawTutorial() {
		std::string gamemodeName, gamemodeDescription;
		if (bIsCarnageRace) {
			gamemodeName = "ARCADE RACE";
			gamemodeDescription = "Ultimate destruction is the name of the game.\nEarn points by wrecking competitors, destroying scenery, catching huge air and passing checkpoints.\nPassing checkpoints will also give you extra time.\nYour position gives you a score multiplier that affects everything, so try to keep in the lead.";
		}
		else if (bIsSmashyRace) {
			gamemodeName = "DEMOLITION";
			gamemodeDescription = "Cause extreme amounts of vandalism as quickly as possible!\nEarn points by crashing into as much scenery as you can before the time runs out.\nSome objects give more points than others, search for high-reward objects and larger groups if you can!";
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
		Draw1080pString(JUSTIFY_LEFT, data, "Press START button", &DrawStringFO2_Small);
	}

	virtual void Process() {
		if (!IsRaceHUDUp()) return;
		if (!ArcadeMode::bIsArcadeMode) return;

		if (pPlayerHost->nRaceTime == -3000) DrawTutorial();

		auto event = ArcadeMode::pCurrentEvent;

		static int lastScore = 0;
		auto score = ArcadeMode::nCurrentEventScore;
		if (lastScore < event->aGoalScores[2] && score >= event->aGoalScores[2]) {
			TriggerEarnPopup("BRONZE EARNED", 3);
		}
		if (lastScore < event->aGoalScores[1] && score >= event->aGoalScores[1]) {
			TriggerEarnPopup("SILVER EARNED", 2);
		}
		if (lastScore < event->aGoalScores[0] && score >= event->aGoalScores[0]) {
			TriggerEarnPopup("GOLD EARNED", 1);
		}
		if (lastScore < event->nPlatinumScore && score >= event->nPlatinumScore) {
			TriggerEarnPopup("AUTHOR SCORE BEATEN", 4);
		}
		lastScore = score;

		NyaDrawing::CNyaRGBA32 rgb = GetMedalRGB(3);
		uint32_t targetScore = event->aGoalScores[2]; // bronze
		std::string scoreTitle = "TARGET";
		if (score >= targetScore) { // silver
			targetScore = event->aGoalScores[1];
			rgb = GetMedalRGB(2);
		}
		if (score >= targetScore) { // gold
			targetScore = event->aGoalScores[0];
			rgb = GetMedalRGB(1);
		}
		if (score >= targetScore && event->nPlatinumScore > 0) { // author
			targetScore = event->nPlatinumScore;
			rgb = GetMedalRGB(4);
		}
		if (score >= targetScore && gCustomSave.aArcadeCareerScores[ArcadeMode::nCurrentEventId] > event->nPlatinumScore) { // PB
			targetScore = gCustomSave.aArcadeCareerScores[ArcadeMode::nCurrentEventId];
			rgb = {255,255,255,255};
			scoreTitle = "PERSONAL BEST";
		}

		int uiId = 1;
		if (bIsCarnageRace) uiId = 2;
		DrawElement(uiId, scoreTitle, FormatScore(targetScore), rgb);

		static CNyaTimer gTimer;
		gTimer.Process();
		if (fEarnPopupTime > 0) {
			auto rgb = nEarnPopupRGB;
			if (fEarnPopupTime < 0.5) rgb.a = std::lerp(0, 255, fEarnPopupTime * 2);
			if (fEarnPopupTime > 3.75) rgb.a = std::lerp(0, 255, (4 - fEarnPopupTime) * 4);
			DrawElementCenter(2.5, "CONGRATULATIONS!", sEarnPopupString, rgb);
			fEarnPopupTime -= gTimer.fDeltaTime;
		}
	}
} HUD_ArcadeMode;