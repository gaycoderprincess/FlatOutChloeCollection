auto GetStringWide(const std::string& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(string);
}

auto GetStringNarrow(const std::wstring& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(string);
}

void DoGameSave() {
	CareerMode::OnSave();
	gCustomSave.Save();
}

int ChloeSkins_GetNumSkinsForCar(void* a1) {
	lua_pushnumber(a1, GetNumSkinsForCar(luaL_checknumber(a1, 1)));
	return 1;
}

int ChloeCollection_GetRandom(void* a1) {
	int r = rand() % (int)luaL_checknumber(a1, 1);
	lua_pushnumber(a1, r);
	return 1;
}

int ChloeMenu_EnterMenu(void* a1) {
	ChloeMenuHud::EnterMenu((const char*)lua_tolstring(a1, 1));
	return 0;
}

int ChloeMenu_ExitMenu(void* a1) {
	ChloeMenuHud::EnterMenu(nullptr);
	return 0;
}

int ChloeMenu_MoveLeft(void* a1) {
	ChloeMenuHud::OnMoveLeft();
	return 0;
}

int ChloeMenu_MoveRight(void* a1) {
	ChloeMenuHud::OnMoveRight();
	return 0;
}

int ChloeMenu_MoveUp(void* a1) {
	ChloeMenuHud::OnMoveUp();
	return 0;
}

int ChloeMenu_MoveDown(void* a1) {
	ChloeMenuHud::OnMoveDown();
	return 0;
}

