struct tDealerCar {
	int carId;
	std::string name;
	int classId;
	int cameraId;
};
std::vector<tDealerCar> aDealerCars;

std::string GetCarName(int carId) {
	for (auto& car : aDealerCars) {
		if (car.carId == carId) return car.name;
	}
	return "NULL";
}

void ApplyCarDealerPatches() {
	static auto config = toml::parse_file("Config/CarData.toml");
	int count = config["CarDealer"]["NumCars"].value_or(0);
	aDealerCars.reserve(count);
	for (int i = 0; i < count; i++) {
		auto category = std::format("Car{}", i+1);
		tDealerCar car;
		car.carId = config["CarDealer"][category]["CarId"].value_or(0);
		car.name = config["CarDealer"][category]["Name"].value_or("");
		car.classId = config["CarDealer"][category]["Class"].value_or(0);
		car.cameraId = config["CarDealer"][category]["Camera"].value_or(0);
		if (car.carId <= 0) continue;
		if (car.classId <= 0) continue;
		if (car.cameraId <= 0) {
			car.cameraId = i+1;
		}
		if (car.name.empty()) continue;
		aDealerCars.push_back(car);
	}
}