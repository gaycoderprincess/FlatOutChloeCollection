class CMenu_CareerCupSelect : public CMenuHUDElement {
public:
	virtual const char* GetName() { return "menu_career_cupselect"; }

	static constexpr tDrawPositions gEvent = {0.15, 0.29, 0.1, 0.19, 0.135};
	static constexpr float fEventHighlightSize = 0.094;

	static constexpr tDrawPositions1080p gCupName = {1320, 193, 0.03};
	static constexpr int nLapsX[10] = {
			1766, // 0
			1769, // 1
			1766, // 2
			1766, // 3
			1761, // 4
			1763, // 5
			1764, // 6
			1766, // 7
			1766, // 8
			1766, // 9
	};
	static constexpr int nLapsY = 323;
	static constexpr float fLapsSize = 0.03;
	static constexpr tDrawPositions1080p gEvents = {1530, 435, 0.04, 0, 45};
	static constexpr tDrawPositions1080p gEventsTitle = {1505, 285, 0.04};

	bool bCursorJustMoved = false;

	int nClass = 0;
	int nCursorX = 0;
	int nCursorY = 0;
	int GetCursorLimitX() {
		if (nCursorY == 2) {
			return CareerMode::aLUACareerClasses[nClass].aEvents.size();
		}
		return CareerMode::aLUACareerClasses[nClass].aCups.size();
	}

	virtual void MoveLeft() {
		if (nCursorY == 1) return;
		bCursorJustMoved = true;
		nCursorX--;
		if (nCursorX < 0) nCursorX = 0;
	}
	virtual void MoveRight() {
		if (nCursorY == 1) return;
		bCursorJustMoved = true;
		nCursorX++;
		if (nCursorX >= GetCursorLimitX()) nCursorX = GetCursorLimitX()-1;
	}
	virtual void MoveUp() {
		bCursorJustMoved = true;
		nCursorY--;
		if (nCursorY < 0) nCursorY = 0;
	}
	virtual void MoveDown() {
		bCursorJustMoved = true;
		nCursorY++;
		if (nCursorY > 2) nCursorY = 2;
	}

	static constexpr tDrawPositions gTrackPlacements = {0.263, 0.353, 0.05, 0.19, 0.135};

	virtual void Init() {
		PreloadTexture("data/menu/cup_bronze_bg.png");
		PreloadTexture("data/menu/cup_silver_bg.png");
		PreloadTexture("data/menu/cup_gold_bg.png");
		PreloadTexture("data/menu/cupselect_bg_right.png");
		PreloadTexture("data/menu/track_icons.dds");
		PreloadTexture("data/menu/track_icons_inactive.dds");
		PreloadTexture("data/menu/common.dds");
	}

	virtual void Process() {
		static CNyaTimer gTimer;
		gTimer.Process();

		if (!bEnabled) return;

		static IDirect3DTexture9* textureLeft[3] = {
				LoadTextureFromBFS("data/menu/cup_bronze_bg.png"),
				LoadTextureFromBFS("data/menu/cup_silver_bg.png"),
				LoadTextureFromBFS("data/menu/cup_gold_bg.png"),
		};
		static auto textureRight = LoadTextureFromBFS("data/menu/cupselect_bg_right.png");
		static auto textureTracks = LoadTextureFromBFS("data/menu/track_icons.dds");
		static auto textureTracks2 = LoadTextureFromBFS("data/menu/track_icons_inactive.dds");
		static auto texturePlacements = LoadTextureFromBFS("data/menu/common.dds");
		static auto trackIcons = LoadHUDData("data/menu/track_icons.bed", "track_icons");
		static auto trackPlacements = LoadHUDData("data/menu/common.bed", "common");
		static std::string sEventDescription;

		if (nCursorX >= GetCursorLimitX()) nCursorX = 0;

		Draw1080pSprite(JUSTIFY_LEFT, 0, 1920, 0, 1080, {255,255,255,255}, textureLeft[nClass]);
		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		const char* trackPlacementNames[] = {
				"first",
				"second",
				"third",
		};

		auto careerClass = &CareerMode::aLUACareerClasses[nClass];
		auto careerSaveClass = &gCustomSave.aCareerClasses[nClass];
		for (int i = 0; i < careerClass->aCups.size(); i++) {
			auto cup = &careerClass->aCups[i];
			auto cupSave = &careerSaveClass->aCups[i];
			auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(cup->aRaces[0].nLevel, "Image"));
			if (!trackIcon) {
				MessageBoxA(0, std::format("Failed to find image for track {}", cup->aRaces[0].nLevel).c_str(), "Fatal error", MB_ICONERROR);
			}
			auto data = gEvent;
			float x1 = data.fPosX + data.fSpacingX * i;
			float y1 = data.fPosY;
			float x2 = x1 + data.fSize * 1.5;
			float y2 = y1 + data.fSize;
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, cupSave->bUnlocked ? textureTracks : textureTracks2, 0, trackIcon->min, trackIcon->max);
			if (i == nCursorX && nCursorY == 0) {
				auto rgb = GetPaletteColor(COLOR_MENU_YELLOW);
				rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;
				x2 = x1 + fEventHighlightSize * 1.5;
				y2 = y1 + fEventHighlightSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, rgb);
			}

			if (cupSave->nPosition >= 1 && cupSave->nPosition <= 3) {
				data = gTrackPlacements;
				auto hud = GetHUDData(trackPlacements, trackPlacementNames[cupSave->nPosition-1]);
				x1 = data.fPosX + data.fSpacingX * i;
				y1 = data.fPosY;
				x2 = x1 + data.fSize;
				y2 = y1 + data.fSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255, 255, 255, 255}, 0, texturePlacements, 0, hud->min, hud->max);
			}
		}
		{
			auto cup = &careerClass->Finals;
			auto cupSave = &careerSaveClass->Finals;
			auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(cup->aRaces[0].nLevel, "Image"));
			if (!trackIcon) {
				MessageBoxA(0, std::format("Failed to find image for track {}", cup->aRaces[0].nLevel).c_str(), "Fatal error", MB_ICONERROR);
			}
			auto data = gEvent;
			float x1 = data.fPosX + data.fSpacingX * 0.5;
			float y1 = data.fPosY + data.fSpacingY;
			float x2 = x1 + data.fSize * 1.5;
			float y2 = y1 + data.fSize;
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, cupSave->bUnlocked ? textureTracks : textureTracks2, 0, trackIcon->min, trackIcon->max);
			if (nCursorY == 1) {
				auto rgb = GetPaletteColor(COLOR_MENU_YELLOW);
				rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;
				x2 = x1 + fEventHighlightSize * 1.5;
				y2 = y1 + fEventHighlightSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, rgb);
			}

			if (cupSave->nPosition >= 1 && cupSave->nPosition <= 3) {
				data = gTrackPlacements;
				auto hud = GetHUDData(trackPlacements, trackPlacementNames[cupSave->nPosition-1]);
				x1 = data.fPosX + data.fSpacingX * 0.5;
				y1 = data.fPosY + data.fSpacingY * 1;
				x2 = x1 + data.fSize;
				y2 = y1 + data.fSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255, 255, 255, 255}, 0, texturePlacements, 0, hud->min, hud->max);
			}
		}
		for (int i = 0; i < careerClass->aEvents.size(); i++) {
			auto cup = &careerClass->aEvents[i];
			auto cupSave = &careerSaveClass->aEvents[i];
			bool unlocked = cupSave->bUnlocked;
			if (pGameFlow->nGameMode == eGameMode::SPLITSCREEN) {
				if (cup->aRaces[0].bIsTimeTrial) unlocked = false; // no time trials in splitscreen
				if (DoesTrackValueExist(cup->aRaces[0].nLevel, "StuntType")) unlocked = false; // no stunts in splitscreen
			}
			auto trackIcon = GetHUDData(trackIcons, GetTrackValueString(cup->aRaces[0].nLevel, "Image"));
			if (!trackIcon) {
				MessageBoxA(0, std::format("Failed to find image for track {}", cup->aRaces[0].nLevel).c_str(), "Fatal error", MB_ICONERROR);
			}
			auto data = gEvent;
			float x1 = data.fPosX + data.fSpacingX * i;
			float y1 = data.fPosY + data.fSpacingY * 2;
			float x2 = x1 + data.fSize * 1.5;
			float y2 = y1 + data.fSize;
			DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255,255,255,255}, 0, unlocked ? textureTracks : textureTracks2, 0, trackIcon->min, trackIcon->max);
			if (i == nCursorX && nCursorY == 2) {
				auto rgb = GetPaletteColor(COLOR_MENU_YELLOW);
				rgb.a = GetFlashingAlpha(gTimer.fTotalTime) * 0.5;
				x2 = x1 + fEventHighlightSize * 1.5;
				y2 = y1 + fEventHighlightSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, rgb);
			}

			if (cupSave->nPosition >= 1 && cupSave->nPosition <= 3) {
				data = gTrackPlacements;
				auto hud = GetHUDData(trackPlacements, trackPlacementNames[cupSave->nPosition-1]);
				x1 = data.fPosX + data.fSpacingX * i;
				y1 = data.fPosY + data.fSpacingY * 2;
				x2 = x1 + data.fSize;
				y2 = y1 + data.fSize;
				DrawRectangle(x1 * GetAspectRatioInv(), x2 * GetAspectRatioInv(), y1, y2, {255, 255, 255, 255}, 0, texturePlacements, 0, hud->min, hud->max);
			}
		}

		tNyaStringData data;
		data.x = gCupName.nPosX;
		data.y = gCupName.nPosY;
		data.size = gCupName.fSize;
		if (nCursorY == 0) {
			auto cup = &careerClass->aCups[nCursorX];
			Draw1080pString(JUSTIFY_RIGHT, data, cup->sName, &DrawStringFO2_Small);
			data.x = nLapsX[cup->aRaces.size()];
			data.y = nLapsY;
			data.size = fLapsSize;
			Draw1080pString(JUSTIFY_RIGHT, data, std::to_string(cup->aRaces.size()), &DrawStringFO2_Small);
			auto hud = gEvents;
			data.x = hud.nPosX;
			data.y = hud.nPosY;
			data.size = hud.fSize;
			for (auto& race : cup->aRaces) {
				auto name = GetTrackName(race.nLevel);
				if (race.bReversed) name = "REV " + name;
				Draw1080pString(JUSTIFY_RIGHT, data, name, &DrawStringFO2_Ingame12);
				data.y += hud.nSpacingY;
			}
		}
		else if (nCursorY == 1) {
			auto cup = &careerClass->Finals;
			Draw1080pString(JUSTIFY_RIGHT, data, cup->sName, &DrawStringFO2_Small);
			data.x = nLapsX[cup->aRaces.size()];
			data.y = nLapsY;
			data.size = fLapsSize;
			Draw1080pString(JUSTIFY_RIGHT, data, std::to_string(cup->aRaces.size()), &DrawStringFO2_Small);
			auto hud = gEvents;
			data.x = hud.nPosX;
			data.y = hud.nPosY;
			data.size = hud.fSize;
			for (auto& race : cup->aRaces) {
				auto name = GetTrackName(race.nLevel);
				if (race.bReversed) name = "REV " + name;
				Draw1080pString(JUSTIFY_RIGHT, data, name, &DrawStringFO2_Ingame12);
				data.y += hud.nSpacingY;
			}
		}
		else if (nCursorY == 2) {
			if (nCursorX >= careerClass->aEvents.size()) return;
			auto cup = &careerClass->aEvents[nCursorX];
			Draw1080pString(JUSTIFY_RIGHT, data, cup->sName, &DrawStringFO2_Small);
			data.x = nLapsX[cup->aRaces.size()];
			data.y = nLapsY;
			data.size = fLapsSize;
			Draw1080pString(JUSTIFY_RIGHT, data, std::to_string(cup->aRaces.size()), &DrawStringFO2_Small);
			auto hud = gEvents;
			data.x = hud.nPosX;
			data.y = hud.nPosY;
			data.size = hud.fSize;
			if (cup->aRaces[0].bIsTimeTrial) {
				if (bCursorJustMoved) {
					bCursorJustMoved = false;

					int targets[5] = {};
					ChloeTimeTrial::GetCareerMedalTimes(cup->aRaces[0].nLevel, cup->aRaces[0].nTimeTrialCar-1, targets);
					std::string targetStrings[] = {
							FormatGameTime(targets[0], false),
							FormatGameTime(targets[1], false),
							FormatGameTime(targets[2], false),
					};
					auto pb = ChloeTimeTrial::GetCareerPBTime(cup->aRaces[0].nLevel, cup->aRaces[0].nTimeTrialCar-1);
					std::string pbString = pb != UINT_MAX ? FormatGameTime(pb, false) : "N/A";
					if (targets[3] != UINT_MAX) {
						auto authorString = FormatGameTime(targets[3], false);
						sEventDescription = std::format("TARGETS\nAUTHOR - {}\nGOLD - {}\nSILVER - {}\nBRONZE - {}\n\nPERSONAL BEST: {}", authorString, targetStrings[0], targetStrings[1], targetStrings[2], pbString);
					}
					else {
						sEventDescription = std::format("TARGETS\nGOLD - {}\nSILVER - {}\nBRONZE - {}\n\nPERSONAL BEST: {}", targetStrings[0], targetStrings[1], targetStrings[2], pbString);
					}
				}
				Draw1080pString(JUSTIFY_RIGHT, data, sEventDescription, &DrawStringFO2_Ingame12);
			}
			else {
				int level = cup->aRaces[0].nLevel;
				int targets[3] = {0,0,0};
				CareerMode::GetStuntTargets(level, targets);
				if (targets[0]) {
					if (level == TRACK_BOWLING) {
						Draw1080pString(JUSTIFY_RIGHT, data, std::format("TARGETS\n{} - 1st\n{} - 2nd\n{} - 3rd\n\nPERSONAL BEST: {}", targets[0], targets[1], targets[2], careerSaveClass->aEvents[nCursorX].nTimeOrScore),
										&DrawStringFO2_Ingame12);
					}
					else {
						Draw1080pString(JUSTIFY_RIGHT, data, std::format("TARGETS\n{}m - 1st\n{}m - 2nd\n{}m - 3rd\n\nPERSONAL BEST: {}m", targets[0], targets[1], targets[2], careerSaveClass->aEvents[nCursorX].nTimeOrScore),
										&DrawStringFO2_Ingame12);
					}
				}
				else {
					Draw1080pString(JUSTIFY_RIGHT, data, std::format("PRIZES\n1st - ${}\n2nd - ${}\n3rd - ${}", cup->aCupWinnings[0], cup->aCupWinnings[1], cup->aCupWinnings[2]),
									&DrawStringFO2_Ingame12);
				}
			}
		}
		data.x = gEventsTitle.nPosX;
		data.y = gEventsTitle.nPosY;
		data.size = gEventsTitle.fSize;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		Draw1080pString(JUSTIFY_RIGHT, data, "EVENTS", &DrawStringFO2_Ingame12);
	}
} Menu_CareerCupSelect;