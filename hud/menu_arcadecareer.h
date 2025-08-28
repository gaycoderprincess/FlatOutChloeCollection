class CMenu_ArcadeCareer : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_arcadecareer"; }

	int nCursorPos = 0;

	virtual void MoveLeft() {
		nCursorPos--;
		if (nCursorPos < 0) nCursorPos = 0;
	}
	virtual void MoveRight() {
		nCursorPos++;
		if (nCursorPos >= ArcadeMode::aArcadeRaces.size()) nCursorPos = ArcadeMode::aArcadeRaces.size()-1;
	}
	virtual void MoveUp() {
		if (nCursorPos < 3) return;
		nCursorPos -= 3;
	}
	virtual void MoveDown() {
		int pos = nCursorPos + 3;
		if (pos >= ArcadeMode::aArcadeRaces.size()) return;
		nCursorPos = pos;
	}

	virtual void Init() {
		PreloadTexture("data/menu/carnagebg_left.png");
		PreloadTexture("data/menu/carnagebg_right.png");
		PreloadTexture("data/menu/track_icons.dds");
		PreloadTexture("data/menu/track_icons_inactive.dds");
		PreloadTexture("data/menu/common.dds");
	}

	tDrawPositions gEvent = {0.05, 0.26, 0.1, 0.14, 0.09};
	float fEventHighlightSize = 0.094;

	tDrawPositions1080p gLevelName = {1445, 163, 0.035};
	tDrawPositions1080p gLevelType = {1445, 214, 0.025};
	tDrawPositions1080p gTotalScore = {1445, 818, 0.04};
	tDrawPositions1080p gTargetScoresTitle = {680, 240, 0.03};
	tDrawPositions1080p gTargetScores = {805, 307, 0.03, 0, 79};
	tDrawPositions1080p gYourScoreTitle = {680, 640, 0.03};
	tDrawPositions1080p gYourScore = {680, 710, 0.04};
	tDrawPositions1080p gYourScoreLocked = {680, 710, 0.03};

	int GetTrackId() {
		return ArcadeMode::aArcadeRaces[nCursorPos].nLevel;
	}

	int GetCarId() {
		return ArcadeMode::aArcadeRaces[nCursorPos].nCar;
	}

	auto GetHighlight() {
		return &ArcadeMode::aArcadeRaces[nCursorPos];
	}

	virtual void Process() {
		ArcadeMode::pCurrentEvent = GetHighlight();
		ArcadeMode::nCurrentEventId = nCursorPos;

		static CNyaTimer gTimer;
		gTimer.Process();

		if (!bEnabled) return;

		static auto textureLeft = LoadTextureFromBFS("data/menu/carnagebg_left.png");
		static auto textureRight = LoadTextureFromBFS("data/menu/carnagebg_right.png");
		static auto textureTracks = LoadTextureFromBFS("data/menu/track_icons.dds");
		static auto textureTracks2 = LoadTextureFromBFS("data/menu/track_icons_inactive.dds");
		static auto textureCommon = LoadTextureFromBFS("data/menu/common.dds");
		static auto trackIcons = LoadHUDData("data/menu/track_icons.bed", "track_icons");
		static auto commonData = LoadHUDData("data/menu/common.bed", "common");

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		CMenu_TrackSelect::DisplayTrackInfo(GetTrackId());

		auto totalScore = gCustomSave.GetArcadeCareerScore();

		for (int i = 0; i < ArcadeMode::aArcadeRaces.size(); i++) {
			auto event = &ArcadeMode::aArcadeRaces[i];
			bool unlocked = totalScore >= event->nPointsToUnlock;
			auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(event->nLevel, "Image"));
			if (!trackIcon) {
				MessageBoxA(0, std::format("Failed to find image for track {}", event->nLevel).c_str(), "Fatal error", MB_ICONERROR);
			}

			int posX = i;
			int posY = 0;
			while (posX >= 3) {
				posX -= 3;
				posY++;
			}

			auto data = gEvent;
			float x1 = data.fPosX + data.fSpacingX * posX;
			float y1 = data.fPosY + data.fSpacingY * posY;
			float x2 = x1 + data.fSize * 1.5;
			float y2 = y1 + data.fSize;
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, unlocked ? textureTracks : textureTracks2, 0, trackIcon->min, trackIcon->max);
			if (i == nCursorPos) {
				auto rgb = GetPaletteColor(18);
				rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;
				x2 = x1 + fEventHighlightSize * 1.5;
				y2 = y1 + fEventHighlightSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, rgb);
			}
		}

		auto event = &ArcadeMode::aArcadeRaces[nCursorPos];
		auto score = gCustomSave.aArcadeCareerScores[nCursorPos];
		bool unlocked = totalScore >= event->nPointsToUnlock;
		int position = 0;
		if (score >= event->aGoalScores[2]) position = 3;
		if (score >= event->aGoalScores[1]) position = 2;
		if (score >= event->aGoalScores[0]) position = 1;
		if (score >= event->nPlatinumScore) position = 4;

		tNyaStringData data;
		data.x = gTargetScoresTitle.nPosX;
		data.y = gTargetScoresTitle.nPosY;
		data.size = gTargetScoresTitle.fSize;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_LEFT, data, "TARGET SCORES", &DrawStringFO2_Small);
		data.x = gYourScoreTitle.nPosX;
		data.y = gYourScoreTitle.nPosY;
		data.size = gYourScoreTitle.fSize;
		if (unlocked) {
			Draw1080pString(JUSTIFY_LEFT, data, "YOUR SCORE", &DrawStringFO2_Small);
		}
		else {
			auto data2 = data;
			data2.SetColor(GetPaletteColor(22));
			Draw1080pString(JUSTIFY_LEFT, data2, "LOCKED", &DrawStringFO2_Small);
		}

		data.x = gYourScore.nPosX;
		data.y = gYourScore.nPosY;
		data.size = gYourScore.fSize;
		if (unlocked) {
			auto data2 = data;
			data2.SetColor(CHUD_ArcadeMode::GetMedalRGB(position));
			Draw1080pString(JUSTIFY_LEFT, data2, FormatScore(score), &DrawStringFO2_Small);
		}
		else {
			auto data2 = data;
			data2.y -= 16;
			data2.size = gYourScoreLocked.fSize;
			data2.SetColor(GetPaletteColor(22));
			Draw1080pString(JUSTIFY_LEFT, data2, std::format("{} POINTS\nTO UNLOCK", FormatScore(event->nPointsToUnlock)), &DrawStringFO2_Small);
		}

		for (int i = 0; i < 4; i++) {
			int target = i == 0 ? event->nPlatinumScore : event->aGoalScores[i-1];
			data.x = gTargetScores.nPosX;
			data.y = gTargetScores.nPosY + gTargetScores.nSpacingY * i;
			data.size = gTargetScores.fSize;
			data.XCenterAlign = false;
			data.XRightAlign = true;
			Draw1080pString(JUSTIFY_LEFT, data, FormatScore(target), &DrawStringFO2_Small);
		}

		data.x = gLevelName.nPosX;
		data.y = gLevelName.nPosY;
		data.size = gLevelName.fSize;
		data.XRightAlign = false;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, event->sName, &DrawStringFO2_Small);
		data.x = gLevelType.nPosX;
		data.y = gLevelType.nPosY;
		data.size = gLevelType.fSize;
		Draw1080pString(JUSTIFY_RIGHT, data, event->bIsArcadeRace ? "ARCADE RACE" : "DEMOLITION", &DrawStringFO2_Small);
		data.x = gTotalScore.nPosX;
		data.y = gTotalScore.nPosY;
		data.size = gTotalScore.fSize;
		Draw1080pString(JUSTIFY_RIGHT, data, std::format("TOTAL SCORE: {}", FormatScore(totalScore)), &DrawStringFO2_Small);
	}
} Menu_ArcadeCareer;