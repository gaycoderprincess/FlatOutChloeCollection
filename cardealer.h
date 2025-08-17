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
	auto config = ReadTOMLFromBfs("data/database/cardata.toml");
	int count = config["CarDealer"]["NumCars"].value_or(0);
	aDealerCars.reserve(count);
	for (int i = 0; i < count; i++) {
		auto category = std::format("Car{}", i+1);
		tDealerCar car;
		car.carId = config["CarDealer"][category]["CarId"].value_or(0);
		car.name = config["CarDealer"][category]["Name"].value_or("");
		car.classId = config["CarDealer"][category]["Class"].value_or(0);
		car.cameraId = config["CarDealer"][category]["Camera"].value_or(0);
		car.performanceId = config["CarDealer"][category]["PerformanceOverride"].value_or(car.carId);
		if (car.carId <= 0) continue;
		if (!nFO2CarsEnabled && car.carId >= 200 && car.carId <= 250) continue;
		if (car.classId <= 0) continue;
		if (car.cameraId <= 0) {
			car.cameraId = i+1;
		}
		if (car.name.empty()) continue;
		aDealerCars.push_back(car);
	}
}