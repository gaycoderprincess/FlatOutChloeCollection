float fCarnageModeMassFudge = 0.75;

toml::table GetCarPerformanceTable(int id) {
	return ReadTOMLFromBfs(std::format("data/database/cars/car{}.toml", GetDealerCar(id)->performanceId));
}

toml::table GetCarDataTable(int id) {
	return ReadTOMLFromBfs(std::format("data/database/cars/car{}.toml", id));
}

toml::table GetCarTireTable(int id) {
	auto perf = GetCarDataTable(id);
	auto tirePath = perf["Data"]["Tires"].value_or("tire1");
	return ReadTOMLFromBfs(std::format("data/database/cars/{}.toml", tirePath));
}

toml::table GetTireDynamicsTable() {
	return ReadTOMLFromBfs("data/database/tiredynamics.toml");
}

// tuning lerp values - 0 to 1
struct tCarTuningData {
	// body
	float fBrakePower;
	float fDurability;

	// engine
	float fTurboAcceleration;
	float fInertiaEngine;
	float fInertiaDriveShaft;
	float fEngineFriction;
	float fTorqueMax;
	float fHorsepower; // visual

	// gearbox
	float fClutchReleaseTime;
	//float fGearShiftTime;
	float fClutchEngageTime;
	float fClutchTorque;
	float fEndRatio;

	// suspension
	float fRestLength;
	float fDefaultCompression;
	float fBumpDamp;
	float fReboundDamp;
	float fBumperRestitution;
	float fRollBarStiffness;

	tCarTuningData() {
		memset(this,0,sizeof(*this));
	}

	void ApplyUpgrade(int upgrade) {
		static auto config = ReadTOMLFromBfs("data/database/upgrades.toml");
		auto category = std::format("Upgrade{}", upgrade);
		fBrakePower += config[category]["BrakePower"].value_or(0.0);
		fDurability += config[category]["Durability"].value_or(0.0);
		fTurboAcceleration += config[category]["TurboAcceleration"].value_or(0.0);
		fInertiaEngine += config[category]["InertiaEngine"].value_or(0.0);
		fInertiaDriveShaft += config[category]["InertiaDriveShaft"].value_or(0.0);
		fEngineFriction += config[category]["EngineFriction"].value_or(0.0);
		fTorqueMax += config[category]["TorqueMax"].value_or(0.0);
		fHorsepower += config[category]["Horsepower"].value_or(0.0);
		fClutchReleaseTime += config[category]["ClutchReleaseTime"].value_or(0.0);
		//fGearShiftTime += config[category]["GearShiftTime"].value_or(0.0);
		fClutchEngageTime += config[category]["ClutchEngageTime"].value_or(0.0);
		fClutchTorque += config[category]["ClutchTorque"].value_or(0.0);
		fEndRatio += config[category]["EndRatio"].value_or(0.0);
		fRestLength += config[category]["RestLength"].value_or(0.0);
		fDefaultCompression += config[category]["DefaultCompression"].value_or(0.0);
		fBumpDamp += config[category]["BumpDamp"].value_or(0.0);
		fReboundDamp += config[category]["ReboundDamp"].value_or(0.0);
		fBumperRestitution += config[category]["BumperRestitution"].value_or(0.0);
		fRollBarStiffness += config[category]["RollBarStiffness"].value_or(0.0);
	}

	void SetAllUpgrades(float value) {
		for (int i = 0; i < sizeof(*this)/4; i++) {
			auto f = (float*)this;
			f[i] = value;
		}
		if (pGameFlow->nEventType != eEventType::DERBY) {
			fDurability = 0.0; // slight hack to make opponents health lower
		}
	}

	float GetUpgradePercentage() {
		float upgrades = 0;
		for (int i = 0; i < sizeof(*this)/4; i++) {
			auto f = (float*)this;
			upgrades += f[i];
		}
		return upgrades / (double)(sizeof(*this)/4);
	}
};

tCarTuningData GetPlayerCareerTuningData(int carId) {
	tCarTuningData data;
	auto& tuning = gCustomSave.aCareerGarage[carId];
	for (int i = 0; i < PlayerProfile::NUM_UPGRADES; i++) {
		if (!tuning.IsUpgradePurchased(i)) continue;
		data.ApplyUpgrade(i);
	}
	return data;
}