int ChloeSkins_ShowSkinSelector(void* a1) {
	Menu_CarDealer.bSkinSelector = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeHUD_CareerCupSelect_Left(void* a1) {
	Menu_CareerCupSelect.MoveLeft();
	return 0;
}

int ChloeHUD_CareerCupSelect_Right(void* a1) {
	Menu_CareerCupSelect.MoveRight();
	return 0;
}

int ChloeHUD_CareerCupSelect_Up(void* a1) {
	Menu_CareerCupSelect.MoveUp();
	return 0;
}

int ChloeHUD_CareerCupSelect_Down(void* a1) {
	Menu_CareerCupSelect.MoveDown();
	return 0;
}

int ChloeHUD_CareerClassSelect_Up(void* a1) {
	Menu_CareerClassSelect.MoveUp();
	return 0;
}

int ChloeHUD_CareerClassSelect_Down(void* a1) {
	Menu_CareerClassSelect.MoveDown();
	return 0;
}

int ChloeHUD_CareerCupSelect_GetCursorX(void* a1) {
	lua_pushnumber(a1, Menu_CareerCupSelect.nCursorX+1);
	return 1;
}

int ChloeHUD_CareerCupSelect_GetCursorY(void* a1) {
	lua_pushnumber(a1, Menu_CareerCupSelect.nCursorY+1);
	return 1;
}

int ChloeHUD_CareerCupSelect_IsSelectedCupUnlocked(void* a1) {
	if (Menu_CareerCupSelect.nCursorY == 0) {
		auto cup = &gCustomSave.aCareerClasses[Menu_CareerCupSelect.nClass].aCups[Menu_CareerCupSelect.nCursorX];
		lua_pushboolean(a1, cup->bUnlocked);
		return 1;
	}
	else if (Menu_CareerCupSelect.nCursorY == 1) {
		auto cup = &gCustomSave.aCareerClasses[Menu_CareerCupSelect.nClass].Finals;
		lua_pushboolean(a1, cup->bUnlocked);
		return 1;
	}
	else if (Menu_CareerCupSelect.nCursorY == 2) {
		if (Menu_CareerCupSelect.nCursorX >= CareerMode::aLUACareerClasses[Menu_CareerCupSelect.nClass].aEvents.size()) {
			lua_pushboolean(a1, false);
			return 1;
		}

		auto cup = &gCustomSave.aCareerClasses[Menu_CareerCupSelect.nClass].aEvents[Menu_CareerCupSelect.nCursorX];
		lua_pushboolean(a1, cup->bUnlocked);
		return 1;
	}
	lua_pushboolean(a1, false);
	return 1;
}

int ChloeCareer_SetIsCareerRace(void* a1) {
	CareerMode::SetIsCareerMode((int)luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCareer_SetIsCareerTimeTrial(void* a1) {
	CareerMode::SetIsCareerModeTimeTrial((int)luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCareer_WasCareerRace(void* a1) {
	lua_pushboolean(a1, CareerMode::bLastRaceCareerRace);
	return 1;
}

int ChloeCareer_WasCareerRaceFinished(void* a1) {
	lua_pushboolean(a1, CareerMode::aPlayerResults[0].bFinished || CareerMode::aPlayerResults[0].bDNF);
	return 1;
}

int ChloeCareer_WasCareerCupJustFinished(void* a1) {
	lua_pushboolean(a1, CareerMode::bWasCareerCupJustFinished);
	return 1;
}

int ChloeCareer_ClearWasCareerRace(void* a1) {
	CareerMode::bLastRaceCareerRace = false;
	CareerMode::bWasCareerCupJustFinished = false;
	return 0;
}

int ChloeCareer_IsClassUnlocked(void* a1) {
	lua_pushboolean(a1, gCustomSave.bCareerClassUnlocked[((int)luaL_checknumber(a1, 1))-1]);
	return 1;
}

int ChloeCareer_ProcessResultsFromLastRace(void* a1) {
	CareerMode::ProcessResultsFromLastRace_Prompted();
	return 0;
}

int ChloeCareer_GetLastRaceSmashScore(void* a1) {
	auto count = CareerMode::aPlayerResults[0].aSceneryBonuses[(int)luaL_checknumber(a1, 1)];
	auto price = fBonusTypePrice[(int)luaL_checknumber(a1, 1)];
	lua_pushnumber(a1, count);
	lua_pushnumber(a1, count*price);
	return 2;
}

int ChloeCareer_GetLastRaceSmashTotalMoney(void* a1) {
	int money = 0;
	for (int i = 0; i < 10; i++) {
		money += CareerMode::aPlayerResults[0].aSceneryBonuses[i] * fBonusTypePrice[i];
	}
	lua_pushnumber(a1, money);
	return 1;
}

int ChloeCareer_GetLastRaceCrashScore(void* a1) {
	auto count = CareerMode::aPlayerResults[0].aCrashBonuses[(int)luaL_checknumber(a1, 1)-1];
	auto price = CareerMode::GetCrashBonusPrice(luaL_checknumber(a1, 1)-1);
	lua_pushnumber(a1, count);
	lua_pushnumber(a1, count*price);
	return 2;
}

int ChloeCareer_GetLastRaceCrashTotalMoney(void* a1) {
	int money = 0;
	for (int i = 0; i < NUM_CRASHBONUS_TYPES; i++) {
		money += CareerMode::aPlayerResults[0].aCrashBonuses[i] * CareerMode::GetCrashBonusPrice(i);
	}
	lua_pushnumber(a1, money);
	return 1;
}

int ChloeCareer_GetNewlyUnlockedClass(void* a1) {
	lua_pushnumber(a1, CareerMode::nNewlyUnlockedClass);
	return 1;
}

int ChloeCareer_GetNewlyUnlockedCarCount(void* a1) {
	int count = 0;
	for (auto& car : aDealerCars) {
		if (car.classId == CareerMode::nNewlyUnlockedClass) count++;
	}
	lua_pushnumber(a1, count);
	return 1;
}

int ChloeCareer_GetNewlyUnlockedCar(void* a1) {
	std::vector<int> cars;
	for (auto& car : aDealerCars) {
		if (car.classId == CareerMode::nNewlyUnlockedClass) cars.push_back(car.carId);
	}
	lua_pushnumber(a1, cars[(int)luaL_checknumber(a1, 1)-1]);
	return 1;
}

int ChloeCareer_ResetNewlyUnlockedClass(void* a1) {
	CareerMode::nNewlyUnlockedClass = -1;
	return 0;
}

int ChloeHUD_SetCarStats(void* a1) {
	Menu_CarDealer.SetCarStats(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeHUD_SetCarStatsTuned(void* a1) {
	Menu_CarDealer.SetCarStatsTuned(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeHUD_SetCarDescription(void* a1) {
	Menu_CarDealer.sCarDescription = (const char*)lua_tolstring(a1, 1);
	return 0;
}

int ChloeGarage_IsCarPurchased(void* a1) {
	lua_pushboolean(a1, gCustomSave.aCareerGarage[(int)luaL_checknumber(a1, 1)].bIsPurchased);
	return 1;
}

int ChloeGarage_GetCarSkin(void* a1) {
	auto car = &gCustomSave.aCareerGarage[(int)luaL_checknumber(a1, 1)];
	if (car->nSkinId == 0 || car->nSkinId > GetNumSkinsForCar((int)luaL_checknumber(a1, 1))) car->nSkinId = 1;
	lua_pushnumber(a1, car->nSkinId);
	return 1;
}

int ChloeGarage_PurchaseCar(void* a1) {
	auto car = &gCustomSave.aCareerGarage[(int)luaL_checknumber(a1, 1)];
	car->Clear();
	car->bIsPurchased = true;
	car->nSkinId = luaL_checknumber(a1, 2);
	return 0;
}

int ChloeGarage_SellCar(void* a1) {
	auto car = &gCustomSave.aCareerGarage[(int)luaL_checknumber(a1, 1)];
	car->Clear();
	return 0;
}

int ChloeGarage_GetFirstOwnedCar(void* a1) {
	for (int i = 0; i < 255; i++) {
		auto car = &gCustomSave.aCareerGarage[i];
		if (car->bIsPurchased) {
			lua_pushnumber(a1, i);
			return 1;
		}
	}
	lua_pushnumber(a1, 1);
	return 1;
}

int ChloeGarage_GetNumCarsOwned(void* a1) {
	int numCarsOwned = 0;
	for (int i = 0; i < 255; i++) {
		auto car = &gCustomSave.aCareerGarage[i];
		if (car->bIsPurchased) {
			numCarsOwned++;
		}
	}
	lua_pushnumber(a1, numCarsOwned);
	return 1;
}

int ChloeGarage_AddUpgrade(void* a1) {
	auto car = &gCustomSave.aCareerGarage[pGameFlow->Profile.nCarType+1];
	car->SetUpgradePurchased(luaL_checknumber(a1, 1), true);
	car->nUpgradesValue += luaL_checknumber(a1, 2);
	return 0;
}

int ChloeGarage_IsCarUpgraded(void* a1) {
	auto car = &gCustomSave.aCareerGarage[(int)luaL_checknumber(a1, 1)];
	lua_pushboolean(a1, car->IsUpgradePurchased(luaL_checknumber(a1, 2)));
	return 1;
}

int ChloeGarage_GetCarUpgradesValue(void* a1) {
	auto car = &gCustomSave.aCareerGarage[(int)luaL_checknumber(a1, 1)];
	lua_pushnumber(a1, car->nUpgradesValue);
	return 1;
}

int ChloeSave_SuspendCustomData(void* a1) {
	gCustomSave.bInitialized = false;
	return 0;
}

int ChloeSave_ResumeCustomData(void* a1) {
	if (!gCustomSave.bInitialized) {
		gCustomSave.ApplyPlayerSettings();
	}
	gCustomSave.bInitialized = true;
	return 0;
}

int ChloeSave_ClearCustomData(void* a1) {
	gCustomSave.Clear();
	return 0;
}

int ChloeSave_LoadCustomData(void* a1) {
	gCustomSave.Load();
	return 0;
}

int ChloeSave_SaveCustomData(void* a1) {
	DoGameSave();
	return 0;
}

int ChloeSave_DeleteCustomData(void* a1) {
	gCustomSave.Delete(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCollection_WriteLog(void* a1) {
	WriteLog((const char*)lua_tolstring(a1, 1));
	return 0;
}

int ChloeCollection_GetNumDealerCars(void* a1) {
	lua_pushnumber(a1, aDealerCars.size());
	return 1;
}

int ChloeCollection_GetDealerCarId(void* a1) {
	auto car = &aDealerCars[(int)luaL_checknumber(a1, 1)-1];
	lua_pushnumber(a1, car->carId);
	return 1;
}

int ChloeCollection_GetDealerCarName(void* a1) {
	auto car = &aDealerCars[(int)luaL_checknumber(a1, 1)-1];
	lua_pushlstring(a1, (const wchar_t*)car->name.c_str(), car->name.length() + 1);
	return 1;
}

int ChloeCollection_GetDealerCarClass(void* a1) {
	auto car = &aDealerCars[(int)luaL_checknumber(a1, 1)-1];
	lua_pushnumber(a1, car->classId);
	return 1;
}

int ChloeCollection_GetDealerCarCamera(void* a1) {
	auto car = &aDealerCars[(int)luaL_checknumber(a1, 1)-1];
	lua_pushnumber(a1, car->cameraId);
	return 1;
}

int ChloeCollection_GetCarClass(void* a1) {
	auto car = GetDealerCar(luaL_checknumber(a1, 1));
	lua_pushnumber(a1, car->classId);
	return 1;
}

int ChloeCollection_SetLoadingScreenTexture(void* a1) {
	CMenu_LoadingScreen::sTextureName = (const char*)lua_tolstring(a1, 1);
	return 0;
}

int ChloeCollection_SetNumLaps(void* a1) {
	CareerMode::nForceNumLaps = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCareer_IsCupActive(void* a1) {
	lua_pushboolean(a1, CareerMode::IsCupActive());
	return 1;
}

int ChloeCareer_GetCurrentClass(void* a1) {
	lua_pushnumber(a1, gCustomSave.nCareerClass);
	return 1;
}

int ChloeCareer_GetCurrentCup(void* a1) {
	lua_pushnumber(a1, gCustomSave.nCareerCup);
	return 1;
}

int ChloeCareer_GetCurrentCupNextEvent(void* a1) {
	lua_pushnumber(a1, gCustomSave.nCareerCupNextEvent+1);
	return 1;
}

int ChloeCareer_StartCup(void* a1) {
	gCustomSave.nCareerClass = luaL_checknumber(a1, 1);
	gCustomSave.nCareerCup = luaL_checknumber(a1, 2);
	gCustomSave.nCareerCupNextEvent = 0;
	gCustomSave.nCareerEvent = 0;
	memset(gCustomSave.aCareerCupPlayers, 0, sizeof(gCustomSave.aCareerCupPlayers));
	return 0;
}

int ChloeCareer_StartFinal(void* a1) {
	gCustomSave.nCareerClass = luaL_checknumber(a1, 1);
	gCustomSave.nCareerCup = 64;
	gCustomSave.nCareerCupNextEvent = 0;
	gCustomSave.nCareerEvent = 0;
	memset(gCustomSave.aCareerCupPlayers, 0, sizeof(gCustomSave.aCareerCupPlayers));
	return 0;
}

int ChloeCareer_StartEvent(void* a1) {
	gCustomSave.nCareerClass = luaL_checknumber(a1, 1);
	gCustomSave.nCareerCup = 0;
	gCustomSave.nCareerCupNextEvent = 0;
	gCustomSave.nCareerEvent = luaL_checknumber(a1, 2);
	memset(gCustomSave.aCareerCupPlayers, 0, sizeof(gCustomSave.aCareerCupPlayers));
	return 0;
}

int ChloeCareer_ResignCup(void* a1) {
	gCustomSave.nCareerCup = 0;
	gCustomSave.nCareerCupNextEvent = 0;
	gCustomSave.nCareerEvent = 0;
	memset(gCustomSave.aCareerCupPlayers, 0, sizeof(gCustomSave.aCareerCupPlayers));
	DoGameSave();
	return 0;
}

int ChloeCareerDefs_BeginCareerDefs(void* a1) {
	for (auto& luaClass : CareerMode::aLUACareerClasses) {
		luaClass.aCarUnlocks.clear();
		luaClass.aCups.clear();
		luaClass.aEvents.clear();
	}
	return 0;
}

int ChloeCareerDefs_BeginClass(void* a1) {
	CareerMode::luaDefs_currentClass = &CareerMode::aLUACareerClasses[(int)luaL_checknumber(a1, 1)-1];
	return 0;
}

int ChloeCareerDefs_BeginCup(void* a1) {
	auto& vec = CareerMode::luaDefs_currentClass->aCups;
	vec.push_back({});
	CareerMode::luaDefs_currentCup = &vec[vec.size()-1];
	return 0;
}

int ChloeCareerDefs_BeginFinal(void* a1) {
	CareerMode::luaDefs_currentCup = &CareerMode::luaDefs_currentClass->Finals;
	return 0;
}

int ChloeCareerDefs_BeginEvent(void* a1) {
	auto& vec = CareerMode::luaDefs_currentClass->aEvents;
	vec.push_back({});
	CareerMode::luaDefs_currentCup = &vec[vec.size()-1];
	return 0;
}

int ChloeCareerDefs_AddCarUnlock(void* a1) {
	CareerMode::luaDefs_currentClass->aCarUnlocks.push_back(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCareerDefs_SetCupName(void* a1) {
	CareerMode::luaDefs_currentCup->sName = (const char*)lua_tolstring(a1, 1);
	return 0;
}

int ChloeCareerDefs_SetAIUpgradeLevel(void* a1) {
	CareerMode::luaDefs_currentCup->fAIUpgradeLevel = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCareerDefs_SetCupWinnings(void* a1) {
	CareerMode::luaDefs_currentCup->aCupWinnings[0] = luaL_checknumber(a1, 1);
	CareerMode::luaDefs_currentCup->aCupWinnings[1] = luaL_checknumber(a1, 2);
	CareerMode::luaDefs_currentCup->aCupWinnings[2] = luaL_checknumber(a1, 3);
	return 0;
}

int ChloeCareerDefs_AddRace(void* a1) {
	CareerMode::tLUAClass::tCup::tRace race;
	race.nLevel = luaL_checknumber(a1, 1);
	race.nLaps = luaL_checknumber(a1, 2);
	race.nAIHandicapLevel = luaL_checknumber(a1, 3);
	race.bIsDerby = false;
	race.bIsTimeTrial = false;
	CareerMode::luaDefs_currentCup->aRaces.push_back(race);
	return 0;
}

int ChloeCareerDefs_AddDerby(void* a1) {
	CareerMode::tLUAClass::tCup::tRace race;
	race.nLevel = luaL_checknumber(a1, 1);
	race.nLaps = luaL_checknumber(a1, 2);
	race.nAIHandicapLevel = luaL_checknumber(a1, 3);
	race.bIsDerby = true;
	race.bIsTimeTrial = false;
	CareerMode::luaDefs_currentCup->aRaces.push_back(race);
	return 0;
}

int ChloeCareerDefs_AddTimeTrial(void* a1) {
	CareerMode::tLUAClass::tCup::tRace race;
	race.nLevel = luaL_checknumber(a1, 1);
	race.nLaps = luaL_checknumber(a1, 2);
	race.nAIHandicapLevel = 1;
	race.bIsDerby = false;
	race.bIsTimeTrial = true;
	race.nTimeTrialCar = luaL_checknumber(a1, 3);
	CareerMode::luaDefs_currentCup->aRaces.push_back(race);
	return 0;
}

int ChloeProfiles_GetProfileCupsCompleted(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileCupsCompleted(id));
	return 1;
}

int ChloeProfiles_GetProfileCupsMax(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileCupsMax(id));
	return 1;
}

int ChloeProfiles_GetProfileCarsUnlocked(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileCarsUnlocked(id));
	return 1;
}

int ChloeProfiles_GetProfileProgress(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileProgress(id));
	return 1;
}

int ChloeOST_GetSoundtrackName(void* a1) {
	int id = (int)luaL_checknumber(a1, 1);
	if (id < 0 || id >= NewMusicPlayer::aPlaylistsIngame.size()) return 0;
	auto name = NewMusicPlayer::aPlaylistsIngame[id].wsName;
	if (name.empty()) return 0;
	lua_pushlstring(a1, name.c_str(), (name.length() + 1) * 2);
	return 1;
}

int ChloeOST_GetMenuSoundtrackName(void* a1) {
	int id = (int)luaL_checknumber(a1, 1);
	if (id < 0 || id >= NewMusicPlayer::aPlaylistsTitle.size()) return 0;
	auto name = NewMusicPlayer::aPlaylistsTitle[id].wsName;
	if (name.empty()) return 0;
	lua_pushlstring(a1, name.c_str(), (name.length() + 1) * 2);
	return 1;
}

int ChloeOST_GetStuntSoundtrackName(void* a1) {
	int id = (int)luaL_checknumber(a1, 1);
	if (id < 0 || id >= NewMusicPlayer::aPlaylistsStunt.size()) return 0;
	auto name = NewMusicPlayer::aPlaylistsStunt[id].wsName;
	if (name.empty()) return 0;
	lua_pushlstring(a1, name.c_str(), (name.length() + 1) * 2);
	return 1;
}

int ChloeOST_GetNumSoundtracks(void* a1) {
	lua_pushnumber(a1, NewMusicPlayer::aPlaylistsIngame.size());
	return 1;
}

int ChloeOST_GetNumMenuSoundtracks(void* a1) {
	lua_pushnumber(a1, NewMusicPlayer::aPlaylistsTitle.size());
	return 1;
}

int ChloeOST_GetNumStuntSoundtracks(void* a1) {
	lua_pushnumber(a1, NewMusicPlayer::aPlaylistsStunt.size());
	return 1;
}

int ChloeCollection_GetAchievementName(void* a1) {
	auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1));
	if (!achievement) return 0;
	std::wstring str = GetStringWide(achievement->sName);
	lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	return 1;
}

int ChloeCollection_GetAchievementDescription(void* a1) {
	auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1));
	if (!achievement) return 0;
	auto desc = (std::string)achievement->sDescription;
	if (!achievement->bUnlocked && achievement->fMaxInternalProgress > 0) {
		desc += std::format(" ({:.0f}/{})", achievement->fInternalProgress, achievement->fMaxInternalProgress);
	}
	std::wstring str = GetStringWide(desc);
	lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	return 1;
}

int ChloeCollection_GetAchievementProgression(void* a1) {
	auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1));
	if (!achievement) return 0;

	int progress = achievement->nProgress;
	if (progress < 0) progress = 0;
	if (progress > 100) progress = 100;

	if (achievement->bUnlocked) progress = 100;
	else if (progress == 100) progress = 99;

	lua_pushnumber(a1, progress);
	return 1;
}

int ChloeCollection_GetAchievementCompleted(void* a1) {
	auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1));
	if (!achievement) return 0;
	lua_pushboolean(a1, achievement->bUnlocked);
	return 1;
}

int ChloeCollection_GetAchievementInCategory(void* a1) {
	auto achievements = Achievements::GetAchievementsInCategory(luaL_checknumber(a1, 1));
	std::sort(achievements.begin(), achievements.end(), [] (Achievements::CAchievement* a, Achievements::CAchievement* b) { return (std::string)a->sName < (std::string)b->sName; });
	int id = luaL_checknumber(a1, 2)-1;
	if (id < 0 || id >= achievements.size()) return 0;
	std::string str = achievements[id]->sIdentifier;
	lua_pushlstring(a1, (const wchar_t*)str.c_str(), (str.length() + 1));
	return 1;
}

int ChloeCollection_GetNumAchievementsInCategory(void* a1) {
	auto achievements = Achievements::GetAchievementsInCategory(luaL_checknumber(a1, 1));
	lua_pushnumber(a1, achievements.size());
	return 1;
}

int ChloeCollection_SetAchievementTracked(void* a1) {
	if (auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1))) {
		achievement->bTracked = !achievement->bTracked;
	}
	return 0;
}

int ChloeCollection_GetAchievementTrackable(void* a1) {
	if (auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1))) {
		lua_pushboolean(a1, !achievement->bUnlocked && achievement->pTrackFunction != nullptr);
		return 1;
	}
	return 0;
}

