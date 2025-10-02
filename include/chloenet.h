namespace ChloeNet {
	template<typename T>
	T GetFuncPtr(const char* funcName) {
		if (auto dll = LoadLibraryA("FlatOutCustomMP_gcp.dll")) {
			return (T)GetProcAddress(dll, funcName);
		}
		return nullptr;
	}

	void OnCrashBonus(int type) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(int)>("ChloeNet_OnCrashBonus");
		if (!funcPtr) return;
		funcPtr(type);
	}

	bool IsReplicatedPlayer(Player* player) {
		static auto funcPtr = GetFuncPtr<bool(__cdecl*)(Player*)>("ChloeNet_IsReplicatedPlayer");
		if (!funcPtr) return false;
		return funcPtr(player);
	}

	bool IsReplicatedPlayerWrecked(Player* player) {
		static auto funcPtr = GetFuncPtr<bool(__cdecl*)(Player*)>("ChloeNet_IsReplicatedPlayerWrecked");
		if (!funcPtr) return false;
		return funcPtr(player);
	}

	int GetReplicatedPlayerArcadeScore(Player* player) {
		static auto funcPtr = GetFuncPtr<int(__cdecl*)(Player*)>("ChloeNet_GetReplicatedPlayerArcadeScore");
		if (!funcPtr) return 0;
		return funcPtr(player);
	}

	int GetReplicatedPlayerColor(Player* player) {
		static auto funcPtr = GetFuncPtr<int(__cdecl*)(Player*)>("ChloeNet_GetReplicatedPlayerColor");
		if (!funcPtr) return 0;
		return funcPtr(player);
	}

	void SetSpectate(bool on) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool)>("ChloeNet_SetSpectate");
		if (!funcPtr) return;
		return funcPtr(on);
	}

	bool IsSpectating() {
		static auto funcPtr = GetFuncPtr<bool(__cdecl*)()>("ChloeNet_IsSpectating");
		if (!funcPtr) return false;
		return funcPtr();
	}

	bool CanSpectate() {
		static auto funcPtr = GetFuncPtr<bool(__cdecl*)()>("ChloeNet_CanSpectate");
		if (!funcPtr) return false;
		return funcPtr();
	}
}