tCarTuningData GetAITuningData(bool usePlayerUpgradeLevel) {
	tCarTuningData data;
	data.SetAllUpgrades(usePlayerUpgradeLevel ? CareerMode::GetPlayerUpgradeLevel() : CareerMode::GetAIUpgradeLevel());
	WriteLogDebug("HANDLING", std::format("Using lerp upgrade level {}", data.fHorsepower));
	return data;
}

tCarTuningData GetPlayerTuningData(int carId) {
	if (CareerMode::bIsCareerRace && !CareerMode::IsCareerTimeTrial()) {
		return GetPlayerCareerTuningData(carId);
	}

	auto data = GetAITuningData(true);
	data.fDurability = CareerMode::GetPlayerUpgradeLevel();
	if (CarnageRace::bIsCarnageRace) data.fDurability = 1.0;
	return data;
}

tCarTuningData GetTuningDataForCar(Car* pCar) {
	if (pCar->pPlayer->nPlayerType == PLAYERTYPE_LOCAL) return GetPlayerTuningData(pCar->pPlayer->nCarId+1);
	return GetAITuningData(false);
}

#define CAR_PERFORMANCE(value, category, name) value = config[category][name].value_or(-99999.0f); if (value == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{} = {}", category, name, value));
#define CAR_PERFORMANCE_ARRAY(value, category, name, arrayCount) for (int j = 0; j < arrayCount; j++) { value[j] = config[category][name][j].value_or(-99999.0f); if (value[j] == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{}[{}] = {}", category, name, j+1, value[j])); }
#define CAR_PERFORMANCE_FALLBACK(value, category, name, default) value = config[category][name].value_or(default); WriteLogDebug("HANDLING", std::format("{}.{} = {}", category, name, value));
#define CAR_PERFORMANCE_TUNE(value, category, category_max, name, tuningValue) value = std::lerp(config[category][name].value_or(-99999.0f), config[category_max][name].value_or(config[category][name].value_or(0.0)), tuningValue); if (value == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{} = {} (tuned)", category, name, value));

#define TIRE_PERFORMANCE(value, category, name) value = global[category][name].value_or(-99999.0f); if (value == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{} = {}", category, name, value));
#define TIRE_PERFORMANCE_ARRAY(value, category, name, arrayCount) for (int j = 0; j < arrayCount; j++) { value[j] = global[category][name][j].value_or(-99999.0f); if (value[j] == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{}[{}] = {}", category, name, j+1, value[j])); }
#define TIRE_PERFORMANCE_FALLBACK(value, category, name, default) value = global[category][name].value_or(default); WriteLogDebug("HANDLING", std::format("{}.{} = {}", category, name, value));
#define TIRE_PERFORMANCE_TUNE(value, category, category_max, name, tuningValue) value = std::lerp(global[category][name].value_or(-99999.0f), config[category_max][name].value_or(config[category][name].value_or(0.0)), tuningValue); if (value == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{} = {} (tuned)", category, name, value));

#define CAR_DATA(value, category, name) value = data[category][name].value_or(-99999.0f); if (value == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{} = {}", category, name, value));
#define CAR_DATA_ARRAY(value, category, name, arrayCount) for (int j = 0; j < arrayCount; j++) { value[j] = data[category][name][j].value_or(-99999.0f); if (value[j] == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{}[{}] = {}", category, name, j+1, value[j])); }
#define CAR_DATA_FALLBACK(value, category, name, default) value = data[category][name].value_or(default); WriteLogDebug("HANDLING", std::format("{}.{} = {}", category, name, value));
#define CAR_DATA_TUNE(value, category, category_max, name, tuningValue) value = std::lerp(data[category][name].value_or(-99999.0f), data[category_max][name].value_or(data[category][name].value_or(0.0)), tuningValue); if (value == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); } WriteLogDebug("HANDLING", std::format("{}.{} = {} (tuned)", category, name, value));