int ChloeDatabase_GetCarPerformanceValue(void* a1) {
	auto config = GetCarPerformanceTable(luaL_checknumber(a1, 1));
	auto category = (const char*)lua_tolstring(a1, 2);
	auto keyValue = (const char*)lua_tolstring(a1, 3);
	float outValue;
	CAR_PERFORMANCE(outValue, category, keyValue);
	lua_pushnumber(a1, outValue);
	return 1;
}

int ChloeDatabase_GetCarHorsepowerTuned(void* a1) {
	int carId = luaL_checknumber(a1, 1);
	auto tuning = GetPlayerCareerTuningData(carId);
	auto config = GetCarPerformanceTable(carId);
	float outValue;
	CAR_PERFORMANCE_TUNE(outValue, "Engine", "Engine_Max", "Horsepower", tuning.fHorsepower);
	lua_pushnumber(a1, outValue);
	return 1;
}

int ChloeDatabase_GetCarHorsepowerTunedWithUpgrade(void* a1) {
	int carId = luaL_checknumber(a1, 1);
	auto tuning = GetPlayerCareerTuningData(carId);
	auto config = GetCarPerformanceTable(carId);
	tuning.ApplyUpgrade(luaL_checknumber(a1, 2));
	float outValue;
	CAR_PERFORMANCE_TUNE(outValue, "Engine", "Engine_Max", "Horsepower", tuning.fHorsepower);
	lua_pushnumber(a1, outValue);
	return 1;
}

