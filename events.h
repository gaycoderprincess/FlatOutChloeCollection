namespace ChloeEvents {
	std::vector<void(*)(Player* pPlayer, int type)> OnCrashBonus;
	std::vector<void(*)(Player* pPlayer)> OnPlayerWrecked;
	std::vector<void(*)(Player* pPlayer)> OnDerbyTimeout;

	void CrashBonus(Player* pPlayer, int type) {
		for (auto& event : OnCrashBonus) {
			event(pPlayer, type);
		}
	}
	void PlayerWrecked(Player* pPlayer) {
		for (auto& event : OnPlayerWrecked) {
			event(pPlayer);
		}
	}
	void DerbyTimeout(Player* pPlayer) {
		for (auto& event : OnDerbyTimeout) {
			event(pPlayer);
		}
	}
}