void __stdcall LoadCarEngine(Engine* engine) {
	WriteLogDebug("HANDLING", std::format("Initing engine for car{} for {}", engine->pPerformance->pCar->pPlayer->nCarId+1,
							  GetStringNarrow(engine->pPerformance->pCar->pPlayer->sPlayerName.Get())));

	auto config = GetCarPerformanceTable(engine->pPerformance->pCar->pPlayer->nCarId+1);

	auto tuning = GetTuningDataForCar(engine->pPerformance->pCar);
	CAR_PERFORMANCE_TUNE(engine->fInertiaEngine, "Engine", "Engine_Max", "InertiaEngine", tuning.fInertiaEngine);
	CAR_PERFORMANCE_TUNE(engine->fEngineFriction, "Engine", "Engine_Max", "EngineFriction", tuning.fEngineFriction);
	CAR_PERFORMANCE(engine->fIdleRpm, "Engine", "IdleRpm");
	CAR_PERFORMANCE_FALLBACK(engine->fZeroPowerRpm, "Engine", "ZeroPowerRpm", engine->fIdleRpm * 0.5);
	CAR_PERFORMANCE_TUNE(engine->fTorqueMax, "Engine", "Engine_Max", "TorqueMax", tuning.fTorqueMax);
	CAR_PERFORMANCE(engine->nTorqueCurveParams, "Engine", "TorqueCurveParams");
	for (int i = 0; i < 20; i++) {
		engine->aTorque[i].fRpm = config["Engine"][std::format("Torque{:02}", i)][0].value_or(0.0);
		engine->aTorque[i].fCurve = config["Engine"][std::format("Torque{:02}", i)][1].value_or(0.0);
	}
	CAR_PERFORMANCE(engine->fExhaustPeakRpm, "Engine", "ExhaustPeakRpm");
	CAR_PERFORMANCE_TUNE(engine->fTurboAcceleration, "Engine", "Engine_Max", "TurboAcceleration", tuning.fTurboAcceleration);
	CAR_PERFORMANCE(engine->fNitroAcceleration, "Engine", "NitroAcceleration");
	CAR_PERFORMANCE_FALLBACK(engine->fMaxNitro, "Engine", "NitroStorage", 6.0);
	CAR_PERFORMANCE(engine->fMass, "Engine", "Mass");

	engine->fInvInertiaEngine = 1.0 / engine->fInertiaEngine;
	engine->fInertiaEngine2 = engine->fInertiaEngine;

	// i have no idea what any of this means, someone help me out?
	auto v37 = Engine::GetEngineSpeed(*(uint32_t*)&engine->fExhaustPeakRpm, engine, engine->fIdleRpm);
	auto v92 = -(engine->fIdleRpm * engine->fEngineFriction);
	engine->fUnknown_100 = -1.0 / (v37 * engine->fTorqueMax - v92) * v92;
	engine->fUnknown_104 = 1.0 - engine->fUnknown_100;
}

void __stdcall LoadCarGearbox(Gearbox* gearbox) {
	WriteLogDebug("HANDLING", std::format("Initing gearbox for car{} for {}", gearbox->pPerformance->pCar->pPlayer->nCarId+1,
							  GetStringNarrow(gearbox->pPerformance->pCar->pPlayer->sPlayerName.Get())));

	auto config = GetCarPerformanceTable(gearbox->pPerformance->pCar->pPlayer->nCarId+1);

	auto tuning = GetTuningDataForCar(gearbox->pPerformance->pCar);
	CAR_PERFORMANCE(gearbox->nNumGears, "Gearbox", "NumGears");
	CAR_PERFORMANCE(gearbox->fGearRRatio, "Gearbox", "GearRRatio");
	CAR_PERFORMANCE(gearbox->fGearNRatio, "Gearbox", "Gear1Ratio");
	CAR_PERFORMANCE(gearbox->fGear1Ratio, "Gearbox", "Gear1Ratio");
	CAR_PERFORMANCE(gearbox->fGear2Ratio, "Gearbox", "Gear2Ratio");
	CAR_PERFORMANCE(gearbox->fGear3Ratio, "Gearbox", "Gear3Ratio");
	CAR_PERFORMANCE(gearbox->fGear4Ratio, "Gearbox", "Gear4Ratio");
	CAR_PERFORMANCE(gearbox->fGear5Ratio, "Gearbox", "Gear5Ratio");
	CAR_PERFORMANCE(gearbox->fGear6Ratio, "Gearbox", "Gear6Ratio");
	CAR_PERFORMANCE(gearbox->fGearRInertia, "Gearbox", "GearRInertia");
	CAR_PERFORMANCE(gearbox->fGearNInertia, "Gearbox", "Gear1Inertia");
	CAR_PERFORMANCE(gearbox->fGear1Inertia, "Gearbox", "Gear1Inertia");
	CAR_PERFORMANCE(gearbox->fGear2Inertia, "Gearbox", "Gear2Inertia");
	CAR_PERFORMANCE(gearbox->fGear3Inertia, "Gearbox", "Gear3Inertia");
	CAR_PERFORMANCE(gearbox->fGear4Inertia, "Gearbox", "Gear4Inertia");
	CAR_PERFORMANCE(gearbox->fGear5Inertia, "Gearbox", "Gear5Inertia");
	CAR_PERFORMANCE(gearbox->fGear6Inertia, "Gearbox", "Gear6Inertia");
	CAR_PERFORMANCE_TUNE(gearbox->fClutchEngageTime, "Gearbox", "Gearbox_Max", "ClutchEngageTime", tuning.fClutchEngageTime);
	CAR_PERFORMANCE_TUNE(gearbox->fClutchReleaseTime, "Gearbox", "Gearbox_Max", "ClutchReleaseTime", tuning.fClutchReleaseTime);
}

