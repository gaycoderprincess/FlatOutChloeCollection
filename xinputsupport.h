double fTimeSincePaused = 0;

bool IsControllerSupportEnabled() {
	if (!nControllerSupport) return false;
	if (GetGameState() != GAME_STATE_MENU && IsInSplitScreen() && nSplitScreenController[0] == 0) return false; // disable controller on p1
	return true;
}

int GetControllerSplitScreenPlayer(Controller* pController) {
	if (pController == GetPlayer(0)->pController) return nSplitScreenController[0]-1;
	return nSplitScreenController[pController->_4[0x79C/4]];
}

bool __thiscall IsMenuInputJustPressedXInput(Controller* pThis, int input) {
	int padId = IsInSplitScreen() ? GetControllerSplitScreenPlayer(pThis) : -1;
	if (input == CONTROLLER_BUTTON_UP) { return IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_UP, padId); }
	if (input == CONTROLLER_BUTTON_DOWN) { return IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_DOWN, padId); }
	if (input == CONTROLLER_BUTTON_LEFT) { return IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_LEFT, padId); }
	if (input == CONTROLLER_BUTTON_RIGHT) { return IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_RIGHT, padId); }
	if (input == CONTROLLER_BUTTON_START) { return IsPadKeyJustPressed(NYA_PAD_KEY_START, padId); }
	if (input == CONTROLLER_BUTTON_A) { return IsPadKeyJustPressed(NYA_PAD_KEY_A, padId); }
	if (input == CONTROLLER_BUTTON_SELECT) { return IsPadKeyJustPressed(NYA_PAD_KEY_B, padId); }
	return false;
}

bool __thiscall IsGameInputJustPressedXInput(Controller* pThis, int input) {
	int padId = IsInSplitScreen() ? GetControllerSplitScreenPlayer(pThis) : -1;
	if (input == 9) { return IsPadKeyJustPressed(NYA_PAD_KEY_START, padId); }
	if (input == 3) { return IsPadKeyJustPressed(GetNyaControllerKeyForAction(CONFIG_CAMERA), padId); }
	if (input == 5) { return IsPadKeyJustPressed(GetNyaControllerKeyForAction(CONFIG_RESET), padId) || IsPadKeyJustPressed(GetNyaControllerKeyForAction(CONFIG_RESET_ALT), padId); }
	if (input == 18) { return IsPadKeyJustPressed(GetNyaControllerKeyForAction(CONFIG_GEARDOWN), padId); }
	if (input == 19) { return IsPadKeyJustPressed(GetNyaControllerKeyForAction(CONFIG_GEARUP), padId); }
	return false;
}

int __thiscall GetAnalogInputXInput(Controller* pThis, int input, float* out) {
	*out = 0.0;
	int padId = IsInSplitScreen() ? GetControllerSplitScreenPlayer(pThis) : -1;
	//if (aPressedAnalog[input]) { *out = 1.0; }
	if (input == 0) { *out = GetPadKeyState(NYA_PAD_KEY_LSTICK_X, padId) / 32767.0; } // steer
	if (input == 1) { *out = GetPadKeyState(GetNyaControllerKeyForAction(CONFIG_THROTTLE), padId) / 255.0; }
	if (input == 2) { *out = GetPadKeyState(GetNyaControllerKeyForAction(CONFIG_BRAKE), padId) / 255.0; }
	return *out != 0.0;
}

int __thiscall GetInputValueXInput(Controller* pThis, int input) {
	int padId = IsInSplitScreen() ? GetControllerSplitScreenPlayer(pThis) : -1;
	//if (aPressedDigital[input]) return 255;
	if (input == 0) {
		auto state1 = GetPadKeyState(GetNyaControllerKeyForAction(CONFIG_HANDBRAKE), padId);
		auto state2 = GetPadKeyState(GetNyaControllerKeyForAction(CONFIG_HANDBRAKE_ALT), padId);
		return std::max(state1, state2);
	}
	if (input == 1 && fTimeSincePaused > 0.75) return GetPadKeyState(GetNyaControllerKeyForAction(CONFIG_RAGDOLL), padId);
	if (input == 4) return GetPadKeyState(GetNyaControllerKeyForAction(CONFIG_LOOK), padId);
	return 0;
}

bool __thiscall IsMenuInputJustPressedNew(Controller* pThis, int input) {
	auto orig = Controller::IsMenuInputJustPressed(pThis, input);
	if (IsInSplitScreen() && nSplitScreenController[0] != 0) orig = 0;
	if (orig || !IsControllerSupportEnabled()) return orig;
	return IsMenuInputJustPressedXInput(pThis, input);
}

bool __thiscall IsGameInputJustPressedNew(Controller* pThis, int input) {
	auto orig = Controller::IsGameInputJustPressed(pThis, input);
	if (IsInSplitScreen() && nSplitScreenController[0] != 0) orig = 0;
	if (orig || !IsControllerSupportEnabled()) return orig;
	return IsGameInputJustPressedXInput(pThis, input);
}

