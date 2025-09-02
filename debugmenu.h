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
	//QuickValueEditor("gUnlockTitle", Menu_Career_Unlock.gUnlockTitle, false);
	//QuickValueEditor("gUnlockSubtitle", Menu_Career_Unlock.gUnlockSubtitle, false);
	//QuickValueEditor("gBottomTitle", Menu_Career_Unlock.gBottomTitle, false);
	//QuickValueEditor("gRewardString", Menu_CareerFinalResults.gRewardString, false);
	//QuickValueEditor("fSpacingX", HUD_PlayerList.fSpacingX);
	//ChloeMenuHud::EnterMenu(&Menu_Career_Unlock);
	//QuickValueEditor("fIngameMapSize", fIngameMapSize);
	//QuickValueEditor("fCarResetSpeed", fCarResetSpeed);
	//QuickValueEditor("nOpponentCount", nOpponentCount);
	//QuickValueEditor("Menu_Achievement_Description.gPos", Menu_Achievement_Description.gPos, false);
	//QuickValueEditor("CarnageRace::gScoreHUD", CarnageRace::gScoreHUD, true);
	//QuickValueEditor("HUD_ArcadeScore.fPosY1", CarnageRace::HUD_ArcadeScore.fPosY1);
	//QuickValueEditor("gBase", CarnageRace::HUD_ArcadeRace.gBase, true);
	//QuickValueEditor("gCheckpointBonus", CarnageRace::HUD_ArcadeRace.gCheckpointBonus, false);
	//QuickValueEditor("fElementTotalSpacing", CarnageRace::HUD_ArcadeRace.fElementTotalSpacing);
	//QuickValueEditor("gEvent", Menu_TrackSelect.gEvent, false);
	//QuickValueEditor("gMap", Menu_TrackSelect.gMap, false);
	//QuickValueEditor("gOptions", Menu_TrackSelect.gOptions, true);
	//QuickValueEditor("gOptionsHover", Menu_TrackSelect.gOptionsHover, true);
	//QuickValueEditor("fArrowX", Menu_TrackSelect.fArrowX);
	//QuickValueEditor("fArrowYOffset", Menu_TrackSelect.fArrowYOffset);
	//QuickValueEditor("fArrowSize", Menu_TrackSelect.fArrowSize);
	//QuickValueEditor("gTrackName", Menu_TrackSelect.gCarName, false);
	//QuickValueEditor("gLevelPB", Menu_TrackSelect.gLevelPB, false);
	//QuickValueEditor("gEvent", Menu_ArcadeCareer.gEvent, true);
	//QuickValueEditor("fEventHighlightSize", Menu_ArcadeCareer.fEventHighlightSize);
	//QuickValueEditor("gLevelName", Menu_ArcadeCareer.gLevelName, false);
	//QuickValueEditor("gLevelType", Menu_ArcadeCareer.gLevelType, false);
	//QuickValueEditor("gTotalScore", Menu_ArcadeCareer.gTotalScore, false);
	//QuickValueEditor("gTargetScoresTitle", Menu_ArcadeCareer.gTargetScoresTitle, false);
	//QuickValueEditor("gTargetScores", Menu_ArcadeCareer.gTargetScores, true);
	//QuickValueEditor("gYourScoreTitle", Menu_ArcadeCareer.gYourScoreTitle, false);
	//QuickValueEditor("gYourScore", Menu_ArcadeCareer.gYourScore, false);
	//QuickValueEditor("gGamemodeLabel", HUD_ArcadeMode.gGamemodeLabel, false);
	//QuickValueEditor("gGamemodeDescription", HUD_ArcadeMode.gGamemodeDescription, false);
	//QuickValueEditor("nPlayerReadyX", Menu_Multiplayer_Lobby.nPlayerReadyX);
	//QuickValueEditor("nPlayerNameX", Menu_Multiplayer_Lobby.nPlayerNameX);
	//QuickValueEditor("nPlayerCarX", Menu_Multiplayer_Lobby.nPlayerCarX);
	//QuickValueEditor("nPlayerPingX", Menu_Multiplayer_Lobby.nPlayerPingX);
	//QuickValueEditor("nPlayerStartY", Menu_Multiplayer_Lobby.nPlayerStartY);
	//QuickValueEditor("nPlayerListSpacing", Menu_Multiplayer_Lobby.nPlayerListSpacing);
	//QuickValueEditor("fPlayerListTextSize", Menu_Multiplayer_Lobby.fPlayerListTextSize);
	//QuickValueEditor("nOptionX", Menu_Multiplayer_Lobby.nOptionX);
	//QuickValueEditor("nOptionStartY", Menu_Multiplayer_Lobby.nOptionStartY);
	//QuickValueEditor("nOptionSpacing", Menu_Multiplayer_Lobby.nOptionSpacing);
	//QuickValueEditor("nOptionSpacing2", Menu_Multiplayer_Lobby.nOptionSpacing2);
	//QuickValueEditor("fOptionTextSize", Menu_Multiplayer_Lobby.fOptionTextSize);
	//QuickValueEditor("fTrackPreviewX", Menu_Multiplayer_Lobby.fTrackPreviewX);
	//QuickValueEditor("nTrackPreviewX", Menu_Multiplayer_Lobby.nTrackPreviewX);
	//if (DrawMenuOption(std::format("bCentered - {}", HUD_LapTime.bCentered))) {
	//	HUD_LapTime.bCentered = !HUD_LapTime.bCentered;
	//}
	//QuickValueEditor("nNonCenterX", HUD_LapTime.nNonCenterX);
	//QuickValueEditor("nNonCenterX_1", HUD_LapTime.nNonCenterX_1);
	//QuickValueEditor("gMenuTitle", HUD_PauseMenu.gMenuTitle, false);
	//QuickValueEditor("gMenuOptions", HUD_PauseMenu.gMenuOptions, true);
	//QuickValueEditor("nSliderLeft", HUD_PauseMenu.nSliderLeft);
	//QuickValueEditor("nSliderRight", HUD_PauseMenu.nSliderRight);
	//QuickValueEditor("nSliderTop", HUD_PauseMenu.nSliderTop);
	//QuickValueEditor("nSliderBottom", HUD_PauseMenu.nSliderBottom);
	//QuickValueEditor("gMenuText", HUD_StartMenu.gMenuText, false);
	//QuickValueEditor("gGamemodeDescription", HUD_ArcadeTutorial.gGamemodeDescription, false);
	//QuickValueEditor("nPlayerYStartLabel", HUD_RaceResults.nPlayerYStartLabel);
	//QuickValueEditor("nPlayerYStart", HUD_RaceResults.nPlayerYStart);
	//QuickValueEditor("nPlayerSpacing", HUD_RaceResults.nPlayerSpacing);
	//QuickValueEditor("fPlayerSize", HUD_RaceResults.fPlayerSize);
	//QuickValueEditor("nPositionX", HUD_RaceResults.nPositionX);
	//QuickValueEditor("nPositionY", HUD_RaceResults.nPositionY);
	//QuickValueEditor("nNameX", HUD_RaceResults.nNameX);
	//QuickValueEditor("nBestLapX", HUD_RaceResults.nBestLapX);
	//QuickValueEditor("nTotalTimeX", HUD_RaceResults.nTotalTimeX);
	//QuickValueEditor("fBalkOffsetY", HUD_RaceResults.fBalkOffsetY);
	//QuickValueEditor("fBalkSizeY", HUD_RaceResults.fBalkSizeY);
	//QuickValueEditor("nPressStartY", HUD_RaceResults.nPressStartY);
	//QuickValueEditor("nFlagX", Menu_Multiplayer_Lobby.nFlagX);
	//QuickValueEditor("nFlagYOffset", Menu_Multiplayer_Lobby.nFlagYOffset);
	//QuickValueEditor("nFlagSize", Menu_Multiplayer_Lobby.nFlagSize);
	//QuickValueEditor("nIconX", Menu_Multiplayer_Lobby.nIconX);
	//QuickValueEditor("nIconYOffset", Menu_Multiplayer_Lobby.nIconYOffset);
	//QuickValueEditor("nIconSize", Menu_Multiplayer_Lobby.nIconSize);
	//QuickValueEditor("nIcon2X", Menu_Multiplayer_Lobby.nIcon2X);
	//QuickValueEditor("nWreckingDerbyCrashX", HUD_RaceResults.nWreckingDerbyCrashX);
	//QuickValueEditor("nWreckingDerbyWreckX", HUD_RaceResults.nWreckingDerbyWreckX);
	//QuickValueEditor("nWreckingDerbyBonusX", HUD_RaceResults.nWreckingDerbyBonusX);
	//QuickValueEditor("nWreckingDerbyTotalX", HUD_RaceResults.nWreckingDerbyTotalX);
	//QuickValueEditor("fPopupSize", FragDerby::HUD_FragDerby.fPopupSize);
	//QuickValueEditor("fPopupSpacing", FragDerby::HUD_FragDerby.fPopupSpacing);
	//QuickValueEditor("nArrowX", Menu_ArcadeCareer.nArrowX);
	//QuickValueEditor("nArrowY1", Menu_ArcadeCareer.nArrowY1);
	//QuickValueEditor("nArrowY2", Menu_ArcadeCareer.nArrowY2);
	//QuickValueEditor("nArrowSizeX", Menu_ArcadeCareer.nArrowSizeX);
	//QuickValueEditor("nArrowSizeY", Menu_ArcadeCareer.nArrowSizeY);
	//QuickValueEditor("f3D2DYDivByDistance", f3D2DYDivByDistance);
	//QuickValueEditor("fPlayerIconOffset", HUD_Derby_Overhead.fPlayerIconOffset);
	//QuickValueEditor("fPlayerIconSize", HUD_Derby_Overhead.fPlayerIconSize);
	//QuickValueEditor("fPlayerHealthIconOffset", HUD_Derby_Overhead.fPlayerHealthIconOffset);
	//QuickValueEditor("fPlayerHealthIconSize", HUD_Derby_Overhead.fPlayerHealthIconSize);
	//QuickValueEditor("fPlayerHealthIconMinSize", HUD_Derby_Overhead.fPlayerHealthIconMinSize);
	//QuickValueEditor("gLoadingTipTitle", Menu_LoadingScreen.gLoadingTipTitle, false);
	//QuickValueEditor("gLoadingTipText", Menu_LoadingScreen.gLoadingTipText, false);
	//if (DrawMenuOption("Generate New Tip")) {
	//	Menu_LoadingScreen.gLoadingScreenTip = Menu_LoadingScreen.GetRandomLoadingScreenTip();
	//	auto loading = Menu_LoadingScreen.GetLoadingScreenForTrack(1);
	//	if (loading) Menu_LoadingScreen.sTextureName = std::format("data/menu/bg/{}.tga", loading);
	//}
	//QuickValueEditor("fBalk1Size", HUD_RaceResults.fBalk1Size);
	//QuickValueEditor("fBalk1Pos", HUD_RaceResults.fBalk1Pos);
	//QuickValueEditor("fBalk2Size", HUD_RaceResults.fBalk2Size);
	//QuickValueEditor("fBalk2Pos", HUD_RaceResults.fBalk2Pos);
	//QuickValueEditor("gTotalEventScore", HUD_RaceResults.gTotalEventScore, false);
	//QuickValueEditor("gEventScore", HUD_RaceResults.gEventScore, false);
	//QuickValueEditor("gHighScore", HUD_RaceResults.gHighScore, true);
	//QuickValueEditor("gScoreBreakdown", HUD_RaceResults.gScoreBreakdown, false);
	//QuickValueEditor("gScoreTypes", HUD_RaceResults.gScoreTypes, true);

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
		if (DrawMenuOption("Tire Dynamics")) {
			ChloeMenuLib::BeginMenu();

			if (auto ply = GetPlayer(0)) {
				auto data = &ply->pCar->TireDynamics[0];
				QuickValueEditor("fRollingResistance", data->fRollingResistance);
				QuickValueEditor("fInducedDragCoeff", data->fInducedDragCoeff);
				QuickValueEditor("fPneumaticTrail", data->fPneumaticTrail);
				QuickValueEditor("fPneumaticOffset", data->fPneumaticOffset);
				QuickValueEditor("fZStiffness[0]", data->fZStiffness[0]);
				QuickValueEditor("fZStiffness[1]", data->fZStiffness[1]);
				QuickValueEditor("fZStiffness[2]", data->fZStiffness[2]);
				QuickValueEditor("fXStiffness[0]", data->fXStiffness[0]);
				QuickValueEditor("fXStiffness[1]", data->fXStiffness[1]);
				QuickValueEditor("fXStiffness[2]", data->fXStiffness[2]);
				QuickValueEditor("fCStiffness[0]", data->fCStiffness[0]);
				QuickValueEditor("fCStiffness[1]", data->fCStiffness[1]);
				QuickValueEditor("fZFriction[0]", data->fZFriction[0]);
				QuickValueEditor("fZFriction[1]", data->fZFriction[1]);
				QuickValueEditor("fXFriction[0]", data->fXFriction[0]);
				QuickValueEditor("fXFriction[1]", data->fXFriction[1]);
				QuickValueEditor("fSlideControl", data->fSlideControl);
				QuickValueEditor("fUnderSteer", data->fUnderSteer);
				QuickValueEditor("fSlowDown", data->fSlowDown);
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}

			ChloeMenuLib::EndMenu();
		}
		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Track Helpers")) {
		ChloeMenuLib::BeginMenu();
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