void __stdcall LoadCarDifferential(Differential* diff) {
	WriteLogDebug("HANDLING", std::format("Initing differential for car{} for {}", diff->pPerformance->pCar->pPlayer->nCarId+1,
							  GetStringNarrow(diff->pPerformance->pCar->pPlayer->sPlayerName.Get())));

	auto config = GetCarPerformanceTable(diff->pPerformance->pCar->pPlayer->nCarId+1);

	auto tuning = GetTuningDataForCar(diff->pPerformance->pCar);
	CAR_PERFORMANCE_TUNE(diff->fInertiaDriveShaft, "Engine", "Engine_Max", "InertiaDriveShaft", tuning.fInertiaDriveShaft);
	CAR_PERFORMANCE_TUNE(diff->fEndRatio, "Gearbox", "Gearbox_Max", "EndRatio", tuning.fEndRatio);
	CAR_PERFORMANCE_TUNE(diff->fClutchTorque, "Gearbox", "Gearbox_Max", "ClutchTorque", tuning.fClutchTorque);

	diff->fInvInertiaDriveShaft = 1.0 / diff->fInvInertiaDriveShaft;
	diff->fInvEndRatio = 1.0 / diff->fEndRatio;
	diff->fUnknown40 = 100.0;
}


// fo2 roamer driverloc:
// -0.299, 0.04, 0.09 in fo1
//float fFO1DriverLocOffset[3] = { (-0.421) - (-0.299), (-0.08546461) - (0.04), (-0.078) - (0.09) };
float fFO1DriverLocOffset[3] = { (-0.307) - (-0.299), (0.484 - 0.04) - 0.27727438, (0.025 - 0.09) - 0.09 };

