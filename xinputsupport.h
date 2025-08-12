// FOUC layout:
// LT - Brake/Reverse
// RT - Accelerate
// LB - Gear Down
// RB - Gear Up
// Select - Skip Music Track
// Start - Pause
// LStick - Steering
// A - Nitro
// B - Handbrake
// X - Camera
// Y - Reset
// DPad Left - Player List
// R3 - Look Back

double fTimeSincePaused = 0;

bool __thiscall IsMenuInputJustPressedNew(Controller* pThis, int input) {
	auto orig = Controller::IsMenuInputJustPressed(pThis, input);
	if (orig) return orig;

	if (input == 10) { return IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_UP); }
	if (input == 11) { return IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_DOWN); }
	if (input == 12) { return IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_LEFT); }
	if (input == 13) { return IsPadKeyJustPressed(NYA_PAD_KEY_DPAD_RIGHT); }
	if (input == 8) { return IsPadKeyJustPressed(NYA_PAD_KEY_START); }
	if (input == 0) { return IsPadKeyJustPressed(NYA_PAD_KEY_A); }
	if (input == 9) { return IsPadKeyJustPressed(NYA_PAD_KEY_B); }
	return false;
}

bool __thiscall IsGameInputJustPressedNew(Controller* pThis, int input) {
	auto orig = Controller::IsGameInputJustPressed(pThis, input);
	if (orig) return orig;

	if (input == 9) { return IsPadKeyJustPressed(NYA_PAD_KEY_START); } // pause
	if (input == 3) { return IsPadKeyJustPressed(NYA_PAD_KEY_X); } // camera
	if (input == 5) { return IsPadKeyJustPressed(NYA_PAD_KEY_Y); } // reset
	if (input == 18) { return IsPadKeyJustPressed(NYA_PAD_KEY_LB); } // gear down
	if (input == 19) { return IsPadKeyJustPressed(NYA_PAD_KEY_RB); } // gear up
	return false;
}

int __thiscall GetAnalogInputNew(Controller* pThis, int input, float* out) {
	*out = 0.0;
	Controller::GetAnalogInput(pThis, input, out);
	if (*out != 0.0) return 0;

	//if (aPressedAnalog[input]) { *out = 1.0; }
	if (input == 0) { *out = GetPadKeyState(NYA_PAD_KEY_LSTICK_X) / 32767.0; } // steer
	if (input == 1) { *out = GetPadKeyState(NYA_PAD_KEY_RT) / 255.0; } // accelerate
	if (input == 2) { *out = GetPadKeyState(NYA_PAD_KEY_LT) / 255.0; } // brake
	return *out != 0.0;
}

int __thiscall GetInputValueNew(Controller* pThis, int input) {
	auto orig = Controller::GetInputValue(pThis, input);
	if (orig) return orig;

	//if (aPressedDigital[input]) return 255;
	if (input == 0) return GetPadKeyState(NYA_PAD_KEY_B); // handbrake
	if (input == 1 && fTimeSincePaused > 0.75) return GetPadKeyState(NYA_PAD_KEY_A); // nitro
	if (input == 4) return GetPadKeyState(NYA_PAD_KEY_R3); // look back
	return 0;
}

void ProcessXInputSupport() {
	static CNyaTimer gTimer;
	gTimer.Process();

	if (GetGameState() == GAME_STATE_RACE) {
		if (pGameFlow->nIsPaused) fTimeSincePaused = 0;
		fTimeSincePaused += gTimer.fDeltaTime;
	}
	else {
		fTimeSincePaused = 999;
	}
}

void ApplyXInputPatches() {
	NyaHookLib::Patch(0x667494, &IsMenuInputJustPressedNew);
	NyaHookLib::Patch(0x667498, &IsGameInputJustPressedNew);
	NyaHookLib::Patch(0x66749C, &GetInputValueNew);
	NyaHookLib::Patch(0x6674B0, &GetAnalogInputNew);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4F13EB, 0x4F147D); // never allocate more controllers
}