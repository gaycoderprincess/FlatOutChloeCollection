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

	//if (DrawMenuOption(std::format("fPosX - {}", SkinSelector::fPosX))) { ValueEditorMenu(SkinSelector::fPosX); }
	//if (DrawMenuOption(std::format("fPosY - {}", SkinSelector::fPosY))) { ValueEditorMenu(SkinSelector::fPosY); }
	//if (DrawMenuOption(std::format("fSize - {}", SkinSelector::fSize))) { ValueEditorMenu(SkinSelector::fSize); }
	//if (DrawMenuOption(std::format("fSpacing - {}", SkinSelector::fSpacing))) { ValueEditorMenu(SkinSelector::fSpacing); }
	//if (DrawMenuOption(std::format("nPowerY - {}", NewMenuHud::nPowerY))) { ValueEditorMenu(NewMenuHud::nPowerY); }
	//if (DrawMenuOption(std::format("nPriceY - {}", NewMenuHud::nPriceY))) { ValueEditorMenu(NewMenuHud::nPriceY); }
	//if (DrawMenuOption(std::format("nWeightY - {}", NewMenuHud::nWeightY))) { ValueEditorMenu(NewMenuHud::nWeightY); }
	//if (DrawMenuOption(std::format("nOffsetY - {}", NewMenuHud::nOffsetY))) { ValueEditorMenu(NewMenuHud::nOffsetY); }
	//if (DrawMenuOption(std::format("nBaseX - {}", NewMenuHud::nBaseX))) { ValueEditorMenu(NewMenuHud::nBaseX); }
	//if (DrawMenuOption(std::format("nOffsetX - {}", NewMenuHud::nOffsetX))) { ValueEditorMenu(NewMenuHud::nOffsetX); }
	//if (DrawMenuOption(std::format("fBaseSize - {}", NewMenuHud::fBaseSize))) { ValueEditorMenu(NewMenuHud::fBaseSize); }
	//if (DrawMenuOption(std::format("fOffsetSize - {}", NewMenuHud::fOffsetSize))) { ValueEditorMenu(NewMenuHud::fOffsetSize); }
	//if (DrawMenuOption(std::format("nOffsetPriceY - {}", NewMenuHud::nOffsetPriceY))) { ValueEditorMenu(NewMenuHud::nOffsetPriceY); }
	//if (DrawMenuOption(std::format("fOffsetPriceSize - {}", NewMenuHud::fOffsetPriceSize))) { ValueEditorMenu(NewMenuHud::fOffsetPriceSize); }
	//if (DrawMenuOption(std::format("nDescriptionX - {}", NewMenuHud::nDescriptionX))) { ValueEditorMenu(NewMenuHud::nDescriptionX); }
	//if (DrawMenuOption(std::format("nDescriptionY - {}", NewMenuHud::nDescriptionY))) { ValueEditorMenu(NewMenuHud::nDescriptionY); }
	//if (DrawMenuOption(std::format("fDescriptionSize - {}", NewMenuHud::fDescriptionSize))) { ValueEditorMenu(NewMenuHud::fDescriptionSize); }
	//if (DrawMenuOption(std::format("nSkinSelectTitleX - {}", NewMenuHud::nSkinSelectTitleX))) { ValueEditorMenu(NewMenuHud::nSkinSelectTitleX); }
	//if (DrawMenuOption(std::format("nSkinSelectTitleY - {}", NewMenuHud::nSkinSelectTitleY))) { ValueEditorMenu(NewMenuHud::nSkinSelectTitleY); }
	//if (DrawMenuOption(std::format("fSkinSelectSize - {}", NewMenuHud::fSkinSelectSize))) { ValueEditorMenu(NewMenuHud::fSkinSelectSize); }
	//if (DrawMenuOption(std::format("fCarNameX - {}", NewMenuHud::fCarNameX))) { ValueEditorMenu(NewMenuHud::fCarNameX); }
	//if (DrawMenuOption(std::format("fCarNameY - {}", NewMenuHud::fCarNameY))) { ValueEditorMenu(NewMenuHud::fCarNameY); }
	//if (DrawMenuOption(std::format("fCarNameSize - {}", NewMenuHud::fCarNameSize))) { ValueEditorMenu(NewMenuHud::fCarNameSize); }
	//if (DrawMenuOption(std::format("nCarNameTextX - {}", NewMenuHud::nCarNameTextX))) { ValueEditorMenu(NewMenuHud::nCarNameTextX); }
	//if (DrawMenuOption(std::format("nCarNameTextY - {}", NewMenuHud::nCarNameTextY))) { ValueEditorMenu(NewMenuHud::nCarNameTextY); }
	//if (DrawMenuOption(std::format("fCarNameTextSize - {}", NewMenuHud::fCarNameTextSize))) { ValueEditorMenu(NewMenuHud::fCarNameTextSize); }
	//QuickValueEditor("fLoadingSpriteX", NewMenuHud::fLoadingSpriteX);
	//QuickValueEditor("fLoadingSpriteY", NewMenuHud::fLoadingSpriteY);
	//QuickValueEditor("fLoadingSpriteSize", NewMenuHud::fLoadingSpriteSize);
	//QuickValueEditor("nLoadingLogoY", NewMenuHud::nLoadingLogoY);
	//QuickValueEditor("nLoadingTextX", NewMenuHud::nLoadingTextX);
	//QuickValueEditor("nLoadingTextY", NewMenuHud::nLoadingTextY);
	//QuickValueEditor("fLoadingTextSize", NewMenuHud::fLoadingTextSize);
	//QuickValueEditor("fPlayerListX", NewGameHud::fPlayerListX);
	//QuickValueEditor("fPlayerListY", NewGameHud::fPlayerListY);
	//QuickValueEditor("fPlayerListSize", NewGameHud::fPlayerListSize);
	//QuickValueEditor("fPlayerListSpacing", NewGameHud::fPlayerListSpacing);
	//QuickValueEditor("fWreckedNotifY", fWreckedNotifY);
	//QuickValueEditor("fWreckedNotifSize", fWreckedNotifSize);
	//QuickValueEditor("nCareerListPositionX", NewMenuHud::nCareerListPositionX);
	//QuickValueEditor("nCareerListNameX", NewMenuHud::nCareerListNameX);
	//QuickValueEditor("nCareerListPointsX", NewMenuHud::nCareerListPointsX);
	//QuickValueEditor("nCareerListTopY", NewMenuHud::nCareerListTopY);
	//QuickValueEditor("nCareerListStartY", NewMenuHud::nCareerListStartY);
	//QuickValueEditor("nCareerListSpacing", NewMenuHud::nCareerListSpacing);
	//QuickValueEditor("fCareerListSize", NewMenuHud::fCareerListSize);
	//QuickValueEditor("nCareerEventNameX", NewMenuHud::nCareerEventNameX);
	//QuickValueEditor("nCareerEventNameY", NewMenuHud::nCareerEventNameY);
	//QuickValueEditor("nCareerEventDescY", NewMenuHud::nCareerEventDescY);
	//QuickValueEditor("fCareerEventSize", NewMenuHud::fCareerEventSize);
	//QuickValueEditor("fCareerEventDescSize", NewMenuHud::fCareerEventDescSize);
	//QuickValueEditor("fNitroAirtimeTolerance", fNitroAirtimeTolerance);
	//QuickValueEditor("fNitroAirtimeRate", fNitroAirtimeRate);
	QuickValueEditor("nClassX", NewMenuHud::nClassX);
	QuickValueEditor("nClassY", NewMenuHud::nClassY);
	QuickValueEditor("fClassSize", NewMenuHud::fClassSize);

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