void __fastcall LoadCarBody(Car* car) {
	WriteLogDebug("HANDLING", std::format("Initing body for car{} for {}", car->pPlayer->nCarId+1,
							  GetStringNarrow(car->pPlayer->sPlayerName.Get())));

	auto config = GetCarPerformanceTable(car->pPlayer->nCarId+1);
	auto data = GetCarDataTable(car->pPlayer->nCarId+1);

	auto tuning = GetTuningDataForCar(car);

	std::string str = data["Data"]["DataPath"].value_or("");
	if (str.empty()) {
		MessageBoxA(0, "Failed to read DataPath from Data", "Fatal error", MB_ICONERROR);
	}
	car->sFolderPath.Set(str.c_str(), str.length());

	str = data["Data"]["Name"].value_or("");
	if (str.empty()) {
		MessageBoxA(0, "Failed to read Name from Data", "Fatal error", MB_ICONERROR);
	}
	car->sName.Set(str.c_str(), str.length());

	auto body = &car->Body;

	const char* steerBalanceFactor = nHandlingMode == HANDLING_PROFESSIONAL ? "ProSteerBalanceFactor" : "SteerBalanceFactor";
	const char* steerBalanceRate = nHandlingMode == HANDLING_PROFESSIONAL ? "ProSteerBalanceRate" : "SteerBalanceRate";
	CAR_PERFORMANCE_ARRAY(body->fArcadeSteerBalanceFactor, "Body", steerBalanceFactor, 3);
	CAR_PERFORMANCE_ARRAY(body->fArcadeSteerBalanceRate, "Body", steerBalanceRate, 3);
	CAR_PERFORMANCE_TUNE(body->fArcadeBrakePower, "Body", "Body_Max", "BrakePower", tuning.fBrakePower);
	CAR_PERFORMANCE(body->nDriverType, "Body", "DriverType");
	CAR_DATA_ARRAY(body->fDriverLoc, "Body", "DriverLoc", 3);
	CAR_PERFORMANCE(body->fWheelDisplacement, "Body", "WheelDisplacement");
	CAR_PERFORMANCE_ARRAY(body->fWheelAligningTorqueLimits, "Body", "WheelAligningTorqueLimits", 2);
	CAR_PERFORMANCE(body->fFFFrictionNominalLoad, "Body", "FFFrictionNominalLoad");
	CAR_PERFORMANCE(body->fFFFrictionOffset, "Body", "FFFrictionOffset");
	CAR_PERFORMANCE(body->fFFCenteringNominalLoad, "Body", "FFCenteringNominalLoad");
	CAR_PERFORMANCE(body->fFFCenteringOffset, "Body", "FFCenteringOffset");
	CAR_PERFORMANCE(body->fMass, "Body", "Mass");
	CAR_PERFORMANCE_ARRAY(body->fMomentOfInertia, "Body", "MomentOfInertia", 3);
	CAR_PERFORMANCE_ARRAY(body->fCenterOfMass, "Body", "CenterOfMass", 3);
	CAR_PERFORMANCE_ARRAY(body->fAeroDrag, "Body", "AeroDrag", 3);
	CAR_PERFORMANCE_ARRAY(body->fAeroDragLoc, "Body", "AeroDragLoc", 3);
	CAR_PERFORMANCE_ARRAY(body->fDamping, "Body", "Damping", 2);
	CAR_PERFORMANCE_ARRAY(body->fCenterOfDownForce, "Body", "CenterOfDownForce", 3);
	CAR_PERFORMANCE(body->fDownForceConst, "Body", "DownForceConst");
	CAR_PERFORMANCE_ARRAY(body->fSteeringSpeeds, "Body", "SteeringSpeeds", 2);
	CAR_PERFORMANCE(body->fSteeringLimit, "Body", "SteeringLimit");
	CAR_PERFORMANCE(body->fSteeringReduction, "Body", "SteeringReduction");
	CAR_PERFORMANCE_ARRAY(body->fBrakeBalance, "Body", "BrakeBalance", 3);
	CAR_PERFORMANCE(body->fBrakeTorque, "Body", "BrakeTorque");
	CAR_PERFORMANCE(body->fHandBrakeTorque, "Body", "HandBrakeTorque");
	CAR_PERFORMANCE(body->fTireTurnAngleIn, "Body", "TireTurnAngleIn");
	CAR_PERFORMANCE(body->fTireTurnAngleOut, "Body", "TireTurnAngleOut");
	CAR_PERFORMANCE(body->fTireCenteringSpeed, "Body", "TireCenteringSpeed");
	CAR_PERFORMANCE(body->nFrontTraction, "Body", "FrontTraction");
	CAR_PERFORMANCE(body->nRearTraction, "Body", "RearTraction");
	CAR_PERFORMANCE_TUNE(fCarDurability[car->pPlayer->nPlayerId-1], "Body", "Body_Max", "Durability", tuning.fDurability);

	if (CarnageRace::bIsCarnageRace && car->pPlayer->nPlayerType != PLAYERTYPE_LOCAL) {
		body->fMass *= fCarnageModeMassFudge;
	}

	body->fTireTurnAngleIn *= 0.017453292;
	body->fTireTurnAngleOut *= 0.017453292;
	body->fMass2 = body->fMass;
	body->fCenterOfMass2[0] = body->fCenterOfMass[0];
	body->fCenterOfMass2[1] = body->fCenterOfMass[1];
	body->fCenterOfMass2[2] = body->fCenterOfMass[2];
	car->fBrakeBalance = body->fBrakeBalance[1];
	car->fTireTurnAngleIn = body->fTireTurnAngleIn;
	car->fTireTurnAngleOut = body->fTireTurnAngleOut;

	// fo2 cars driverloc offset hack
	if (car->pPlayer->nCarId >= 200 && car->pPlayer->nCarId <= 250) {
		for (int i = 0; i < 3; i++) {
			body->fDriverLoc[i] -= fFO1DriverLocOffset[i];
		}
	}
}

