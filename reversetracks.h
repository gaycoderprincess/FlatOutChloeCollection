bool bIsTrackReversed = false;

int GetReversedSplitID(int id, int max) {
	if (id != max) return max - id;
	return id;
}

void __cdecl GetSplitpointIDReversed(void* a1, int a2, int a3) {
	if (bIsTrackReversed) {
		// count 5
		// 1 2 3 4 5
		// 4 3 2 1 5

		a3 = GetReversedSplitID(a3, pEnvironment->nNumSplitpoints);
	}
	lua_rawgeti(a1, a2, a3);
}

int GetReversedSplineID(int id, int max) {
	// count 34
	// 1 2 3 4 5 6 ..
	// 2 1 34 33 32 ..
	if (id == 1) return 2;
	if (id == 2) return 1;
	return max - (id - 3);

	// count 5
	// 1 2 3 4 5
	// 5 4 3 2 1
	//return (max + 1) - id;
}

void __cdecl GetRaceLineIDReversed(void* a1, int a2, int a3) {
	if (bIsTrackReversed) {
		a3 = GetReversedSplineID(a3, pTrackAI->pAIRaceLine->nNumSplines);
	}
	lua_rawgeti(a1, a2, a3);
}

void __cdecl GetBorderLineLeftIDReversed(void* a1, int a2, int a3) {
	if (bIsTrackReversed) {
		a3 = GetReversedSplineID(a3, pTrackAI->pAIBorderLineLeft->nNumSplines);
	}
	lua_rawgeti(a1, a2, a3);
}

void __cdecl GetBorderLineRightIDReversed(void* a1, int a2, int a3) {
	if (bIsTrackReversed) {
		a3 = GetReversedSplineID(a3, pTrackAI->pAIBorderLineRight->nNumSplines);
	}
	lua_rawgeti(a1, a2, a3);
}

void SetTrackReversed(bool apply) {
	bIsTrackReversed = apply;
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4697F7, &GetSplitpointIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x469845, &GetSplitpointIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x469893, &GetSplitpointIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4024CA, &GetRaceLineIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4024F6, &GetRaceLineIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x40263D, &GetBorderLineRightIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x402669, &GetBorderLineRightIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4027FE, &GetBorderLineLeftIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x40282A, &GetBorderLineLeftIDReversed);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4011E4, apply ? 0x402BB0 : 0x4028D0); // disable sectors.ai
	NyaHookLib::Patch(0x4026C8 + 1, apply ? "AIBorderLineRight" : "AIBorderLineLeft");
	NyaHookLib::Patch(0x402762 + 1, apply ? "AIBorderLineRight" : "AIBorderLineLeft");
	NyaHookLib::Patch(0x40254F + 1, apply ? "AIBorderLineLeft" : "AIBorderLineRight");
	NyaHookLib::Patch(0x402595 + 1, apply ? "AIBorderLineLeft" : "AIBorderLineRight");
	NyaHookLib::Patch(0x469857 + 1, apply ? "Right" : "Left");
	NyaHookLib::Patch(0x4698A5 + 1, apply ? "Left" : "Right");
}

void ReverseTrackStartpoints() {
	if (!bIsTrackReversed) return;

	for (int i = 0; i < 8; i++) {
		pEnvironment->aStartpoints[i].fMatrix[0] *= -1; // x.x
		pEnvironment->aStartpoints[i].fMatrix[1] *= -1; // x.y
		pEnvironment->aStartpoints[i].fMatrix[2] *= -1; // x.z
		pEnvironment->aStartpoints[i].fMatrix[8] *= -1; // z.x
		pEnvironment->aStartpoints[i].fMatrix[9] *= -1; // z.y
		pEnvironment->aStartpoints[i].fMatrix[10] *= -1; // z.z
	}
}