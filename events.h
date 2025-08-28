namespace ChloeEvents {
	template<typename T>
	class ChloeEvent {
	protected:
		std::vector<T> functions;

	public:
		void AddHandler(T function) {
			// check for duplicates
			for (auto& func : functions) {
				if (func == function) return;
			}
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

	class EventNewLapRecord : public ChloeEvent<void(*)(Player*, uint32_t)> {
	public:
		void OnHit(Player* pPlayer, uint32_t lapTime) {
			for (auto& func : functions) {
				func(pPlayer, lapTime);
			}
		}
	} NewLapRecordEvent;

	class EventFilesystemInit : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} FilesystemInitEvent;

	class EventMapLoaded : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} MapLoadedEvent;

	class EventDrawUI : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} DrawUIEvent; // used for ingame race UI

	class EventDrawAboveUI : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} DrawAboveUIEvent; // loosely used for drawing per-frame stuff

	class EventFinishFrame : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} FinishFrameEvent; // loosely used for non-drawing per-frame stuff

	class EventSaveCreated : public ChloeEvent<void(*)(int)> {
	public:
		void OnHit(int saveSlot) {
			for (auto& func : functions) {
				func(saveSlot);
			}
		}
	} SaveCreatedEvent;

	class EventSaveLoaded : public ChloeEvent<void(*)(int)> {
	public:
		void OnHit(int saveSlot) {
			for (auto& func : functions) {
				func(saveSlot);
			}
		}
	} SaveLoadedEvent;

	class EventSaveDeleted : public ChloeEvent<void(*)(int)> {
	public:
		void OnHit(int saveSlot) {
			for (auto& func : functions) {
				func(saveSlot);
			}
		}
	} SaveDeletedEvent;
}