int __attribute__((naked)) LoadCarBodyASM() {
	__asm__ (
		"mov ecx, ebx\n\t"
		"jmp %0\n\t"
			:
			:  "i" (LoadCarBody)
	);
}

void __stdcall LoadCarTires(Car* car) {
	WriteLogDebug("HANDLING", std::format("Initing tires for car{} for {}", car->pPlayer->nCarId+1,
							  GetStringNarrow(car->pPlayer->sPlayerName.Get())));

	auto config = GetCarTireTable(car->pPlayer->nCarId+1);
	float fRollingResistance;
	float fInducedDragCoeff;
	float fPneumaticTrail;
	float fPneumaticOffset;
	float fOptimalSlipRatio;
	float fOptimalSlipAngle;
	float fOptimalSlipLoad;
	float fOptimalLoadFactor;
	float fXFriction[2];
	float fZFriction[2];
	float fSlideControl;
	float fUnderSteer;
	float fSlowDown;
	CAR_PERFORMANCE(fRollingResistance, "TireDynamics", "RollingResistance");
	CAR_PERFORMANCE(fInducedDragCoeff, "TireDynamics", "InducedDragCoeff");
	CAR_PERFORMANCE(fPneumaticTrail, "TireDynamics", "PneumaticTrail");
	CAR_PERFORMANCE(fPneumaticOffset, "TireDynamics", "PneumaticOffset");
	CAR_PERFORMANCE(fOptimalSlipRatio, "TireDynamics", "OptimalSlipRatio");
	CAR_PERFORMANCE(fOptimalSlipAngle, "TireDynamics", "OptimalSlipAngle");
	CAR_PERFORMANCE(fOptimalSlipLoad, "TireDynamics", "OptimalSlipLoad");
	CAR_PERFORMANCE(fOptimalLoadFactor, "TireDynamics", "OptimalLoadFactor");
	CAR_PERFORMANCE_ARRAY(fXFriction, "TireDynamics", "XFriction", 2);
	CAR_PERFORMANCE_ARRAY(fZFriction, "TireDynamics", "ZFriction", 2);
	CAR_PERFORMANCE(fSlideControl, "TireDynamics", "SlideControl");
	CAR_PERFORMANCE(fUnderSteer, "TireDynamics", "UnderSteer");
	CAR_PERFORMANCE(fSlowDown, "TireDynamics", "SlowDown");

	auto v22 = car->Body.fMass * 2.4525001;
	auto v105 = fOptimalSlipLoad * v22 * 2.6800001 / std::tan(fOptimalSlipAngle * 0.017453292);
	auto v106 = fOptimalLoadFactor * v22 * 0.001;
	auto v107 = fOptimalSlipLoad / fOptimalSlipRatio * v22 * 2.6800001 / (v22 * 0.001);

	auto global = GetTireDynamicsTable();
	for (int i = 0; i < 7; i++) {
		auto category = std::format("GlobalDynamics_{}", i+1);
		TIRE_PERFORMANCE(car->TireDynamics[i].fRollingResistance, category, "RollingResistance");
		TIRE_PERFORMANCE(car->TireDynamics[i].fInducedDragCoeff, category, "InducedDragCoeff");
		TIRE_PERFORMANCE(car->TireDynamics[i].fPneumaticTrail, category, "PneumaticTrail");
		TIRE_PERFORMANCE(car->TireDynamics[i].fPneumaticOffset, category, "PneumaticOffset");
		TIRE_PERFORMANCE_ARRAY(car->TireDynamics[i].fZStiffness, category, "ZStiffness", 3);
		TIRE_PERFORMANCE_ARRAY(car->TireDynamics[i].fXStiffness, category, "XStiffness", 3);
		TIRE_PERFORMANCE_ARRAY(car->TireDynamics[i].fCStiffness, category, "CStiffness", 2);
		TIRE_PERFORMANCE_ARRAY(car->TireDynamics[i].fZFriction, category, "ZFriction", 2);
		TIRE_PERFORMANCE_ARRAY(car->TireDynamics[i].fXFriction, category, "XFriction", 2);
		TIRE_PERFORMANCE(car->TireDynamics[i].fSlideControl, category, "SlideControl");
		TIRE_PERFORMANCE(car->TireDynamics[i].fUnderSteer, category, "UnderSteer");
		TIRE_PERFORMANCE(car->TireDynamics[i].fSlowDown, category, "SlowDown");

		// why are some of these multiplied by 0.0? wtf
		car->TireDynamics[i].fRollingResistance *= fRollingResistance;
		car->TireDynamics[i].fInducedDragCoeff *= fInducedDragCoeff;
		car->TireDynamics[i].fPneumaticTrail *= fPneumaticTrail;
		car->TireDynamics[i].fPneumaticOffset *= fPneumaticOffset;
		car->TireDynamics[i].fZStiffness[0] *= 0.0;
		car->TireDynamics[i].fZStiffness[1] *= v107;
		car->TireDynamics[i].fZStiffness[2] *= 0.0;
		car->TireDynamics[i].fXStiffness[0] *= v105;
		car->TireDynamics[i].fXStiffness[1] *= v106;
		car->TireDynamics[i].fXStiffness[2] *= 0.0;
		car->TireDynamics[i].fCStiffness[0] *= 0.0;
		car->TireDynamics[i].fCStiffness[1] *= 0.0;
		car->TireDynamics[i].fZFriction[0] *= fZFriction[0];
		car->TireDynamics[i].fZFriction[1] *= fZFriction[1];
		car->TireDynamics[i].fXFriction[0] *= fXFriction[0];
		car->TireDynamics[i].fXFriction[1] *= fXFriction[1];
		car->TireDynamics[i].fSlideControl *= fSlideControl;
		car->TireDynamics[i].fUnderSteer *= fUnderSteer;
		car->TireDynamics[i].fSlowDown *= fSlowDown;
	}

	CAR_PERFORMANCE(car->Body.fFrontTireMass, "Front", "Mass");
	CAR_PERFORMANCE(car->Body.fFrontTireMomentOfInertia, "Front", "MomentOfInertia");
	CAR_PERFORMANCE(car->Body.fFrontTireRadius, "Front", "Radius");
	CAR_PERFORMANCE(car->Body.fFrontTireWidth, "Front", "Width");
	CAR_PERFORMANCE(car->Body.fFrontSuspensionLift, "Front", "SuspensionLift");
	CAR_PERFORMANCE(car->Body.fRearTireMass, "Rear", "Mass");
	CAR_PERFORMANCE(car->Body.fRearTireMomentOfInertia, "Rear", "MomentOfInertia");
	CAR_PERFORMANCE(car->Body.fRearTireRadius, "Rear", "Radius");
	CAR_PERFORMANCE(car->Body.fRearTireWidth, "Rear", "Width");
	CAR_PERFORMANCE(car->Body.fRearSuspensionLift, "Rear", "SuspensionLift");
}

