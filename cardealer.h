struct tDealerCar {
	int carId;
	std::string name;
	int classId;
	int cameraId;
	int performanceId;
};
std::vector<tDealerCar> aDealerCars;

tDealerCar* GetDealerCar(int carId) {
	for (auto& car : aDealerCars) {
		if (car.carId == carId) return &car;
	}
	return nullptr;
}

std::string GetCarName(int carId) {
	if (auto car = GetDealerCar(carId)) {
		return car->name;
	}
	return "NULL";
}

std::string GetSkinName(int carId, int skinId, bool wrapAround) {
	static auto config = toml::parse_file("Config/CarSkins.toml");
	int numSkins = GetNumSkinsForCar(carId);
	if (!wrapAround && (skinId < 1 || skinId > numSkins)) {
		return "---";
	}
	// wrap around
	while (skinId < 1) {
		skinId += numSkins;
	}
	while (skinId > numSkins) {
		skinId -= numSkins;
	}
	std::string string = config["car_" + std::to_string(carId)]["skin" + std::to_string(skinId) + "name"].value_or("");
	if (string.empty()) string = "Skin " + std::to_string(skinId);
	return string;
}

std::string GetSkinAuthor(int carId, int skinId, bool wrapAround) {
	static auto config = toml::parse_file("Config/CarSkins.toml");
	int numSkins = GetNumSkinsForCar(carId);
	if (!wrapAround && (skinId < 1 || skinId > numSkins)) {
		return "";
	}
	// wrap around
	while (skinId < 1) {
		skinId += numSkins;
	}
	while (skinId > numSkins) {
		skinId -= numSkins;
	}
	std::string string = config["car_" + std::to_string(carId)]["skin" + std::to_string(skinId)].value_or("");
	if (!string.empty()) string = "Author: " + string;
	return string;
}

void ApplyCarDealerPatches() {
	WriteLogDebug("CARDEALER", "--- Loading car data ---");

	for (int i = 0; i < 1024; i++) {
		auto path = std::format("data/database/cars/car{}.toml", i+1);
		if (!DoesFileExist(path.c_str())) continue;

		auto config = ReadTOMLFromBfs(path);
		if (!DoesFileExist(std::format("{}body.bgm", config["Data"]["DataPath"].value_or("")).c_str())) continue;

		tDealerCar car;
		car.carId = i+1;
		car.name = config["Data"]["Name"].value_or("");
		car.classId = config["Data"]["Class"].value_or(0);
		car.cameraId = config["Data"]["Camera"].value_or(0);
		car.performanceId = config["Data"]["PerformanceOverride"].value_or(car.carId);
		if (car.classId <= 0) continue;
		if (car.cameraId <= 0) {
			car.cameraId = i+1;
		}
		if (car.name.empty()) continue;
		WriteLogDebug("CARDEALER", std::format("Registered dealer car {} ({}) to data ID {} with performance from car{}", i+1, car.name, car.carId, car.performanceId));
		aDealerCars.push_back(car);
	}
	std::sort(aDealerCars.begin(), aDealerCars.end(), [](const tDealerCar& a, const tDealerCar& b) {
		if (a.classId != b.classId) {
			return a.classId < b.classId;
		}
		if (a.cameraId != b.cameraId) {
			return a.cameraId < b.cameraId;
		}
		return a.carId < b.carId;
	});
	WriteLogDebug("CARDEALER", "--- Finished loading car data ---");
}

ChloeHook Hook_DynamicCarDealer([]() {
	ChloeEvents::FilesystemInitEvent.AddHandler(ApplyCarDealerPatches);
});