int ChloeDatabase_GetCarDataValue(void* a1) {
	auto config = GetCarDataTable(luaL_checknumber(a1, 1));
	auto config2 = GetCarPerformanceTable(luaL_checknumber(a1, 1));
	auto category = (const char*)lua_tolstring(a1, 2);
	auto keyValue = (const char*)lua_tolstring(a1, 3);
	float outValue = config[category][keyValue].value_or(-99999.0f);
	if (outValue == -99999.0f) {
		outValue = config2[category][keyValue].value_or(-99999.0f);
	}
	lua_pushnumber(a1, outValue);
	return 1;
}

int ChloeDatabase_GetCarPerformanceString(void* a1) {
	auto config = GetCarPerformanceTable(luaL_checknumber(a1, 1));
	auto category = (const char*)lua_tolstring(a1, 2);
	auto keyValue = (const char*)lua_tolstring(a1, 3);
	auto str = (std::string)config[category][keyValue].value_or("NULL");
	lua_pushlstring(a1, (const wchar_t*)str.c_str(), (str.length() + 1));
	return 1;
}

int ChloeDatabase_GetCarDataString(void* a1) {
	auto config = GetCarDataTable(luaL_checknumber(a1, 1));
	auto category = (const char*)lua_tolstring(a1, 2);
	auto keyValue = (const char*)lua_tolstring(a1, 3);
	auto str = (std::string)config[category][keyValue].value_or("NULL");
	lua_pushlstring(a1, (const wchar_t*)str.c_str(), (str.length() + 1));
	return 1;
}