void LoadCarSuspensionPart(Car* car, bool front) {
	auto config = GetCarPerformanceTable(car->pPlayer->nCarId+1);

	auto tuning = GetTuningDataForCar(car);

	auto susp = front ? &car->Body.SuspensionFront : &car->Body.SuspensionRear;
	auto category = front ? "SuspensionFront" : "SuspensionRear";
	auto categoryMax = front ? "SuspensionFront_Max" : "SuspensionRear_Max";

	CAR_PERFORMANCE(susp->fMinLength, category, "MinLength");
	CAR_PERFORMANCE(susp->fMaxLength, category, "MaxLength");
	CAR_PERFORMANCE_TUNE(susp->fRestLength, category, categoryMax, "RestLength", tuning.fRestLength);
	CAR_PERFORMANCE_TUNE(susp->fDefaultCompression, category, categoryMax, "DefaultCompression", tuning.fDefaultCompression);
	CAR_PERFORMANCE_TUNE(susp->fBumpDamp, category, categoryMax, "BumpDamp", tuning.fBumpDamp);
	CAR_PERFORMANCE_TUNE(susp->fReboundDamp, category, categoryMax, "ReboundDamp", tuning.fReboundDamp);
	CAR_PERFORMANCE(susp->fBumperLength, category, "BumperLength");
	CAR_PERFORMANCE(susp->fBumperConst, category, "BumperConst");
	CAR_PERFORMANCE_TUNE(susp->fBumperRestitution, category, categoryMax, "BumperRestitution", tuning.fBumperRestitution);
	CAR_PERFORMANCE_TUNE(susp->fRollBarStiffness, category, categoryMax, "RollBarStiffness", tuning.fRollBarStiffness);
	CAR_PERFORMANCE(susp->fCamberAngle, category, "CamberAngle");
	CAR_PERFORMANCE(susp->fCamberChangeUp, category, "CamberChangeUp");
	CAR_PERFORMANCE(susp->fCamberChangeDown, category, "CamberChangeDown");
	CAR_PERFORMANCE(susp->fCamberChangeIn, category, "CamberChangeIn");
	CAR_PERFORMANCE(susp->fCamberChangeOut, category, "CamberChangeOut");

	auto v62 = car->Body.fMass * 0.25;
	susp->fDefaultCompression = (v62 * 9.81) / susp->fDefaultCompression;
	auto v77 = 1.0 / v62;
	auto v37 = sqrt(susp->fDefaultCompression * v77);
	auto v59 = v37 * v62 + v37 * v62;
	susp->fBumpDamp *= v59;
	susp->fReboundDamp *= v59;
	susp->fCamberAngle *= 0.017453292;
	susp->fCamberChangeUp *= 0.017453292;
	susp->fCamberChangeDown *= 0.017453292;
}

