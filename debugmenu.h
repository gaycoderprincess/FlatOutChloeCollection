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

void QuickValueEditor(const char* name, CHUDElement::tDrawPositions& value, bool useSpacing) {
	if (DrawMenuOption(std::format("{}.fPosX - {}", name, value.fPosX))) { ValueEditorMenu(value.fPosX); }
	if (DrawMenuOption(std::format("{}.fPosY - {}", name, value.fPosY))) { ValueEditorMenu(value.fPosY); }
	if (DrawMenuOption(std::format("{}.fSize - {}", name, value.fSize))) { ValueEditorMenu(value.fSize); }
	if (useSpacing) {
		if (DrawMenuOption(std::format("{}.fSpacingX - {}", name, value.fSpacingX))) { ValueEditorMenu(value.fSpacingX); }
		if (DrawMenuOption(std::format("{}.fSpacingY - {}", name, value.fSpacingY))) { ValueEditorMenu(value.fSpacingY); }
	}
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

std::vector<NyaVec3> aCustomSplinesL;
std::vector<NyaVec3> aCustomSplinesR;
std::vector<NyaVec3> aCustomSplinesRaceLine;
std::vector<NyaMat4x4> aCustomStartpoints;

struct tSplitpoint {
	NyaVec3 pos;
	NyaVec3 left;
	NyaVec3 right;
};
std::vector<tSplitpoint> aCustomSplitpoints;

void SplineViewerMenu(std::vector<NyaVec3>& splines, const std::string& name) {
	if (!splines.empty()) {
		if (DrawMenuOption(std::format("Edit {} ({})", name, splines.size()))) {
			ChloeMenuLib::BeginMenu();
			for (auto& node: splines) {
				if (DrawMenuOption(std::to_string((&node - &splines[0]) + 1))) {
					ChloeMenuLib::BeginMenu();
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Node", "", false, false)) {
						auto ply = GetPlayer(0);
						ply->pCar->GetMatrix()->p = node;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (DrawMenuOption("Delete Node", "", false, false)) {
						splines.erase(splines.begin() + (&node - &splines[0]));
						ChloeMenuLib::BackOut();
						break;
					}
					ChloeMenuLib::EndMenu();
				}
			}
			ChloeMenuLib::EndMenu();
		}
	}
}

void StartpointViewerMenu(std::vector<NyaMat4x4>& startpoints, const std::string& name) {
	if (!startpoints.empty()) {
		if (DrawMenuOption(std::format("Edit {} ({})", name, startpoints.size()))) {
			ChloeMenuLib::BeginMenu();
			for (auto& node : startpoints) {
				if (DrawMenuOption(std::to_string((&node - &startpoints[0]) + 1))) {
					ChloeMenuLib::BeginMenu();
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Node", "", false, false)) {
						auto ply = GetPlayer(0);
						*ply->pCar->GetMatrix() = node;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (DrawMenuOption("Delete Node", "", false, false)) {
						startpoints.erase(startpoints.begin() + (&node - &startpoints[0]));
						ChloeMenuLib::BackOut();
						break;
					}
					ChloeMenuLib::EndMenu();
				}
			}
			ChloeMenuLib::EndMenu();
		}
	}
}

void SplitpointViewerMenu(std::vector<tSplitpoint>& splitpoints, const std::string& name) {
	if (!splitpoints.empty()) {
		if (DrawMenuOption(std::format("Edit {} ({})", name, splitpoints.size()))) {
			ChloeMenuLib::BeginMenu();
			for (auto& node : splitpoints) {
				if (DrawMenuOption(std::to_string((&node - &splitpoints[0]) + 1))) {
					ChloeMenuLib::BeginMenu();
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Node", "", false, false)) {
						auto ply = GetPlayer(0);
						ply->pCar->GetMatrix()->p = node.pos;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Left", "", false, false)) {
						auto ply = GetPlayer(0);
						ply->pCar->GetMatrix()->p = node.left;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Right", "", false, false)) {
						auto ply = GetPlayer(0);
						ply->pCar->GetMatrix()->p = node.right;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (DrawMenuOption("Delete Node", "", false, false)) {
						splitpoints.erase(splitpoints.begin() + (&node - &splitpoints[0]));
						ChloeMenuLib::BackOut();
						break;
					}
					ChloeMenuLib::EndMenu();
				}
			}
			ChloeMenuLib::EndMenu();
		}
	}
}

void WriteSplitpoints() {
	std::ofstream fout((std::string)GetTrackName(pGameFlow->nLevel) + "_splitpoints.bed", std::ios::out);
	if (!fout.is_open()) return;

	fout << "Count = ";
	fout << aCustomSplitpoints.size();
	fout << "\n\nSplitpoints = {";
	for (auto& point : aCustomSplitpoints) {
		fout << std::format("\n\t[{}] = {{", (&point - &aCustomSplitpoints[0]) + 1);
		fout << std::format("\n\t\tPosition = {{ {}, {}, {} }},", point.pos.x, point.pos.y, point.pos.z);
		fout << std::format("\n\t\tLeft = {{ {}, {}, {} }},", point.left.x, point.left.y, point.left.z);
		fout << std::format("\n\t\tRight = {{ {}, {}, {} }},", point.right.x, point.right.y, point.right.z);
		fout << "\n\n\t},";
	}
	fout << "\n}";
}

void WriteSpline(std::ofstream& fout, std::vector<NyaVec3>& vec, const std::string& name) {
	if (vec.empty()) return;

	fout << "\n\t[\"" + name + "\"] = {";
	fout << "\n\t\tCount = " + std::to_string(vec.size()) + ",";
	fout << "\n\t\tControlPoints = {";
	for (auto& pos : vec) {
		fout << std::format("\n\t\t\t[{}] = {{ {}, {}, {} }},", std::to_string((&pos - &vec[0]) + 1), pos.x, pos.y, pos.z);
	}
	fout << "\n\t\t},";
	fout << "\n\t},\n";
}

void WriteSplines() {
	std::ofstream fout((std::string)GetTrackName(pGameFlow->nLevel) + "_splines.ai", std::ios::out);
	if (!fout.is_open()) return;

	fout << "Count = 2";
	fout << "\n\nSplines = {";

	WriteSpline(fout, aCustomSplinesL, "AIBorderLineLeft");
	WriteSpline(fout, aCustomSplinesR, "AIBorderLineRight");
	WriteSpline(fout, aCustomSplinesRaceLine, "AIRaceLine");

	fout << "\n}";
}

void WriteStartpoints() {
	std::ofstream fout((std::string)GetTrackName(pGameFlow->nLevel) + "_startpoints.bed", std::ios::out);
	if (!fout.is_open()) return;

	fout << "Count = ";
	fout << aCustomStartpoints.size();
	fout << "\n\nStartpoints = {";
	for (auto& point : aCustomStartpoints) {
		for (int i = 0; i < 16; i++) {
			if (std::abs(point[i]) < 0.001) point[i] = 0;
		}

		fout << std::format("\n\t[{}] = {{", (&point - &aCustomStartpoints[0]) + 1);
		fout << std::format("\n\t\tPosition = {{ {}, {}, {} }},", point[12], point[13], point[14]);
		fout << "\n\t\tOrientation = {";
		fout << std::format("\n\t\t\t[\"x\"]={{{},{},{}}},", point[0], point[1], point[2]);
		fout << std::format("\n\t\t\t[\"y\"]={{{},{},{}}},", point[4], point[5], point[6]);
		fout << std::format("\n\t\t\t[\"z\"]={{{},{},{}}},", point[8], point[9], point[10]);
		fout << "\n\t\t},";
		fout << "\n\n\t},";
	}
	fout << "\n}";
}

void ProcessDebugMenu() {
	ChloeMenuLib::BeginMenu();

	if (DrawMenuOption("Game Rules")) {
		ChloeMenuLib::BeginMenu();

		for (auto& rule : GameRules::KeyValue::aKeyValues) {
			DrawDebugMenuViewerOption(std::format("{} - {}", rule->key, rule->value));
		}
		for (auto& rule : GameRules::KeyValueArray::aKeyValues) {
			for (int i = 0; i < rule->count; i++) {
				DrawDebugMenuViewerOption(std::format("{}[{}] - {}", rule->key, i+1, rule->values[i]));
			}
		}

		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Playtime Stats")) {
		ChloeMenuLib::BeginMenu();

		for (int i = 0; i < NUM_PLAYTIME_TYPES; i++) {
			if (!gCustomSave.playtimeNew[i]) continue;
			DrawDebugMenuViewerOption(std::format("{} - {}", aPlaytimeTypeNames[i], GetTimeString(gCustomSave.playtimeNew[i])));
		}

		ChloeMenuLib::EndMenu();
	}

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

		if (DrawMenuOption("Dump to File")) {
			std::ofstream fout("achievements.txt", std::ios::out);
			if (fout.is_open()) {
				auto achievements = Achievements::gAchievements;
				std::sort(achievements.begin(), achievements.end(), [] (Achievements::CAchievement* a, Achievements::CAchievement* b) { return (std::string)a->sName < (std::string)b->sName; });
				for (auto& achievement : achievements) {
					if (achievement->bHidden) continue;

					fout << achievement->sName;
					fout << " - ";
					fout << achievement->sDescription;
					fout << "\n";
				}
			}
		}

		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Car Helpers")) {
		ChloeMenuLib::BeginMenu();
		if (DrawMenuOption("Driver Location")) {
			ChloeMenuLib::BeginMenu();

			if (auto ply = GetPlayer(0)) {
				if (DrawMenuOption(std::format("X - {}", ply->pCar->Body.fDriverLoc[0]))) {
					ValueEditorMenu(ply->pCar->Body.fDriverLoc[0]);
				}
				if (DrawMenuOption(std::format("Y - {}", ply->pCar->Body.fDriverLoc[1]))) {
					ValueEditorMenu(ply->pCar->Body.fDriverLoc[1]);
				}
				if (DrawMenuOption(std::format("Z - {}", ply->pCar->Body.fDriverLoc[2]))) {
					ValueEditorMenu(ply->pCar->Body.fDriverLoc[2]);
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}

			ChloeMenuLib::EndMenu();
		}
		DrawDebugMenuViewerOption(std::format("Career Car - {} ({})", pGameFlow->Profile.nCarType, GetCarName(pGameFlow->Profile.nCarType+1)));
		DrawDebugMenuViewerOption(std::format("Career Car Upgraded - {}", gCustomSave.aCareerGarage[pGameFlow->Profile.nCarType+1].IsAnyUpgradePurchased()));
		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Track Helpers")) {
		ChloeMenuLib::BeginMenu();
		if (DrawMenuOption("Spline Creator")) {
			ChloeMenuLib::BeginMenu();
			if (GetGameState() == GAME_STATE_RACE) {
				if (DrawMenuOption("Add Left Node")) {
					auto ply = GetPlayer(0);
					aCustomSplinesL.push_back(ply->pCar->GetMatrix()->p);
				}
				if (DrawMenuOption("Add Right Node")) {
					auto ply = GetPlayer(0);
					aCustomSplinesR.push_back(ply->pCar->GetMatrix()->p);
				}
				if (DrawMenuOption("Add Raceline Node")) {
					auto ply = GetPlayer(0);
					aCustomSplinesRaceLine.push_back(ply->pCar->GetMatrix()->p);
				}
				SplineViewerMenu(aCustomSplinesL, "Left");
				SplineViewerMenu(aCustomSplinesR, "Right");
				SplineViewerMenu(aCustomSplinesRaceLine, "Raceline");
				if (!aCustomSplinesL.empty() && !aCustomSplinesR.empty() && !aCustomSplinesRaceLine.empty()) {
					if (DrawMenuOption("Save Splines", "", false, false)) {
						WriteSplines();
					}
					if (DrawMenuOption("Delete All Splines", "", false, false)) {
						aCustomSplinesL.clear();
						aCustomSplinesR.clear();
						aCustomSplinesRaceLine.clear();
					}
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}
			ChloeMenuLib::EndMenu();
		}
		if (DrawMenuOption("Startpoint Creator")) {
			ChloeMenuLib::BeginMenu();
			if (GetGameState() == GAME_STATE_RACE) {
				if (DrawMenuOption("Add Startpoint")) {
					auto ply = GetPlayer(0);
					aCustomStartpoints.push_back(*ply->pCar->GetMatrix());
				}
				StartpointViewerMenu(aCustomStartpoints, "Startpoints");
				if (!aCustomStartpoints.empty()) {
					if (DrawMenuOption("Save Startpoints", "", false, false)) {
						WriteStartpoints();
					}
					if (DrawMenuOption("Delete All Startpoints", "", false, false)) {
						aCustomStartpoints.clear();
					}
					if (DrawMenuOption("Apply Startpoints to Game", "", false, false)) {
						//pEnvironment->nNumStartpoints = aCustomStartpoints.size();
						//if (pEnvironment->nNumStartpoints > 32) pEnvironment->nNumStartpoints = 32;
						for (int i = 0; i < 8; i++) {
							auto& start = aCustomStartpoints[i];
							memcpy(pEnvironment->aStartpoints[i].fMatrix, &start, sizeof(start));
							pEnvironment->aStartpoints[i].fPosition[0] = start.p[0];
							pEnvironment->aStartpoints[i].fPosition[1] = start.p[1];
							pEnvironment->aStartpoints[i].fPosition[2] = start.p[2];
						}
					}
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}
			ChloeMenuLib::EndMenu();
		}
		if (DrawMenuOption("Splitpoint Creator")) {
			ChloeMenuLib::BeginMenu();
			if (GetGameState() == GAME_STATE_RACE) {
				if (DrawMenuOption("Add Splitpoint")) {
					auto ply = GetPlayer(0);
					tSplitpoint point;
					point.pos = point.left = point.right = ply->pCar->GetMatrix()->p;
					point.left -= ply->pCar->GetMatrix()->x * 50;
					point.right += ply->pCar->GetMatrix()->x * 50;
					aCustomSplitpoints.push_back(point);
				}
				SplitpointViewerMenu(aCustomSplitpoints, "Splitpoints");
				if (!aCustomSplitpoints.empty()) {
					if (DrawMenuOption("Save Splitpoints", "", false, false)) {
						WriteSplitpoints();
					}
					if (DrawMenuOption("Delete All Splitpoints", "", false, false)) {
						aCustomSplitpoints.clear();
					}
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}
			ChloeMenuLib::EndMenu();
		}
		if (DrawMenuOption("Resetpoint Editor")) {
			ChloeMenuLib::BeginMenu();
			if (GetGameState() == GAME_STATE_RACE) {
				if (DrawMenuOption("Add Reset")) {
					auto ply = GetPlayer(0);
					tResetpoint point;
					point.matrix = *ply->pCar->GetMatrix();
					point.split = ply->nCurrentSplit % pEnvironment->nNumSplitpoints;
					aNewResetPoints.push_back(point);
				}
				if (!aNewResetPoints.empty()) {
					if (DrawMenuOption(std::format("Edit Resetpoints ({})", aNewResetPoints.size()))) {
						ChloeMenuLib::BeginMenu();
						for (auto& reset: aNewResetPoints) {
							if (DrawMenuOption(std::to_string((&reset - &aNewResetPoints[0]) + 1))) {
								ChloeMenuLib::BeginMenu();
								if (DrawMenuOption("Teleport to Node", "", false, false)) {
									auto ply = GetPlayer(0);
									Car::Reset(ply->pCar, &reset.matrix.p.x, &reset.matrix.x.x);
									break;
								}
								if (DrawMenuOption("Delete Resetpoint", "", false, false)) {
									aNewResetPoints.erase(aNewResetPoints.begin() + (&reset - &aNewResetPoints[0]));
									ChloeMenuLib::BackOut();
									break;
								}
								ChloeMenuLib::EndMenu();
							}
						}
						ChloeMenuLib::EndMenu();
					}
					if (DrawMenuOption("Save Resetpoints", "", false, false)) {
						SaveResetPoints(GetResetPointFilename());
					}
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
		auto plyPos = ply->pCar->GetMatrix()->p;
		DrawDebugMenuViewerOption(std::format("Player Position - {:.1f} {:.1f} {:.1f}", plyPos.x, plyPos.y, plyPos.z));
		DrawDebugMenuViewerOption(std::format("Player Health - {:.1f}", 1.0 - ply->pCar->fDamage));
		DrawDebugMenuViewerOption(std::format("Race Time - {}", pPlayerHost->nRaceTime));
		if (auto reset = pPlayerResetpoint) {
			DrawDebugMenuViewerOption(std::format("Closest Resetpoint - {:.1f}m", (reset->p - ply->pCar->GetMatrix()->p).length()));
		}
	}

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}