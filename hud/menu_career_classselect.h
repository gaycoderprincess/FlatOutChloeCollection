class CMenu_CareerClassSelect : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_career_classselect"; }

	virtual void MoveUp() {
		gCustomSave.nCareerClass--;
		if (gCustomSave.nCareerClass < 1) gCustomSave.nCareerClass = 3;
	}
	virtual void MoveDown() {
		gCustomSave.nCareerClass++;
		if (gCustomSave.nCareerClass > 3) gCustomSave.nCareerClass = 1;
	}

	static constexpr int nHighlightX = 590;
	static constexpr int nHighlightY = 310;
	static constexpr int nHighlightSizeX = 396;
	static constexpr int nHighlightSizeY = 115;
	static constexpr int nHighlightSpacing = 145;

	static constexpr tDrawPositions1080p gTitle = {1320, 190, 0.04};
	static constexpr tDrawPositions1080p gDescription = {1397, 435, 0.04, 0, 45};

	static std::string GetClassDescription(int classId) {
		const char* descriptions[] = {
				"Derby class description",
				"Race class description",
				"Street class description",
		};

		int cupsCompleted = 0;
		int eventsCompleted = 0;
		auto careerClass = &CareerMode::aLUACareerClasses[classId];
		for (int i = 0; i < careerClass->aCups.size(); i++) {
			auto cup = &gCustomSave.aCareerClasses[classId].aCups[i];
			if (cup->nPosition >= 1 && cup->nPosition <= 3) cupsCompleted++;
		}
		for (int i = 0; i < careerClass->aEvents.size(); i++) {
			auto cup = &gCustomSave.aCareerClasses[classId].aEvents[i];
			if (cup->nPosition >= 1 && cup->nPosition <= 3) eventsCompleted++;
		}
		auto cup = &gCustomSave.aCareerClasses[classId].Finals;
		if (cup->nPosition >= 1 && cup->nPosition <= 3) cupsCompleted++;

		//std::string str = descriptions[classId];
		auto str = std::format("Cups: {}/{}\nEvents: {}/{}", cupsCompleted, careerClass->aCups.size()+1, eventsCompleted, careerClass->aEvents.size());
		return str;
	}

	virtual void Init() {
		PreloadTexture("data/menu/classicon_1.png");
		PreloadTexture("data/menu/classicon_2.png");
		PreloadTexture("data/menu/classicon_3.png");
		PreloadTexture("data/menu/classicon_1_locked.png");
		PreloadTexture("data/menu/classicon_2_locked.png");
		PreloadTexture("data/menu/classicon_3_locked.png");
		PreloadTexture("data/menu/classselect_bg_left.png");
		PreloadTexture("data/menu/classselect_bg_right.png");
	}

	virtual void Process() {
		static CNyaTimer gTimer;
		gTimer.Process();

		if (!bEnabled) return;

		static IDirect3DTexture9* textureClass[3] = {
				LoadTextureFromBFS("data/menu/classicon_1.png"),
				LoadTextureFromBFS("data/menu/classicon_2.png"),
				LoadTextureFromBFS("data/menu/classicon_3.png"),
		};
		static IDirect3DTexture9* textureClassLocked[3] = {
				LoadTextureFromBFS("data/menu/classicon_1_locked.png"),
				LoadTextureFromBFS("data/menu/classicon_2_locked.png"),
				LoadTextureFromBFS("data/menu/classicon_3_locked.png"),
		};
		static auto textureLeft = LoadTextureFromBFS("data/menu/classselect_bg_left.png");
		static auto textureRight = LoadTextureFromBFS("data/menu/classselect_bg_right.png");

		Menu_CareerCupSelect.nCursorX = 0;
		Menu_CareerCupSelect.nCursorY = 0;
		if (gCustomSave.nCareerClass < 1) gCustomSave.nCareerClass = 1;

		for (int i = 0; i < 3; i++) {
			float x1 = nHighlightX;
			float y1 = nHighlightY + (nHighlightSpacing * i);
			float x2 = x1 + nHighlightSizeX;
			float y2 = y1 + nHighlightSizeY;
			Draw1080pSprite(JUSTIFY_LEFT, x1, x2, y1, y2, {255, 255, 255, 255}, gCustomSave.bCareerClassUnlocked[i] ? textureClass[i] : textureClassLocked[i]);
		}

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		auto rgb = GetPaletteColor(COLOR_MENU_YELLOW);
		rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;

		float x1 = nHighlightX;
		float y1 = nHighlightY + (nHighlightSpacing * (gCustomSave.nCareerClass-1));
		float x2 = x1 + nHighlightSizeX;
		float y2 = y1 + nHighlightSizeY;
		Draw1080pSprite(JUSTIFY_LEFT, x1, x2, y1, y2, rgb, nullptr);

		Menu_CareerCupSelect.nClass = gCustomSave.nCareerClass-1;

		const char* classNames[] = {
				"BRONZE CLASS",
				"SILVER CLASS",
				"GOLD CLASS",
		};

		tNyaStringData data;
		data.x = gTitle.nPosX;
		data.y = gTitle.nPosY;
		data.size = gTitle.fSize;
		Draw1080pString(JUSTIFY_RIGHT, data, classNames[Menu_CareerCupSelect.nClass], &DrawStringFO2_Small);
		data.x = gDescription.nPosX;
		data.y = gDescription.nPosY;
		data.size = gDescription.fSize;
		Draw1080pString(JUSTIFY_RIGHT, data, GetClassDescription(Menu_CareerCupSelect.nClass), &DrawStringFO2_Ingame12);
	}
} Menu_CareerClassSelect;