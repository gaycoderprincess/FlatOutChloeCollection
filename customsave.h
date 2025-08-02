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
	void Clear() {
		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();
	}
	void Load() {
		int saveSlot = pGameFlow->nSaveSlot;
		if (saveSlot < 0) {
			saveSlot = pGameFlow->Profile.nAutosaveSlot;
		}

		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();

		auto file = std::ifstream(GetCustomSavePath(saveSlot+1), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		file.read((char*)this, sizeof(*this));
	}
	void Save() {
		if (!bInitialized) return;

		int saveSlot = pGameFlow->nSaveSlot;
		if (saveSlot < 0) {
			saveSlot = pGameFlow->Profile.nAutosaveSlot;
		}

		if (saveSlot < 0) {
			MessageBoxA(0, "Trying to save to slot 0, this is a bug", "nya?!~", MB_ICONWARNING);
		}

		ReadPlayerSettings();

		auto file = std::ofstream(GetCustomSavePath(saveSlot+1), std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		file.write((char*)this, sizeof(*this));
	}
	void Delete(int slot) {
		auto save = GetCustomSavePath(slot+1);
		if (std::filesystem::exists(save)) {
			std::filesystem::remove(save);
		}
	}
} gCustomSave;