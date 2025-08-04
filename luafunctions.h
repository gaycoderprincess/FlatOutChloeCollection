auto GetStringWide(const std::string& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(string);
}

auto GetStringNarrow(const std::wstring& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(string);
}

int ChloeSkins_GetNumSkinsForCar(void* a1) {
	lua_pushnumber(a1, GetNumSkinsForCar(luaL_checknumber(a1, 1)));
	return 1;
}

int ChloeSkins_IsSkinCustom(void* a1) {
	static auto config = toml::parse_file("Config/CarSkins.toml");
	int carId = (int)luaL_checknumber(a1, 1);
	int skinId = (int)luaL_checknumber(a1, 2);
	bool wrapAround = luaL_checknumber(a1, 3);
	int numSkins = GetNumSkinsForCar(carId);
	if (!wrapAround && (skinId < 1 || skinId > numSkins)) {
		lua_pushboolean(a1, false);
		return 1;
	}
	// wrap around
	while (skinId < 1) {
		skinId += numSkins;
	}
	while (skinId > numSkins) {
		skinId -= numSkins;
	}
	std::wstring author = config["car_" + std::to_string(carId)]["skin" + std::to_string(skinId)].value_or(L"");
	lua_pushboolean(a1, !author.empty());
	return 1;
}

int ChloeCollection_GetRandom(void* a1) {
	int r = rand() % (int)luaL_checknumber(a1, 1);
	lua_pushnumber(a1, r);
	return 1;
}