int __thiscall GetAnalogInputNew(Controller* pThis, int input, float* out) {
	*out = 0.0;
	Controller::GetAnalogInput(pThis, input, out);
	if (IsInSplitScreen() && nSplitScreenController[0] != 0) *out = 0;
	if (*out != 0.0 || !IsControllerSupportEnabled()) return *out != 0.0;
	return GetAnalogInputXInput(pThis, input, out);
}

int __thiscall GetInputValueNew(Controller* pThis, int input) {
	auto orig = Controller::GetInputValue(pThis, input);
	if (IsInSplitScreen() && nSplitScreenController[0] != 0) orig = 0;
	if (orig || !IsControllerSupportEnabled()) return orig;
	return GetInputValueXInput(pThis, input);
}

void ApplyUltrawidePatches();
namespace SplitscreenController {
	void __thiscall dummy_sub_4FADB0(Controller* pThis, char a2) {}
	void __thiscall dummy_Reset(Controller* pThis) {}
	void __thiscall dummy_nullsub(Controller* pThis) {}
	int __thiscall dummy_sub_4FB4A0(Controller* pThis, int a2, int a3) { return 0; }
	int __thiscall dummy_sub_484CB0(Controller* pThis) { return 1; }
	int __thiscall dummy_sub_4FB490(Controller* pThis) { MessageBoxA(0, "dummy_sub_4FB490", "", MB_ICONERROR); __debugbreak(); return 0; } // returns controller+0x130
	int __thiscall dummy_sub_4FB480(Controller* pThis) { MessageBoxA(0, "dummy_sub_4FB480", "", MB_ICONERROR); __debugbreak(); return 0; } // returns controller+0x798
	void __thiscall dummy_sub_4F2EB0(Controller* pThis, int a2) {} // does something with the vtable?? idk
	float __thiscall dummy_sub_4FB5C0(Controller* pThis) { return 0.0; }

	void* pVTable[] = {
			(void*)&dummy_sub_4FADB0, // +0
			(void*)&dummy_Reset, // +4
			(void*)&dummy_nullsub, // +8
			(void*)&dummy_Reset, // +C
			(void*)&dummy_sub_4FB4A0, // +10
			(void*)&dummy_sub_484CB0, // +14
			(void*)&dummy_sub_4FB490, // +18
			(void*)&dummy_sub_484CB0, // +1C
			(void*)&dummy_sub_4FB480, // +20
			(void*)&IsMenuInputJustPressedXInput, // +24
			(void*)&IsGameInputJustPressedXInput, // +28
			(void*)&GetInputValueXInput, // +2C
			(void*)&dummy_sub_484CB0, // +30
			(void*)&dummy_sub_4F2EB0, // +34
			(void*)0x4FB0E0, // +38
			(void*)0x4FB110, // +3C
			(void*)&GetAnalogInputXInput, // +40
			(void*)&dummy_sub_4FB5C0, // +44
			(void*)&dummy_sub_4FB5C0, // +48
			(void*)&dummy_sub_4FB5C0, // +4C
			(void*)&dummy_sub_4FB5C0, // +50
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
	};

	Controller* pControllers[nMaxSplitscreenPlayers] = {};
	void CopyController(uint8_t* data) {
		for (int i = 0; i < nMaxSplitscreenPlayers; i++) {
			if (pControllers[i]) return;
			pControllers[i] = (Controller*)malloc(0x79C+16);
			memcpy(pControllers[i], data, 0x79C);
			pControllers[i]->_4[-1] = (uint32_t)&pVTable[0];
			pControllers[i]->_4[0x79C/4] = i; // player id
		}

		ApplyUltrawidePatches(); // hacky fix here to override the fov from widescreen fix
	}
}

void ProcessXInputSupport() {
	static CNyaTimer gTimer;
	gTimer.Process();

	if (GetGameState() == GAME_STATE_RACE) {
		if (pGameFlow->nIsPaused) fTimeSincePaused = 0;
		fTimeSincePaused += gTimer.fDeltaTime;

		if (!pLoadingScreen) {
			if (IsInSplitScreen()) {
				SplitscreenController::CopyController((uint8_t*)GetPlayer(0)->pController);
				for (int i = 1; i < pPlayerHost->GetNumPlayers(); i++) {
					auto ply = GetPlayer(i);
					if (ply->nPlayerType != PLAYERTYPE_LOCAL) continue;
					ply->pController = SplitscreenController::pControllers[i];
				}
			}
		}
	}
	else {
		fTimeSincePaused = 999;
	}

	SetPadDeadzone(((nControllerDeadzone / 100.0) * 8192) + 2048);
}

void ApplyXInputPatches() {
	NyaHookLib::Patch(0x667494, &IsMenuInputJustPressedNew);
	NyaHookLib::Patch(0x667498, &IsGameInputJustPressedNew);
	NyaHookLib::Patch(0x66749C, &GetInputValueNew);
	NyaHookLib::Patch(0x6674B0, &GetAnalogInputNew);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4F13D3, 0x4F147D); // never allocate more controllers

	ChloeEvents::FinishFrameEvent.AddHandler(ProcessXInputSupport);
}