namespace ChloeEvents {
	template<typename T>
	class ChloeEvent {
	protected:
		std::vector<T> functions;

	public:
		void AddHandler(T function) {
			functions.push_back(function);
		}
	};

	class EventCrashBonus : public ChloeEvent<void(*)(Player*, int)> {
	public:
		void OnHit(Player* pPlayer, int type) {
			for (auto& func : functions) {
				func(pPlayer, type);
			}
		}
	} CrashBonusEvent;

	class EventPlayerWrecked : public ChloeEvent<void(*)(Player*)> {
	public:
		void OnHit(Player* pPlayer) {
			for (auto& func : functions) {
				func(pPlayer);
			}
		}
	} PlayerWreckedEvent;

	class EventDerbyTimeout : public ChloeEvent<void(*)(Player*)> {
	public:
		void OnHit(Player* pPlayer) {
			for (auto& func : functions) {
				func(pPlayer);
			}
		}
	} DerbyTimeoutEvent;
}