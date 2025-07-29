namespace MenuHudState {
	bool bInCarDealer = false;
}

namespace SkinSelector {
	float fPosX = 0.1;
	float fPosY = 0.42;
	float fSize = 0.02;
	float fSpacing = 0.04;

	bool bMenuUp = false;

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
			return "---";
		}
		// wrap around
		while (skinId < 1) {
			skinId += numSkins;
		}
		while (skinId > numSkins) {
			skinId -= numSkins;
		}
		std::string string = config["car_" + std::to_string(carId)]["skin" + std::to_string(skinId)].value_or("");
		if (!string.empty()) string = "Skin Author: " + std::to_string(skinId);
		return string;
	}

	void OnTick() {
		if (!bMenuUp) return;

		auto menu = pGameFlow->pMenuInterface;
		if (!menu) return;
		if (!menu->pMenuScene) return;

		int car = menu->pMenuScene->nCar;
		int skin = menu->pMenuScene->nCarSkin;
		int numSkins = GetNumSkinsForCar(car);

		bool skinLoop = false;
		if (numSkins > 6) {
			skinLoop = true;
		}
		else {
			skinLoop = false;
		}

		auto func = &DrawStringFO2_Small;

		tNyaStringData data;
		data.x = 1.0 - (fPosX * GetAspectRatio());
		data.y = fPosY;
		data.size = fSize;
		for (int i = 1; i <= 6; i++) {
			int skinId = 0;
			if (numSkins > 6) {
				skinId = skin - 3 + i;
			}
			else {
				skinId = i;
			}

			if (skinId == skin) {
				data.SetColor(255, 255, 255, 255);
			}
			else {
				data.SetColor(127, 127, 127, 255);
			}

			DrawString(data, GetSkinName(car, skinId, skinLoop), func);
			data.y += fSpacing;
		}
	}
}