std::string GetCustomSavePath(int id) {
	return std::format("Savegame/customsave{:03}.sav", id);
}

struct tCustomSaveStructure {
	struct {
		uint32_t bIsLocked : 1;
		uint32_t bIsPurchased : 1;
		uint8_t nSkinId;
		uint8_t nUpgrades[19];
		uint32_t nUpgradesValue;

		bool IsUpgradePurchased(int upgrade) {
			uint32_t bit = 1 << (upgrade % 8);
			return (nUpgrades[upgrade / 8] & bit) != 0;
		}
		void SetUpgradePurchased(int upgrade, bool purchased) {
			uint32_t bit = 1 << (upgrade % 8);
			if (purchased) {
				nUpgrades[upgrade / 8] |= bit;
			}
			else {
				nUpgrades[upgrade / 8] &= ~bit;
			}
		}
		void ApplyUpgradesToCar() {
			pGameFlow->Profile.nNumCarUpgrades = 0;

			for (int i = 0; i < 19*8; i++) {
				if (IsUpgradePurchased(i)) {
					pGameFlow->Profile.aCarUpgrades[pGameFlow->Profile.nNumCarUpgrades++] = i;
				}
				if (pGameFlow->Profile.nNumCarUpgrades >= 40) break;
			}
		}
	} aCareerGarage[256];

	static inline bool bInitialized = false;

	tCustomSaveStructure() {
		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();
	}
	void SetDefaultPlayerSettings() {

	}
	void ApplyPlayerSettings() const {

	}
	void ReadPlayerSettings() {

	}
	void Load() {
		bInitialized = true;

		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();

		auto file = std::ifstream(GetCustomSavePath(pGameFlow->nSaveSlot+1), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		file.read((char*)this, sizeof(*this));
	}
	void Save() {
		if (!bInitialized) return;

		ReadPlayerSettings();

		auto file = std::ofstream(GetCustomSavePath(pGameFlow->nSaveSlot+1), std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		file.write((char*)this, sizeof(*this));
	}
} gCustomSave;