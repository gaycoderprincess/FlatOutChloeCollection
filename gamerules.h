namespace GameRules {
	toml::table* GameRulesGlobal = nullptr;
	toml::table* GameRulesDerby = nullptr;
	toml::table* GameRulesFragDerby = nullptr;
	toml::table* GameRulesSmashySmash = nullptr;
	toml::table* GameRulesArcadeRace = nullptr;
	toml::table* GetGameRules() {
		if (pGameFlow->nEventType == eEventType::DERBY) {
			if (bIsFragDerby) return GameRulesFragDerby;
			return GameRulesDerby;
		}
		if (bIsCarnageRace) return GameRulesArcadeRace;
		if (bIsSmashyRace) return GameRulesSmashySmash;
		return GameRulesGlobal;
	}

	float Get(const char* value, toml::table* table) {
		if (!table) table = GetGameRules();
		return (*table)[value].value_or((*GameRulesGlobal)[value].value_or(0.0));
	}

	float GetArray(const char* value, int i, toml::table* table) {
		if (!table) table = GetGameRules();
		return (*table)[value][i].value_or((*GameRulesGlobal)[value][i].value_or(0.0));
	}

	class KeyValue {
	public:
		const char* key;
		float value = 0;
		toml::table* table = nullptr;

		static inline std::vector<KeyValue*> aKeyValues;

		KeyValue() = delete;
		explicit KeyValue(const char* key, toml::table* table = nullptr) : key(key), table(table) {
			aKeyValues.push_back(this);
		}
		operator float() const { return value; }
	};

	class KeyValueArray {
	public:
		const char* key;
		int count = 0;
		float* values = nullptr;
		toml::table* table = nullptr;

		static inline std::vector<KeyValueArray*> aKeyValues;

		KeyValueArray() = delete;
		explicit KeyValueArray(const char* key, int count, toml::table* table = nullptr) : key(key), count(count), table(table) {
			values = new float[count];
			aKeyValues.push_back(this);
		}
		float operator[] (int i) const { return values[i]; }
	};

	void OnTick() {
		static toml::table* lastRule = nullptr;
		auto rule = GetGameRules();
		if (lastRule != rule) {
			for (auto& var : KeyValue::aKeyValues) {
				var->value = Get(var->key, var->table);
			}
			for (auto& var : KeyValueArray::aKeyValues) {
				for (int i = 0; i < var->count; i++) {
					var->values[i] = GetArray(var->key, i, var->table);
				}
			}
			lastRule = rule;
		}
	}

	void Load() {
		GameRulesGlobal = new toml::table();
		GameRulesDerby = new toml::table();
		GameRulesFragDerby = new toml::table();
		GameRulesSmashySmash = new toml::table();
		GameRulesArcadeRace = new toml::table();

		*GameRulesGlobal = ReadTOMLFromBfs("data/database/gamerules/global.toml");
		*GameRulesDerby = ReadTOMLFromBfs("data/database/gamerules/derby.toml");
		*GameRulesFragDerby = ReadTOMLFromBfs("data/database/gamerules/fragderby.toml");
		*GameRulesSmashySmash = ReadTOMLFromBfs("data/database/gamerules/smashysmash.toml");
		*GameRulesArcadeRace = ReadTOMLFromBfs("data/database/gamerules/arcaderace.toml");
	}

	ChloeHook Init([]() {
		ChloeEvents::FilesystemInitEvent.AddHandler(Load);
		ChloeEvents::FinishFrameEvent.AddHandler(OnTick);
	});
}

GameRules::KeyValue WhammoCrashVelocity1("WhammoCrashVelocity1");
GameRules::KeyValue PowerHitCrashVelocity1("PowerHitCrashVelocity1");
GameRules::KeyValue BlastOutCrashVelocity1("BlastOutCrashVelocity1");
GameRules::KeyValue RagdollPiggybagThreshold("RagdollPiggybagThreshold");
GameRules::KeyValue WreckPiggybagThreshold("WreckPiggybagThreshold");
GameRules::KeyValue FlipPiggybagThreshold("FlipPiggybagThreshold");
GameRules::KeyValue FlipComboThreshold("FlipComboThreshold");
GameRules::KeyValue CrashVelocityMultiplier("CrashVelocityMultiplier");