void __stdcall LoadCarSuspension(Car* car) {
	WriteLogDebug("HANDLING", std::format("Initing suspension for car{} for {}", car->pPlayer->nCarId+1,
							  GetStringNarrow(car->pPlayer->sPlayerName.Get())));

	LoadCarSuspensionPart(car, true);
	LoadCarSuspensionPart(car, false);
}

void __stdcall LoadCarEngineMesh(Car* car) {
	car->SelectEngine(car, 0);
	car->InitEngine(0);
	car->FinishInitEngine(0);
}

void __stdcall LoadCarSounds(Car* car) {
	WriteLogDebug("HANDLING", std::format("Initing sounds for car{} for {}", car->pPlayer->nCarId+1,
							  GetStringNarrow(car->pPlayer->sPlayerName.Get())));

	auto config = GetCarDataTable(car->pPlayer->nCarId+1);
	auto config2 = GetCarPerformanceTable(car->pPlayer->nCarId+1);

	auto engineSoundFile = (std::string)config["Data"]["EngineSound"].value_or("");
	if (engineSoundFile.empty()) {
		engineSoundFile = config2["Data"]["EngineSound"].value_or("");
	}

	Car::LoadEngineSounds(car->pPlayer->nPlayerType == PLAYERTYPE_AI, 0, &car->pEngineSound,  std::format("data/sound/{}", engineSoundFile).c_str(), 0);
	Car::LoadSurfaceSounds(4, &car->pSurfaceSounds, "data/sound/surface_sounds.bed");
}

void ApplyCarDatabasePatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CBA2, &LoadCarEngine);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CBAE, &LoadCarGearbox);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CBBA, &LoadCarDifferential);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CBC1, &LoadCarBodyASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CC0C, &LoadCarTires);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CC27, &LoadCarSuspension);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41DA38, &LoadCarEngineMesh);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x447A3D, &LoadCarSounds);

	// remove equipment code
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43EC90, 0x4D6B74);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43ECA0, 0x4D6B74);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A72D2, 0x4D6B74);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A72E2, 0x4D6B74);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A72F2, 0x4D6B74);
	NyaHookLib::Fill(0x43BB7E, 0x90, 5); // remove Equipment lua functions
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43BD20, 0x43C24A); // remove player tuning init
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43C250, 0x43C4FE); // remove ai tuning init
	NyaHookLib::Patch<uint8_t>(0x43D70C, 0xEB); // don't read cars.cfg
	NyaHookLib::Patch<uint8_t>(0x43F36A, 0xEB); // don't use numcars
	NyaHookLib::Patch<uint8_t>(0x43F380, 0xEB); // don't use numcars

	// remove equipment class
	NyaHookLib::Patch<uint8_t>(0x4A7790, 0xC3); // don't allocate equipment data
	NyaHookLib::Patch<uint8_t>(0x43EE6A, 0xEB); // don't init player tuning
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43EE6A, 0x43EE88); // don't init splitscreen tuning
	NyaHookLib::Fill(0x43EFF0, 0x90, 7); // don't reset car tuning class when initing players
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43FCA1, 0x43FCD8); // don't init car tuning class
	// remove all car tuning inits
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43F7C1, 0x43C4FE);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x440066, 0x43C24A);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4401FA, 0x43C24A);
}