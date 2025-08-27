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
}