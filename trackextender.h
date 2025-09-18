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
	NyaHookLib::Patch(0x402942 + 1, apply ? "data/sectors_reversed.ai" : "data/sectors.ai");
	NyaHookLib::Patch(0x4026C8 + 1, apply ? "AIBorderLineRight" : "AIBorderLineLeft");
	NyaHookLib::Patch(0x402762 + 1, apply ? "AIBorderLineRight" : "AIBorderLineLeft");
	NyaHookLib::Patch(0x40254F + 1, apply ? "AIBorderLineLeft" : "AIBorderLineRight");
	NyaHookLib::Patch(0x402595 + 1, apply ? "AIBorderLineLeft" : "AIBorderLineRight");
	NyaHookLib::Patch(0x469857 + 1, apply ? "Right" : "Left");
	NyaHookLib::Patch(0x4698A5 + 1, apply ? "Left" : "Right");
	ChloeEvents::MapLoadEvent.AddHandler(ReverseTrackStartpoints);
}

void SetExclusiveGeometryFolder(bool apply) {
	NyaHookLib::Patch(0x4BC963 + 2, apply ? 0x1138 : 0x111C);
	NyaHookLib::Patch(0x4BC4F8 + 2, apply ? 0x1150 : 0x1134);
	NyaHookLib::Patch(0x4BC505 + 2, apply ? 0x113C : 0x1120);
	NyaHookLib::Patch(0x4BC50D + 2, apply ? 0x113C : 0x1120);
	NyaHookLib::Patch(0x4BC526 + 2, apply ? 0x1150 : 0x1134);
	NyaHookLib::Patch(0x4BC533 + 2, apply ? 0x113C : 0x1120);
	NyaHookLib::Patch(0x4BC53B + 2, apply ? 0x113C : 0x1120);
	NyaHookLib::Patch<uint8_t>(0x4CA19F + 2, apply ? 0x1C*2 : 0x1C);
}

void SetTrackCustomProperties() {
	SetExclusiveGeometryFolder(DoesFileExist(std::format("{}geometry/track_geom.w32", pEnvironment->sStagePath.Get()).c_str()));
}

auto NoFO2WindowProps_call = (void(__stdcall*)(void*, const char*, void*, void*, void*, void*, void*, void*, void*))0x4C95E0;
void __stdcall NoFO2WindowProps(void* a1, const char* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8, void* a9) {
	if (!strcmp(a2, "window")) return;
	if (DoesTrackValueExist(pGameFlow->nLevel, "NoProps")) return;
	return NoFO2WindowProps_call(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

void ApplyTrackExtenderPatches() {
	ChloeEvents::MapPreLoadEvent.AddHandler(SetTrackCustomProperties);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4CD314, &NoFO2WindowProps);
}