int ChloeSkins_ShowSkinSelector(void* a1) {
	NewMenuHud::bInSkinSelector = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeHUD_SetInCarDealer(void* a1) {
	NewMenuHud::bInCarDealer = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeHUD_SetInCareerCupSelect(void* a1) {
	NewMenuHud::bInCareerCupSelect = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeHUD_CareerCupSelect_Left(void* a1) {
	NewMenuHud::CareerCupSelect_MoveLeft();
	return 0;
}

int ChloeHUD_CareerCupSelect_Right(void* a1) {
	NewMenuHud::CareerCupSelect_MoveRight();
	return 0;
}

int ChloeHUD_CareerCupSelect_Up(void* a1) {
	NewMenuHud::CareerCupSelect_MoveUp();
	return 0;
}

int ChloeHUD_CareerCupSelect_Down(void* a1) {
	NewMenuHud::CareerCupSelect_MoveDown();
	return 0;
}

int ChloeHUD_CareerCupSelect_GetCursorX(void* a1) {
	lua_pushnumber(a1, NewMenuHud::nCareerCupSelectCursorX+1);
	return 1;
}

int ChloeHUD_CareerCupSelect_GetCursorY(void* a1) {
	lua_pushnumber(a1, NewMenuHud::nCareerCupSelectCursorY+1);
	return 1;
}

int ChloeHUD_CareerCupSelect_IsSelectedCupUnlocked(void* a1) {
	if (NewMenuHud::nCareerCupSelectCursorY == 0) {
		auto cup = &gCustomSave.aCareerClasses[NewMenuHud::nCareerCupSelectClass].aCups[NewMenuHud::nCareerCupSelectCursorX];
		lua_pushboolean(a1, cup->bUnlocked);
		return 1;
	}
	else if (NewMenuHud::nCareerCupSelectCursorY == 1) {
		auto cup = &gCustomSave.aCareerClasses[NewMenuHud::nCareerCupSelectClass].Finals;
		lua_pushboolean(a1, cup->bUnlocked);
		return 1;
	}
	else if (NewMenuHud::nCareerCupSelectCursorY == 2) {
		if (NewMenuHud::nCareerCupSelectCursorX >= CareerMode::aLUACareerClasses[NewMenuHud::nCareerCupSelectClass].aEvents.size()) {
			lua_pushboolean(a1, false);
			return 1;
		}

		auto cup = &gCustomSave.aCareerClasses[NewMenuHud::nCareerCupSelectClass].aEvents[NewMenuHud::nCareerCupSelectCursorX];
		lua_pushboolean(a1, cup->bUnlocked);
		return 1;
	}
	lua_pushboolean(a1, false);
	return 1;
}

int ChloeHUD_SetInCareer(void* a1) {
	NewMenuHud::bInCareer = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCareer_SetIsCareerRace(void* a1) {
	CareerMode::SetIsCareerMode((int)luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCareer_WasCareerRace(void* a1) {
	lua_pushboolean(a1, CareerMode::bLastRaceCareerRace);
	return 1;
}

int ChloeCareer_ClearWasCareerRace(void* a1) {
	CareerMode::bLastRaceCareerRace = false;
	return 0;
}

int ChloeHUD_SetCarStats(void* a1) {
	NewMenuHud::nCarHorsepower = luaL_checknumber(a1, 1);
	NewMenuHud::nCarWeight = luaL_checknumber(a1, 2);
	NewMenuHud::nCarPrice = luaL_checknumber(a1, 3);
	NewMenuHud::sCarName = GetCarName(pGameFlow->pMenuInterface->pMenuScene->nCar);
	return 0;
}

int ChloeHUD_SetCarDescription(void* a1) {
	NewMenuHud::sCarDescription = (const char*)lua_tolstring(a1, 1);
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
	return 0;
}

int ChloeGarage_IsCarUpgraded(void* a1) {
	auto car = &gCustomSave.aCareerGarage[(int)luaL_checknumber(a1, 1)];
	lua_pushboolean(a1, car->IsUpgradePurchased(luaL_checknumber(a1, 2)));
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
	gCustomSave.Save();
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
	NewMenuHud::SetLoadingScreenTexture((const char*)lua_tolstring(a1, 1));
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
	memset(gCustomSave.aCareerCupPlayers, 0, sizeof(gCustomSave.aCareerCupPlayers));
	return 0;
}

int ChloeCareer_StartFinal(void* a1) {
	gCustomSave.nCareerClass = luaL_checknumber(a1, 1);
	gCustomSave.nCareerCup = 64;
	gCustomSave.nCareerCupNextEvent = 0;
	memset(gCustomSave.aCareerCupPlayers, 0, sizeof(gCustomSave.aCareerCupPlayers));
	return 0;
}

int ChloeCareer_StartEvent(void* a1) {
	gCustomSave.nCareerClass = luaL_checknumber(a1, 1);
	gCustomSave.nCareerEvent = luaL_checknumber(a1, 2);
	gCustomSave.nCareerCupNextEvent = 0;
	memset(gCustomSave.aCareerCupPlayers, 0, sizeof(gCustomSave.aCareerCupPlayers));
	return 0;
}

int ChloeCareerDefs_BeginCareerDefs(void* a1) {
	for (auto& luaClass : CareerMode::aLUACareerClasses) {
		luaClass.nMoneyAward = 0;
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

int ChloeCareerDefs_SetMoneyAward(void* a1) {
	CareerMode::luaDefs_currentClass->nMoneyAward = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCareerDefs_SetCupName(void* a1) {
	CareerMode::luaDefs_currentCup->sName = (const char*)lua_tolstring(a1, 1);
	return 0;
}

int ChloeCareerDefs_SetAIUpgradeLevel(void* a1) {
	CareerMode::luaDefs_currentCup->nAIUpgradeLevel = luaL_checknumber(a1, 1);
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
	race.nAIHandicapLevel = luaL_checknumber(a1, 3);
	race.bIsDerby = false;
	race.bIsTimeTrial = true;
	CareerMode::luaDefs_currentCup->aRaces.push_back(race);
	return 0;
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
	NewMusicPlayer::Init();

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
	RegisterLUAFunction(a1, (void*)&ChloeSkins_IsSkinCustom, "ChloeSkins_IsSkinCustom");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetRandom, "ChloeCollection_GetRandom");
	//RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarCustomMenuBG, "ChloeCollection_GetCarCustomMenuBG");
	RegisterLUAFunction(a1, (void*)&ChloeSkins_ShowSkinSelector, "ChloeSkins_ShowSkinSelector");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_SetInCareer, "ChloeHUD_SetInCareer");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_SetInCarDealer, "ChloeHUD_SetInCarDealer");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_SetInCareerCupSelect, "ChloeHUD_SetInCareerCupSelect");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_Left, "ChloeHUD_CareerCupSelect_Left");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_Right, "ChloeHUD_CareerCupSelect_Right");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_Up, "ChloeHUD_CareerCupSelect_Up");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_Down, "ChloeHUD_CareerCupSelect_Down");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_GetCursorX, "ChloeHUD_CareerCupSelect_GetCursorX");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_GetCursorY, "ChloeHUD_CareerCupSelect_GetCursorY");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_CareerCupSelect_IsSelectedCupUnlocked, "ChloeHUD_CareerCupSelect_IsSelectedCupUnlocked");
	RegisterLUAFunction(a1, (void*)&ChloeHUD_SetCarStats, "ChloeHUD_SetCarStats");
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
	RegisterLUAFunction(a1, (void*)&ChloeCollection_WriteLog, "ChloeCollection_WriteLog");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetNumDealerCars, "ChloeCollection_GetNumDealerCars");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetDealerCarId, "ChloeCollection_GetDealerCarId");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetDealerCarName, "ChloeCollection_GetDealerCarName");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetDealerCarClass, "ChloeCollection_GetDealerCarClass");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetDealerCarCamera, "ChloeCollection_GetDealerCarCamera");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarClass, "ChloeCollection_GetCarClass");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetLoadingScreenTexture, "ChloeCollection_SetLoadingScreenTexture");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_IsCupActive, "ChloeCareer_IsCupActive");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetCurrentClass, "ChloeCareer_GetCurrentClass");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetCurrentCup, "ChloeCareer_GetCurrentCup");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_GetCurrentCupNextEvent, "ChloeCareer_GetCurrentCupNextEvent");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_StartCup, "ChloeCareer_StartCup");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_StartFinal, "ChloeCareer_StartFinal");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_StartEvent, "ChloeCareer_StartEvent");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_SetIsCareerRace, "ChloeCareer_SetIsCareerRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_WasCareerRace, "ChloeCareer_WasCareerRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareer_ClearWasCareerRace, "ChloeCareer_ClearWasCareerRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginCareerDefs, "ChloeCareerDefs_BeginCareerDefs");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginClass, "ChloeCareerDefs_BeginClass");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginCup, "ChloeCareerDefs_BeginCup");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginFinal, "ChloeCareerDefs_BeginFinal");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_BeginEvent, "ChloeCareerDefs_BeginEvent");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_AddCarUnlock, "ChloeCareerDefs_AddCarUnlock");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_SetMoneyAward, "ChloeCareerDefs_SetMoneyAward");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_SetCupName, "ChloeCareerDefs_SetCupName");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_SetAIUpgradeLevel, "ChloeCareerDefs_SetAIUpgradeLevel");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_SetCupWinnings, "ChloeCareerDefs_SetCupWinnings");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_AddRace, "ChloeCareerDefs_AddRace");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_AddDerby, "ChloeCareerDefs_AddDerby");
	RegisterLUAFunction(a1, (void*)&ChloeCareerDefs_AddTimeTrial, "ChloeCareerDefs_AddTimeTrial");

	RegisterLUAEnum(a1, HANDLING_NORMAL, "HANDLING_NORMAL");
	RegisterLUAEnum(a1, HANDLING_PROFESSIONAL, "HANDLING_PROFESSIONAL");
	RegisterLUAEnum(a1, HANDLING_HARDCORE, "HANDLING_HARDCORE");

	static auto sVersionString = "Chloe's Collection v1.00 - Flat-Out Edition";
	lua_setglobal(a1, "ChloeCollectionVersion");
	lua_setglobal(a1, sVersionString);
	lua_settable(a1, LUA_ENVIRONINDEX);
}

void ApplyLUAPatches() {
	NyaFO2Hooks::PlaceScriptHook();
	NyaFO2Hooks::aScriptFuncs.push_back(CustomLUAFunctions);
}