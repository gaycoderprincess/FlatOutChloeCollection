toml::table GetCarPerformanceTable(int id) {
	return ReadTOMLFromBfs(std::format("data/database/cars/car{}.toml", GetDealerCar(id)->performanceId));
}

// tuning lerp values - 0 to 1
struct tCarTuningData {
	// body
	float fBrakePower;
	float fDurability; // todo implement with the new durability hack

	// engine
	float fTurboAcceleration;
	float fInertiaEngine;
	float fInertiaDriveShaft;
	float fEngineFriction;
	float fTorqueMax;
	float fHorsepower;

	// gearbox
	float fClutchReleaseTime;
	float fGearShiftTime;
	float fClutchEngageTime;
	float fClutchTorque;
	float fEndRatio;

	// suspension
	struct tSuspensionTuning {
		float fRestLength;
		float fDefaultCompression;
		float fBumpDamp;
		float fReboundDamp;
		float fBumperRestitution;
		float fRollBarStiffness;
	};
	tSuspensionTuning SuspFront;
	tSuspensionTuning SuspRear;

	tCarTuningData() {
		memset(this,0,sizeof(*this));
	}
};

#define CAR_PERFORMANCE(value, category, name) value = config[category][name].value_or(-99999.0f); if (value == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); }
#define CAR_PERFORMANCE_FALLBACK(value, category, name, default) value = config[category][name].value_or(default)
#define CAR_PERFORMANCE_TUNE(value, category, category_max, name, tuningValue) value = std::lerp(config[category][name].value_or(-99999.0f), config[category_max][name].value_or(config[category][name].value_or(0.0)), tuningValue); if (value == -99999.0f) { MessageBoxA(0, std::format("Failed to read {} from {}", name, category).c_str(), "Fatal error", MB_ICONERROR); }

void __stdcall LoadCarEngine(Engine* engine) {
	auto config = GetCarPerformanceTable(engine->pPerformance->pCar->pPlayer->nCarId+1);

	tCarTuningData tuning; // todo
	CAR_PERFORMANCE_TUNE(engine->fInertiaEngine, "Engine", "Engine_Max", "InertiaEngine", tuning.fInertiaEngine);
	CAR_PERFORMANCE_TUNE(engine->fEngineFriction, "Engine", "Engine_Max", "EngineFriction", tuning.fEngineFriction);
	CAR_PERFORMANCE(engine->fIdleRpm, "Engine", "IdleRpm");
	CAR_PERFORMANCE_FALLBACK(engine->fZeroPowerRpm, "Engine", "ZeroPowerRpm", engine->fIdleRpm * 0.5);
	CAR_PERFORMANCE_TUNE(engine->fTorqueMax, "Engine", "Engine_Max", "TorqueMax", tuning.fTorqueMax);
	CAR_PERFORMANCE(engine->nTorqueCurveParams, "Engine", "TorqueCurveParams");
	for (int i = 0; i < 20; i++) {
		engine->aTorque[i].fRpm = config["Engine"][std::format("Torque{:02}", i)]["RPM"].value_or(0.0);
		engine->aTorque[i].fCurve = config["Engine"][std::format("Torque{:02}", i)]["Curve"].value_or(0.0);
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
	auto config = GetCarPerformanceTable(gearbox->pPerformance->pCar->pPlayer->nCarId+1);

	tCarTuningData tuning; // todo
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
	auto config = GetCarPerformanceTable(diff->pPerformance->pCar->pPlayer->nCarId+1);

	tCarTuningData tuning; // todo
	CAR_PERFORMANCE_TUNE(diff->fInertiaDriveShaft, "Engine", "Engine_Max", "InertiaDriveShaft", tuning.fInertiaDriveShaft);
	CAR_PERFORMANCE_TUNE(diff->fEndRatio, "Gearbox", "Gearbox_Max", "EndRatio", tuning.fEndRatio);
	CAR_PERFORMANCE_TUNE(diff->fClutchTorque, "Gearbox", "Gearbox_Max", "ClutchTorque", tuning.fClutchTorque);

	diff->fInvInertiaDriveShaft = 1.0 / diff->fInvInertiaDriveShaft;
	diff->fInvEndRatio = 1.0 / diff->fEndRatio;
	diff->fUnknown40 = 100.0;
}

void ApplyCarDatabasePatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CBA2, &LoadCarEngine);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CBAE, &LoadCarGearbox);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41CBBA, &LoadCarDifferential);
}