void RegisterLUAFunction(void* a1, void* function, const char* name) {
	lua_setglobal(a1, name);
	lua_pushcfunction(a1, function, 0);
	lua_settable(a1, LUA_ENVIRONINDEX);
}

void RegisterLUAEnum(void* a1, int id, const char* name) {
	lua_setglobal(a1, name);
	lua_pushnumber(a1, id);
	lua_settable(a1, LUA_ENVIRONINDEX);
}

void CustomLUAFunctions(void* a1) {
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetAspect, "ChloeWidescreen_GetAspect");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_LeftJustify, "ChloeWidescreen_LeftJustify");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_SafeLeftJustify, "ChloeWidescreen_SafeLeftJustify");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_RightJustify, "ChloeWidescreen_RightJustify");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_SafeRightJustify, "ChloeWidescreen_SafeRightJustify");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetCenter, "ChloeWidescreen_GetCenter");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetLeft, "ChloeWidescreen_GetLeft");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetSafeLeft, "ChloeWidescreen_GetSafeLeft");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetRight, "ChloeWidescreen_GetRight");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetSafeRight, "ChloeWidescreen_GetSafeRight");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_HasSafeZone, "ChloeWidescreen_HasSafeZone");
	//RegisterLUAFunction(a1, (void*)&ChloeWidescreen_WasWidescreenToggled, "ChloeWidescreen_WasWidescreenToggled");
	RegisterLUAFunction(a1, (void*)&ChloeSkins_GetNumSkinsForCar, "ChloeSkins_GetNumSkinsForCar");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetRandom, "ChloeCollection_GetRandom");
	//RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarCustomMenuBG, "ChloeCollection_GetCarCustomMenuBG");
	RegisterLUAFunction(a1, (void*)&ChloeMenu_EnterMenu, "ChloeMenu_EnterMenu");
	RegisterLUAFunction(a1, (void*)&ChloeMenu_ExitMenu, "ChloeMenu_ExitMenu");
	RegisterLUAFunction(a1, (void*)&ChloeMenu_MoveLeft, "ChloeMenu_MoveLeft");
	RegisterLUAFunction(a1, (void*)&ChloeMenu_MoveRight, "ChloeMenu_MoveRight");
	RegisterLUAFunction(a1, (void*)&ChloeMenu_MoveUp, "ChloeMenu_MoveUp");
	RegisterLUAFunction(a1, (void*)&ChloeMenu_MoveDown, "ChloeMenu_MoveDown");
	RegisterLUAFunction(a1, (void*)&ChloeSkins_ShowSkinSelector, "ChloeSkins_ShowSkinSelector");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_GetCursorX, "ChloeHUD_CareerCupSelect_GetCursorX");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_GetCursorY, "ChloeHUD_CareerCupSelect_GetCursorY");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_IsSelectedCupUnlocked, "ChloeHUD_CareerCupSelect_IsSelectedCupUnlocked");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_SetCarStats, "ChloeHUD_SetCarStats");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_SetCarStatsTuned, "ChloeHUD_SetCarStatsTuned");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_SetCarDescription, "ChloeHUD_SetCarDescription");
	RegisterLUAFunction(a1, (void*)&ChloeSave_ClearCustomData, "ChloeSave_ClearCustomData");
	RegisterLUAFunction(a1, (void*)&ChloeSave_LoadCustomData, "ChloeSave_LoadCustomData");
	RegisterLUAFunction(a1, (void*)&ChloeSave_SaveCustomData, "ChloeSave_SaveCustomData");
	RegisterLUAFunction(a1, (void*)&ChloeSave_DeleteCustomData, "ChloeSave_DeleteCustomData");
	RegisterLUAFunction(a1, (void*)&ChloeSave_SuspendCustomData, "ChloeSave_SuspendCustomData");
	RegisterLUAFunction(a1, (void*)&ChloeSave_ResumeCustomData, "ChloeSave_ResumeCustomData");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_IsCarPurchased, "ChloeGarage_IsCarPurchased");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_GetCarSkin, "ChloeGarage_GetCarSkin");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_PurchaseCar, "ChloeGarage_PurchaseCar");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_SellCar, "ChloeGarage_SellCar");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_GetFirstOwnedCar, "ChloeGarage_GetFirstOwnedCar");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_GetNumCarsOwned, "ChloeGarage_GetNumCarsOwned");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_AddUpgrade, "ChloeGarage_AddUpgrade");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_IsCarUpgraded, "ChloeGarage_IsCarUpgraded");
	RegisterLUAFunction(a1, (void*)&ChloeGarage_GetCarUpgradesValue, "ChloeGarage_GetCarUpgradesValue");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_WriteLog, "ChloeCollection_WriteLog");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetNumDealerCars, "ChloeCollection_GetNumDealerCars");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetDealerCarId, "ChloeCollection_GetDealerCarId");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetDealerCarName, "ChloeCollection_GetDealerCarName");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetDealerCarClass, "ChloeCollection_GetDealerCarClass");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetDealerCarCamera, "ChloeCollection_GetDealerCarCamera");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarClass, "ChloeCollection_GetCarClass");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetLoadingScreenTexture, "ChloeCollection_SetLoadingScreenTexture");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetNumLaps, "ChloeCollection_SetNumLaps");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_IsCupActive, "ChloeCareer_IsCupActive");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetCurrentClass, "ChloeCareer_GetCurrentClass");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetCurrentCup, "ChloeCareer_GetCurrentCup");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetCurrentCupNextEvent, "ChloeCareer_GetCurrentCupNextEvent");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_StartCup, "ChloeCareer_StartCup");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_StartFinal, "ChloeCareer_StartFinal");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_StartEvent, "ChloeCareer_StartEvent");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_ResignCup, "ChloeCareer_ResignCup");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_SetIsCareerRace, "ChloeCareer_SetIsCareerRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_SetIsCareerTimeTrial, "ChloeCareer_SetIsCareerTimeTrial");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_WasCareerRace, "ChloeCareer_WasCareerRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_WasCareerRaceFinished, "ChloeCareer_WasCareerRaceFinished");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_WasCareerCupJustFinished, "ChloeCareer_WasCareerCupJustFinished");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_ClearWasCareerRace, "ChloeCareer_ClearWasCareerRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_IsClassUnlocked, "ChloeCareer_IsClassUnlocked");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_ProcessResultsFromLastRace, "ChloeCareer_ProcessResultsFromLastRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetLastRaceSmashScore, "ChloeCareer_GetLastRaceSmashScore");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetLastRaceSmashTotalMoney, "ChloeCareer_GetLastRaceSmashTotalMoney");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetLastRaceCrashScore, "ChloeCareer_GetLastRaceCrashScore");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetLastRaceCrashTotalMoney, "ChloeCareer_GetLastRaceCrashTotalMoney");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetNewlyUnlockedClass, "ChloeCareer_GetNewlyUnlockedClass");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_ResetNewlyUnlockedClass, "ChloeCareer_ResetNewlyUnlockedClass");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetNewlyUnlockedCarCount, "ChloeCareer_GetNewlyUnlockedCarCount");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetNewlyUnlockedCar, "ChloeCareer_GetNewlyUnlockedCar");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginCareerDefs, "ChloeCareerDefs_BeginCareerDefs");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginClass, "ChloeCareerDefs_BeginClass");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginCup, "ChloeCareerDefs_BeginCup");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginFinal, "ChloeCareerDefs_BeginFinal");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginEvent, "ChloeCareerDefs_BeginEvent");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_AddCarUnlock, "ChloeCareerDefs_AddCarUnlock");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_SetCupName, "ChloeCareerDefs_SetCupName");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_SetAIUpgradeLevel, "ChloeCareerDefs_SetAIUpgradeLevel");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_SetCupWinnings, "ChloeCareerDefs_SetCupWinnings");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_AddRace, "ChloeCareerDefs_AddRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_AddDerby, "ChloeCareerDefs_AddDerby");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_AddTimeTrial, "ChloeCareerDefs_AddTimeTrial");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileCupsCompleted, "ChloeProfiles_GetProfileCupsCompleted");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileCupsMax, "ChloeProfiles_GetProfileCupsMax");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileCarsUnlocked, "ChloeProfiles_GetProfileCarsUnlocked");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileProgress, "ChloeProfiles_GetProfileProgress");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetSoundtrackName, "ChloeOST_GetSoundtrackName");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetMenuSoundtrackName, "ChloeOST_GetMenuSoundtrackName");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetStuntSoundtrackName, "ChloeOST_GetStuntSoundtrackName");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetNumSoundtracks, "ChloeOST_GetNumSoundtracks");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetNumMenuSoundtracks, "ChloeOST_GetNumMenuSoundtracks");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetNumStuntSoundtracks, "ChloeOST_GetNumStuntSoundtracks");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementName, "ChloeCollection_GetAchievementName");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementDescription, "ChloeCollection_GetAchievementDescription");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementProgression, "ChloeCollection_GetAchievementProgression");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementCompleted, "ChloeCollection_GetAchievementCompleted");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementInCategory, "ChloeCollection_GetAchievementInCategory");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetNumAchievementsInCategory, "ChloeCollection_GetNumAchievementsInCategory");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetAchievementTracked, "ChloeCollection_SetAchievementTracked");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementTrackable, "ChloeCollection_GetAchievementTrackable");
	RegisterLUAFunction(a1, (void*)&ChloeDatabase_GetCarPerformanceValue, "ChloeDatabase_GetCarPerformanceValue");
	RegisterLUAFunction(a1, (void*)&ChloeDatabase_GetCarHorsepowerTuned, "ChloeDatabase_GetCarHorsepowerTuned");
	RegisterLUAFunction(a1, (void*)&ChloeDatabase_GetCarHorsepowerTunedWithUpgrade, "ChloeDatabase_GetCarHorsepowerTunedWithUpgrade");
	RegisterLUAFunction(a1, (void*)&ChloeDatabase_GetCarDataValue, "ChloeDatabase_GetCarDataValue");
	RegisterLUAFunction(a1, (void*)&ChloeDatabase_GetCarPerformanceString, "ChloeDatabase_GetCarPerformanceString");
	RegisterLUAFunction(a1, (void*)&ChloeDatabase_GetCarDataString, "ChloeDatabase_GetCarDataString");

	RegisterLUAEnum(a1, Achievements::CAT_GENERAL, "ACHIEVEMENTS_GENERAL");
	RegisterLUAEnum(a1, Achievements::CAT_SINGLEPLAYER, "ACHIEVEMENTS_SINGLEPLAYER");
	RegisterLUAEnum(a1, Achievements::CAT_MULTIPLAYER, "ACHIEVEMENTS_MULTIPLAYER");
	RegisterLUAEnum(a1, Achievements::CAT_CAREER, "ACHIEVEMENTS_CAREER");
	RegisterLUAEnum(a1, Achievements::CAT_GAMEMODES, "ACHIEVEMENTS_GAMEMODES");
	RegisterLUAEnum(a1, Achievements::CAT_TRACKS, "ACHIEVEMENTS_TRACKS");
	RegisterLUAEnum(a1, Achievements::CAT_HIDDEN, "ACHIEVEMENTS_HIDDEN");

	RegisterLUAEnum(a1, HANDLING_NORMAL, "HANDLING_NORMAL");
	RegisterLUAEnum(a1, HANDLING_PROFESSIONAL, "HANDLING_PROFESSIONAL");
	RegisterLUAEnum(a1, HANDLING_HARDCORE, "HANDLING_HARDCORE");

	static auto sVersionString = "Chloe Collection v1.04 - Time Trial Edition";
	lua_setglobal(a1, "ChloeCollectionVersion");
	lua_setglobal(a1, sVersionString);
	lua_settable(a1, LUA_ENVIRONINDEX);
}

void ApplyLUAPatches() {
	NyaFO2Hooks::PlaceScriptHook();
	NyaFO2Hooks::aScriptFuncs.push_back(CustomLUAFunctions);
}