int GetHandlingMode(Player* pPlayer) {
	if (pPlayer && !bIsInMultiplayer && pPlayer->nPlayerType != PLAYERTYPE_LOCAL) return HANDLING_NORMAL;
	
	int handlingMode = nHandlingMode;
	if (CareerMode::IsCareerTimeTrial()) handlingMode = HANDLING_NORMAL;
	if (bIsArcadeMode && handlingMode == HANDLING_HARDCORE) handlingMode = HANDLING_PROFESSIONAL;
	if (bIsInMultiplayer) handlingMode = nMultiplayerHandlingMode;
	return handlingMode;
}

void __fastcall SetHandlingModeForCar(Car* car) {
	bool hardcore = GetHandlingMode(car->pPlayer) == HANDLING_HARDCORE;
	NyaHookLib::Patch<uint64_t>(0x418B39, hardcore ? 0x86D9900000038DE9 : 0x86D90000038C840F);
	NyaHookLib::Patch<uint64_t>(0x419211, hardcore ? 0x44D990000003D7E9 : 0x44D9000003D6850F);
}

void SetHandlingMode() {
	static int nLast = -1;
	auto handlingMode = GetHandlingMode(nullptr);
	if (nLast != handlingMode) {
		NyaHookLib::Patch(0x4B9F66 + 1, handlingMode == HANDLING_HARDCORE ? "FrictionHardcore" : "Friction");
		nLast = handlingMode;
	}
}

uintptr_t SetHandlingModeASM_jmp = 0x4189D6;
int __attribute__((naked)) SetHandlingModeASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"sub esp, 0x8C\n\t"
		"jmp %0\n\t"
			:
			:  "m" (SetHandlingModeASM_jmp), "i" (SetHandlingModeForCar)
	);
}

void ApplyHandlingModePatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4189D0, &SetHandlingModeASM);
}