void __fastcall LoadDevTexture(DevTexture* pTexture) {
	if (pTexture->pD3DTexture) return;

	pDeviceD3d->_vf_CreateTextureFromFile(pTexture, pTexture->sPath.Get(), pTexture->nFlags);
}

uintptr_t LoadMenuTexturesASM_jmp = 0x460E9E;
void __attribute__((naked)) LoadMenuTexturesASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"popad\n\t"

		"mov esi, [esp+0xC]\n\t"
		"mov edx, [esp+0xA4]\n\t"
		"jmp %0\n\t"
			:
			:  "m" (LoadMenuTexturesASM_jmp), "i" (LoadDevTexture)
	);
}

void __fastcall OnCarModelLoad(Car* pCar) {
	static std::string path, folderPath, name;
	path = std::format("{}tire.bgm", pCar->sFolderPath.Get());
	folderPath = pCar->sFolderPath.Get();
	name = "tire.bgm";

	NyaHookLib::Patch(0x426174 + 1, path.c_str());
	NyaHookLib::Patch(0x426B9B + 1, path.c_str());
	NyaHookLib::Patch(0x423D41 + 1, folderPath.c_str());
	NyaHookLib::Patch<uint8_t>(0x423D3F + 1, folderPath.length());
	NyaHookLib::Patch(0x426BAB + 1, name.c_str());
	NyaHookLib::Patch(0x426BCD + 1, name.c_str());

	// retro demo cars
	auto carId = pCar->pPlayer->nCarId;
	if (carId >= 150 && carId <= 160) {
		NyaHookLib::Patch(0x424E9C + 1, "dashboard");
		NyaHookLib::Patch(0x424F48 + 1, "engine");
		NyaHookLib::Patch(0x4259B8 + 1, "steering_wheel");
	}
	else {
		NyaHookLib::Patch(0x424E9C + 1, "dash_lo");
		NyaHookLib::Patch(0x424F48 + 1, "engine_1");
		NyaHookLib::Patch(0x4259B8 + 1, "steering_wheel_lo");
	}
}

uintptr_t LoadCarModelASM_jmp = 0x460E9E;
void __attribute__((naked)) LoadCarModelASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebp\n\t"
		"call %1\n\t"
		"popad\n\t"

		"jmp %0\n\t"
			:
			:  "m" (LoadCarModelASM_jmp), "i" (OnCarModelLoad)
	);
}

DevTexture* __stdcall LoadMenucarTextureNew(void* a1, const char* path, int a3, int a4) {
	bool bIsFO2Car = false;
	if (pGameFlow->pMenuInterface->pMenuScene->nCar >= 200 && pGameFlow->pMenuInterface->pMenuScene->nCar <= 250) bIsFO2Car = true;

	std::string str = path;
	if (str.starts_with("tire_0") && bIsFO2Car) {
		str[0] = 'f';
		str[1] = '2';
		str[2] = 't';
		str[3] = 'i';
		str[4] = 'r';
		str[5] = 'e';
	}
	return LoadTextureFromFile(a1, str.c_str(), a3, a4);
}

// fo2 roamer driverloc:
// -0.307, 0.484, 0.025 in fo2
// -0.299, 0.04, 0.09 in fo1
float fFO1DriverLocOffset[3] = { (-0.307) - (-0.299), (0.484 - 0.04) - 0.27727438, (0.025 - 0.09) - 0.09 };

void __fastcall OnCarCreated(Car* car) {
	if (car->pPlayer->nCarId >= 200 && car->pPlayer->nCarId <= 250) {
		for (int i = 0; i < 3; i++) {
			car->Body.fDriverLoc[i] -= fFO1DriverLocOffset[i];
		}
	}
}

uintptr_t OnCarCreatedASM_jmp = 0x50E080;
void __attribute__((naked)) OnCarCreatedASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebx\n\t"
		"call %1\n\t"
		"popad\n\t"

		"jmp %0\n\t"
			:
			:  "m" (OnCarCreatedASM_jmp), "i" (OnCarCreated)
	);
}

int __stdcall GetEngineModel(void* a1) {
	return 1;
}

void ApplyCarLimitAdjusterPatches() {
	// remove hardcoded shared path from lights_damaged
	static const char lightsDamagedPath[] = "lights_damaged.tga";
	NyaHookLib::Patch(0x41E483 + 1, lightsDamagedPath);

	// swap folder priorities
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41E41D, 0x4EFDD0);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41E430, 0x4EFD60);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4244B1, 0x4EFDD0);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4244B9, 0x4EFD60);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x460E93, &LoadMenuTexturesASM);

	NyaHookLib::Patch(0x4658C4 + 1, 2097152); // menucar model memory
	NyaHookLib::Patch(0x4658C9 + 3, 2097152); // menucar model memory
	NyaHookLib::Patch(0x4658D8 + 1, 16777216); // menucar skin memory
	NyaHookLib::Patch(0x4658DD + 3, 16777216); // menucar skin memory

	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x460E8E, &LoadMenucarTextureNew);
	LoadCarModelASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41D765, &LoadCarModelASM);

	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x42280A, &OnCarCreatedASM);

	// remove car id bitwise operations
	NyaHookLib::Patch<uint16_t>(0x43F50D, 0x9090);
	NyaHookLib::Patch<uint16_t>(0x43F52F, 0x9090);

	// make all cars load all engine models as engine_1
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x424FD0, 0x424F5A); // engine_2
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42503D, 0x424F5A); // engine_3
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x425117, 0x4250AA); // coolingfan_2
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x425184, 0x4250AA); // coolingfan_3
}