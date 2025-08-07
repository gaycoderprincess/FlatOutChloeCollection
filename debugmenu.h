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

void ProcessDebugMenu() {
	ChloeMenuLib::BeginMenu();

	QuickValueEditor("fPlayerHealthTextX", NewGameHud::fPlayerHealthTextX);
	QuickValueEditor("fPlayerHealthTextY", NewGameHud::fPlayerHealthTextY);
	QuickValueEditor("fPlayerHealthTextSize", NewGameHud::fPlayerHealthTextSize);
	QuickValueEditor("fPlayerHealthBarX", NewGameHud::fPlayerHealthBarX);
	QuickValueEditor("fPlayerHealthBarY", NewGameHud::fPlayerHealthBarY);
	QuickValueEditor("fPlayerHealthBarSize", NewGameHud::fPlayerHealthBarSize);
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