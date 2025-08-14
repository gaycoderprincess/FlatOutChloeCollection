void DrawDebugMenuViewerOption(const std::string& name, const std::string& desc = "") {
	if (DrawMenuOption(name, desc, false, false, true)) {
		ChloeMenuLib::SetEnterHint("");
	}
}

void ValueEditorMenu(float& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stof(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
}

void ValueEditorMenu(int& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stoi(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
}

// copied from lua
std::string GetTimeString(double time_in_sec) {
	int64_t h = std::floor(time_in_sec / 3600.0);
	int64_t m = std::floor(((int)time_in_sec % 3600) / 60.0);
	int64_t s = (int)time_in_sec % 60;
	return std::format("{}:{:02}:{:02}", h, m, s);
}

void PaletteEditorMenu(uint8_t& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stoi(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
}

void QuickValueEditor(const char* name, float& value) {
	if (DrawMenuOption(std::format("{} - {}", name, value))) { ValueEditorMenu(value); }
}

void QuickValueEditor(const char* name, int& value) {
	if (DrawMenuOption(std::format("{} - {}", name, value))) { ValueEditorMenu(value); }
}

void QuickValueEditor(const char* name, CHUDElement::tDrawPositions1080p& value, bool useSpacing) {
	if (DrawMenuOption(std::format("{}.nPosX - {}", name, value.nPosX))) { ValueEditorMenu(value.nPosX); }
	if (DrawMenuOption(std::format("{}.nPosY - {}", name, value.nPosY))) { ValueEditorMenu(value.nPosY); }
	if (DrawMenuOption(std::format("{}.fSize - {}", name, value.fSize))) { ValueEditorMenu(value.fSize); }
	if (useSpacing) {
		if (DrawMenuOption(std::format("{}.nSpacingX - {}", name, value.nSpacingX))) { ValueEditorMenu(value.nSpacingX); }
		if (DrawMenuOption(std::format("{}.nSpacingY - {}", name, value.nSpacingY))) { ValueEditorMenu(value.nSpacingY); }
	}
}

void ProcessDebugMenu() {
	ChloeMenuLib::BeginMenu();

	//QuickValueEditor("fPlayerHealthTextX", NewGameHud::fPlayerHealthTextX);
	//QuickValueEditor("fPlayerHealthTextY", NewGameHud::fPlayerHealthTextY);
	//QuickValueEditor("fPlayerHealthTextSize", NewGameHud::fPlayerHealthTextSize);
	//QuickValueEditor("fPlayerHealthBarX", NewGameHud::fPlayerHealthBarX);
	//QuickValueEditor("fPlayerHealthBarY", NewGameHud::fPlayerHealthBarY);
	//QuickValueEditor("fPlayerHealthBarSize", NewGameHud::fPlayerHealthBarSize);
	//QuickValueEditor("fMapOffset", NewGameHud::fMapOffset);
	//QuickValueEditor("fMusicPlayerOffset", NewGameHud::fMusicPlayerOffset);
	//NewGameHud::fMusicPlayer416 = 416 + NewGameHud::fMusicPlayerOffset;
	//NewGameHud::fMusicPlayer480 = 480 + NewGameHud::fMusicPlayerOffset;
	//NewGameHud::fMusicPlayer428 = 428 + NewGameHud::fMusicPlayerOffset;
	//QuickValueEditor("fContactTimerX", NewGameHud::fContactTimerX);
	//QuickValueEditor("fContactTimerY1", NewGameHud::fContactTimerY1);
	//QuickValueEditor("fContactTimerY2", NewGameHud::fContactTimerY2);
	//QuickValueEditor("fContactTimerSize1", NewGameHud::fContactTimerSize1);
	//QuickValueEditor("fContactTimerSize2", NewGameHud::fContactTimerSize2);
	//QuickValueEditor("gCarSkinSelectAuthor.nPosX", NewMenuHud::gCarSkinSelectAuthor.nPosX);
	//QuickValueEditor("gCarSkinSelectAuthor.nPosY", NewMenuHud::gCarSkinSelectAuthor.nPosY);
	//QuickValueEditor("gCarSkinSelectAuthor.fSize", NewMenuHud::gCarSkinSelectAuthor.fSize);
	//QuickValueEditor("fCrashBonusTextX", HUD_DamageMeter.fCrashBonusTextX);
	//QuickValueEditor("fCrashBonusTextY", HUD_DamageMeter.fCrashBonusTextY);
	//QuickValueEditor("fCrashBonusTextSize", HUD_DamageMeter.fCrashBonusTextSize);
	//QuickValueEditor("fCrashVelocityMultiplier", fCrashVelocityMultiplier);
	//QuickValueEditor("gTitles", Menu_CrashResult.gTitles, true);
	//QuickValueEditor("fNumberOffset", Menu_CrashResult.fNumberOffset);
	//QuickValueEditor("fCashOffset", Menu_CrashResult.fCashOffset);
	//QuickValueEditor("fBottomOffset", Menu_CrashResult.fBottomOffset);
	//QuickValueEditor("fSubSize", Menu_CrashResult.fSubSize);
	//QuickValueEditor("gTotalTitle", Menu_CrashResult.gTotalTitle, false);
	//QuickValueEditor("gTotalAmount", Menu_CrashResult.gTotalAmount, false);
	QuickValueEditor("gUnlockTitle", Menu_Career_Unlock.gUnlockTitle, false);
	QuickValueEditor("gUnlockSubtitle", Menu_Career_Unlock.gUnlockSubtitle, false);
	QuickValueEditor("gBottomTitle", Menu_Career_Unlock.gBottomTitle, false);
	ChloeMenuHud::EnterMenu(&Menu_Career_Unlock);

	//QuickValueEditor("fCarResetSpeed", fCarResetSpeed);

	if (DrawMenuOption("Achievements")) {
		ChloeMenuLib::BeginMenu();

		if (Achievements::GetNumUnlockedAchievements() > 0) {
			DrawMenuOption("Unlocked", "", true);
			for (auto& achievement : Achievements::gAchievements) {
				if (!achievement->bUnlocked) continue;
				DrawDebugMenuViewerOption(achievement->sName);
				DrawDebugMenuViewerOption(achievement->sDescription);
				DrawMenuOption("", "", true);
			}
		}

		if (Achievements::GetNumUnlockedAchievements() < Achievements::GetNumVisibleAchievements()) {
			DrawMenuOption("Locked", "", true);
			for (auto& achievement : Achievements::gAchievements) {
				if (achievement->bUnlocked) continue;
				if (achievement->bHidden) continue;
				DrawDebugMenuViewerOption(std::format("{} ({}%)", achievement->sName, achievement->nProgress));
				DrawDebugMenuViewerOption(achievement->sDescription);
				DrawMenuOption("", "", true);
			}
		}

		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Car Helpers")) {
		ChloeMenuLib::BeginMenu();
		if (DrawMenuOption("Driver Location")) {
			ChloeMenuLib::BeginMenu();

			if (auto ply = GetPlayer(0)) {
				if (DrawMenuOption(std::format("X - {}", ply->pCar->vDriverLoc[0]))) {
					ValueEditorMenu(ply->pCar->vDriverLoc[0]);
				}
				if (DrawMenuOption(std::format("Y - {}", ply->pCar->vDriverLoc[1]))) {
					ValueEditorMenu(ply->pCar->vDriverLoc[1]);
				}
				if (DrawMenuOption(std::format("Z - {}", ply->pCar->vDriverLoc[2]))) {
					ValueEditorMenu(ply->pCar->vDriverLoc[2]);
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}

			ChloeMenuLib::EndMenu();
		}
		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Palette Editor")) {
		ChloeMenuLib::BeginMenu();
		for (int i = 0; i < 256; i++) {
			auto& col = *(NyaDrawing::CNyaRGBA32*)&gPalette[i];
			if (DrawMenuOption(std::format("Color {} - {} {} {}", i, col.b, col.g, col.r))) {
				ChloeMenuLib::BeginMenu();
				if (DrawMenuOption(std::format("Red - {}", col.b))) {
					PaletteEditorMenu(col.b);
				}
				if (DrawMenuOption(std::format("Green - {}", col.g))) {
					PaletteEditorMenu(col.g);
				}
				if (DrawMenuOption(std::format("Blue - {}", col.r))) {
					PaletteEditorMenu(col.r);
				}

				DrawRectangle(0.4, 0.6, 0.4, 0.6, GetPaletteColor(i));

				ChloeMenuLib::EndMenu();
			}
		}
		ChloeMenuLib::EndMenu();
	}

	DrawMenuOption("Game State:", "", true);

	if (DrawMenuOption("Cup Info")) {
		ChloeMenuLib::BeginMenu();

		DrawDebugMenuViewerOption(std::format("Class - {:X}", gCustomSave.nCareerClass));
		DrawDebugMenuViewerOption(std::format("Cup - {:X}", gCustomSave.nCareerCup));
		DrawDebugMenuViewerOption(std::format("Next Event - {:X}", gCustomSave.nCareerCupNextEvent));
		DrawDebugMenuViewerOption(std::format("Event - {:X}", gCustomSave.nCareerEvent));

		ChloeMenuLib::EndMenu();
	}

	if (auto ply = GetPlayer(0)) {
		DrawDebugMenuViewerOption(std::format("Player Pointer - {:X}", (uintptr_t)ply));
		DrawDebugMenuViewerOption(std::format("Player Car Pointer - {:X}", (uintptr_t)ply->pCar));
		DrawDebugMenuViewerOption(std::format("Player Score Pointer - {:X}", (uintptr_t)GetPlayerScore<PlayerScoreRace>(1)));
	}

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}