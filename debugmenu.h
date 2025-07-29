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

void ProcessDebugMenu() {
	ChloeMenuLib::BeginMenu();

	if (DrawMenuOption(std::format("fPosX - {}", SkinSelector::fPosX))) {
		ValueEditorMenu(SkinSelector::fPosX);
	}
	if (DrawMenuOption(std::format("fPosY - {}", SkinSelector::fPosY))) {
		ValueEditorMenu(SkinSelector::fPosY);
	}
	if (DrawMenuOption(std::format("fSize - {}", SkinSelector::fSize))) {
		ValueEditorMenu(SkinSelector::fSize);
	}
	if (DrawMenuOption(std::format("fSpacing - {}", SkinSelector::fSpacing))) {
		ValueEditorMenu(SkinSelector::fSpacing);
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
				ChloeMenuLib::EndMenu();
			}
		}
		ChloeMenuLib::EndMenu();
	}

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}