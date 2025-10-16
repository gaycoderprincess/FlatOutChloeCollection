class CMenu_ArcadeCareer : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_arcadecareer"; }

	int nCursorPos = 0;
	int nScrollPos = 0;
	bool bSelected = false;
	bool bCanMoveLeft = false;
	bool bCanMoveRight = true;

	void MoveLeft() override {
		if (!bCanMoveLeft) return;
		if (bSelected) return;
		nCursorPos--;
		if (nCursorPos < 0) nCursorPos = 0;
	}
	void MoveRight() override {
		if (!bCanMoveRight) return;
		if (bSelected) return;
		nCursorPos++;
		if (nCursorPos >= ArcadeMode::aArcadeRaces.size()) nCursorPos = ArcadeMode::aArcadeRaces.size()-1;
	}
	void MoveUp() override {
		if (bSelected) return;
		if (nCursorPos < 3) return;
		nCursorPos -= 3;
	}
	void MoveDown() override {
		if (bSelected) return;
		int pos = nCursorPos + 3;
		if (pos >= ArcadeMode::aArcadeRaces.size()) return;
		nCursorPos = pos;
	}

	void Init() override {
		PreloadTexture("data/menu/carnageoverlay_gold.tga"),
		PreloadTexture("data/menu/carnageoverlay_silver.tga"),
		PreloadTexture("data/menu/carnageoverlay_bronze.tga"),
		PreloadTexture("data/menu/carnageoverlay_author.tga"),
		PreloadTexture("data/menu/carnageoverlay_sauthor.tga"),
		PreloadTexture("data/menu/carnagebg_left.tga");
		PreloadTexture("data/menu/carnagebg_right.tga");
		PreloadTexture("data/menu/carnagebg_selected.tga");
		PreloadTexture("data/menu/track_icons.dds");
		PreloadTexture("data/menu/track_icons_inactive.dds");
		PreloadTexture("data/menu/track_icons_2.dds");
		PreloadTexture("data/menu/track_icons_2_inactive.dds");
		PreloadTexture("data/menu/common.dds");
	}

	void Reset() override {
		bSelected = false;
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

	int GetTrackId() const {
		return ArcadeMode::aArcadeRaces[nCursorPos].nLevel;
	}

	int GetCarId() const {
		return ArcadeMode::aArcadeRaces[nCursorPos].nCar;
	}

	auto GetHighlight() const {
		return &ArcadeMode::aArcadeRaces[nCursorPos];
	}

	int nArrowX = 280;
	int nArrowY1 = 265;
	int nArrowY2 = 795;
	int nArrowSizeX = 16;
	int nArrowSizeY = 10;

	constexpr static inline int nNumEventsOnScreen = (3*5);

	void DrawEventTitle() const {
		auto event = &ArcadeMode::aArcadeRaces[nCursorPos];
		tNyaStringData data;
		data.x = gLevelName.nPosX;
		data.y = gLevelName.nPosY;
		data.size = gLevelName.fSize;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, event->sName, &DrawStringFO2_Regular18);
		data.x = gLevelType.nPosX;
		data.y = gLevelType.nPosY;
		data.size = gLevelType.fSize;
		Draw1080pString(JUSTIFY_RIGHT, data, event->bIsFragDerby ? "FRAG DERBY" : (event->bIsArcadeRace ? "ARCADE RACE" : "DEMOLITION"), &DrawStringFO2_Regular18);
	}

	void ProcessSelected() const {
		static auto textureRight = LoadTextureFromBFS("data/menu/carnagebg_selected.tga");
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);
		DrawEventTitle();

		CMenu_CarDealer::ProcessSkinSelector();
	}

	void Process() override {
		ArcadeMode::pCurrentEvent = GetHighlight();
		ArcadeMode::nCurrentEventId = nCursorPos;

		static CNyaTimer gTimer;
		gTimer.Process();

		if (!bEnabled) return;
		if (bSelected) return ProcessSelected();

		static IDirect3DTexture9* texturePlacement[] = {
				LoadTextureFromBFS("data/menu/carnageoverlay_gold.tga"),
				LoadTextureFromBFS("data/menu/carnageoverlay_silver.tga"),
				LoadTextureFromBFS("data/menu/carnageoverlay_bronze.tga"),
				LoadTextureFromBFS("data/menu/carnageoverlay_author.tga"),
				LoadTextureFromBFS("data/menu/carnageoverlay_sauthor.tga"),
		};
		static auto textureLeft = LoadTextureFromBFS("data/menu/carnagebg_left.tga");
		static auto textureLeft2 = LoadTextureFromBFS("data/menu/carnagebg_left2.tga");
		static auto textureRight = LoadTextureFromBFS("data/menu/carnagebg_right.tga");
		static auto textureTracks = LoadTextureFromBFS("data/menu/track_icons.dds");
		static auto textureTracksLocked = LoadTextureFromBFS("data/menu/track_icons_inactive.dds");
		static auto textureTracks2 = LoadTextureFromBFS("data/menu/track_icons_2.dds");
		static auto textureTracks2Locked = LoadTextureFromBFS("data/menu/track_icons_2_inactive.dds");
		static auto textureCommon = LoadTextureFromBFS("data/menu/common.dds");
		static auto trackIcons = LoadHUDData("data/menu/track_icons.bed", "track_icons");
		static auto trackIcons2 = LoadHUDData("data/menu/track_icons_2.bed", "track_icons_2");
		static auto commonData = LoadHUDData("data/menu/common.bed", "common");

		auto event = &ArcadeMode::aArcadeRaces[nCursorPos];
		auto score = gCustomSave.aArcadeCareerScores[nCursorPos];
		bool showSAuthor = score >= event->nPlatinumScore && event->nCommunityScore > 0;

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, showSAuthor ? textureLeft2 : textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		CMenu_TrackSelect::DisplayTrackInfo(GetTrackId());

		static auto arrowLeft = GetHUDData(commonData, "nuoliylos");
		static auto arrowRight = GetHUDData(commonData, "nuolialas");
		Draw1080pSprite(JUSTIFY_LEFT, nArrowX - nArrowSizeX, nArrowX + nArrowSizeX, nArrowY1 - nArrowSizeY, nArrowY1 + nArrowSizeY, {255,255,255,255}, textureCommon, arrowLeft->min, arrowLeft->max);
		Draw1080pSprite(JUSTIFY_LEFT, nArrowX - nArrowSizeX, nArrowX + nArrowSizeX, nArrowY2 - nArrowSizeY, nArrowY2 + nArrowSizeY, {255,255,255,255}, textureCommon, arrowRight->min, arrowRight->max);

		while (nCursorPos < nScrollPos) {
			nScrollPos -= 3;
		}
		while (nCursorPos >= nScrollPos+nNumEventsOnScreen) {
			nScrollPos += 3;
		}

		auto totalScore = gCustomSave.GetArcadeCareerScore();
		int posX = 0;
		int posY = 0;
		for (int i = nScrollPos; i < nScrollPos+nNumEventsOnScreen; i++) {
			if (i < 0 || i >= ArcadeMode::aArcadeRaces.size()) continue;

			bCanMoveLeft = posX != 0;
			bCanMoveRight = posX != 2;

			auto event = &ArcadeMode::aArcadeRaces[i];
			auto score = gCustomSave.aArcadeCareerScores[i];
			bool unlocked = ArcadeMode::bAllUnlocked || totalScore >= event->nPointsToUnlock;
			int position = 0;
			if (score >= event->aGoalScores[2]) position = 3;
			if (score >= event->aGoalScores[1]) position = 2;
			if (score >= event->aGoalScores[0]) position = 1;
			if (score >= event->nPlatinumScore) position = 4;
			if (event->nCommunityScore > 0 && score >= event->nCommunityScore) position = 5;

			if (!event->bIsSmashySmash && score > gCustomSave.trackArcadeScores[event->nLevel]) {
				gCustomSave.trackArcadeScores[event->nLevel] = score;
			}

			if (position == 1 || position == 4 || position == 5) {
				gCustomSave.tracksWon[event->nLevel] = true;
			}
			
			auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(event->nLevel, "Image"));
			auto trackIconTexture = unlocked ? textureTracks : textureTracksLocked;
			if (!trackIcon) {
				trackIcon = GetHUDData(trackIcons2, GetTrackValueString(event->nLevel, "Image"));
				trackIconTexture = unlocked ? textureTracks2 : textureTracks2Locked;
				if (!trackIcon) {
					MessageBoxA(0, std::format("Failed to find image for track {}", event->nLevel).c_str(), "Fatal error", MB_ICONERROR);
				}
			}
			
			auto data = gEvent;
			float x1 = data.fPosX + data.fSpacingX * posX;
			float y1 = data.fPosY + data.fSpacingY * posY;
			float x2 = x1 + data.fSize * 1.5;
			float y2 = y1 + data.fSize;
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, trackIconTexture, 0, trackIcon->min, trackIcon->max);
			if (position >= 1 && position <= 5) {
				if (position == 4) {
					Achievements::AwardAchievement(GetAchievement("AUTHOR_MEDAL"));
				}
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, texturePlacement[position-1]);
			}
			if (i == nCursorPos) {
				auto rgb = GetPaletteColor(COLOR_MENU_YELLOW);
				rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;
				x2 = x1 + fEventHighlightSize * 1.5;
				y2 = y1 + fEventHighlightSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, rgb);
			}

			posX++;
			if (posX >= 3) {
				posX -= 3;
				posY++;
			}
		}

		bool unlocked = ArcadeMode::bAllUnlocked || totalScore >= event->nPointsToUnlock;
		int position = 0;
		if (score >= event->aGoalScores[2]) position = 3;
		if (score >= event->aGoalScores[1]) position = 2;
		if (score >= event->aGoalScores[0]) position = 1;
		if (score >= event->nPlatinumScore) position = 4;
		if (event->nCommunityScore > 0 && score >= event->nCommunityScore) position = 5;

		tNyaStringData data;
		data.x = gTargetScoresTitle.nPosX;
		data.y = gTargetScoresTitle.nPosY;
		data.size = gTargetScoresTitle.fSize;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_LEFT, data, "TARGET SCORES", &DrawStringFO2_Regular18);
		data.x = gYourScoreTitle.nPosX;
		data.y = gYourScoreTitle.nPosY;
		data.size = gYourScoreTitle.fSize;
		if (unlocked) {
			Draw1080pString(JUSTIFY_LEFT, data, "YOUR SCORE", &DrawStringFO2_Regular18);
		}
		else {
			auto data2 = data;
			data2.SetColor(GetPaletteColor(COLOR_MENU_RED));
			Draw1080pString(JUSTIFY_LEFT, data2, "LOCKED", &DrawStringFO2_Regular18);
		}

		data.x = gYourScore.nPosX;
		data.y = gYourScore.nPosY;
		data.size = gYourScore.fSize;
		if (unlocked) {
			auto data2 = data;
			data2.SetColor(GetMedalRGB(position));
			Draw1080pString(JUSTIFY_LEFT, data2, FormatScore(score), &DrawStringFO2_Regular18);
		}
		else {
			auto data2 = data;
			data2.y -= 16;
			data2.size = gYourScoreLocked.fSize;
			data2.SetColor(GetPaletteColor(COLOR_MENU_RED));
			Draw1080pString(JUSTIFY_LEFT, data2, std::format("{} POINTS\nTO UNLOCK", FormatScore(event->nPointsToUnlock)), &DrawStringFO2_Regular18);
		}

		for (int i = 0; i < 4; i++) {
			int targets[4] = {
				event->nPlatinumScore,
				event->aGoalScores[0],
				event->aGoalScores[1],
				event->aGoalScores[2],
			};
			int targetsSAuthor[4] = {
				event->nCommunityScore,
				event->nPlatinumScore,
				event->aGoalScores[0],
				event->aGoalScores[1],
			};

			int target = showSAuthor ? targetsSAuthor[i] : targets[i];
			data.x = gTargetScores.nPosX;
			data.y = gTargetScores.nPosY + gTargetScores.nSpacingY * i;
			data.size = gTargetScores.fSize;
			data.XCenterAlign = false;
			data.XRightAlign = true;
			Draw1080pString(JUSTIFY_LEFT, data, FormatScore(target), &DrawStringFO2_Regular18);
		}

		DrawEventTitle();
		data.x = gTotalScore.nPosX;
		data.y = gTotalScore.nPosY;
		data.size = gTotalScore.fSize;
		data.XRightAlign = false;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, std::format("TOTAL SCORE: {}", FormatScore(totalScore)), &DrawStringFO2_Regular18);
	}
} Menu_ArcadeCareer;