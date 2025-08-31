namespace ChloeCollection {
	template<typename T>
	T GetFuncPtr(const char* funcName) {
		if (auto dll = LoadLibraryA("FlatOutChloeCollection_gcp.dll")) {
			return (T)GetProcAddress(dll, funcName);
		}
		return nullptr;
	}

	int GetPlayerArcadeScore(Player* pPlayer) {
		static auto funcPtr = GetFuncPtr<int(__cdecl*)(Player*)>("ChloeCollection_GetPlayerArcadeScore");
		if (!funcPtr) return 0;
		return funcPtr(pPlayer);
	}

	int GetLocalPlayerArcadeScore() {
		static auto funcPtr = GetFuncPtr<int(__cdecl*)()>("ChloeCollection_GetLocalPlayerArcadeScore");
		if (!funcPtr) return 0;
		return funcPtr();
	}
}