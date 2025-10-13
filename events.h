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

	// track init - after all AI data is initialized
	class EventMapLoad : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} MapLoadEvent;

	// environment init - before any AI data is loaded, also triggered during menu load
	class EventMapPreLoad : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} MapPreLoadEvent;

	// race init - before any map data is loaded
	class EventRacePreLoad : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} RacePreLoadEvent;

	class EventDrawRaceUI : public ChloeEvent<void(*)(int)> {
	public:
		void OnHit(int layer) {
			for (auto& func : functions) {
				func(layer);
			}
		}
	} DrawRaceUIEvent; // used for ingame race UI

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

	class EventSaveCleared : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} SaveClearedEvent;

	class EventPlayerReset : public ChloeEvent<void(*)(Player*)> {
	public:
		void OnHit(Player* pPlayer) {
			for (auto& func : functions) {
				func(pPlayer);
			}
		}
	} PlayerResetEvent;

	class EventRaceRestart : public ChloeEvent<void(*)()> {
	public:
		void OnHit() {
			for (auto& func : functions) {
				func();
			}
		}
	} RaceRestartEvent;
}

namespace ChloeEventHooks {
	auto OnMapLoad_call = (void(__stdcall*)(void*, void*, void*))0x44AD00;
	void __stdcall OnMapLoad(void* a1, void* a2, void* a3) {
		ChloeEvents::RacePreLoadEvent.OnHit();
		OnMapLoad_call(a1, a2, a3);
		ChloeEvents::MapLoadEvent.OnHit();
	}
	
	auto OnMapPreLoad_call = (void(__stdcall*)(int, int, int, int, int, int, int, float, char))0x4B9250;
	void __stdcall OnMapPreLoad(int a1, int a2, int a3, int a4, int a5, int a6, int a7, float a8, char a9) {
		ChloeEvents::MapPreLoadEvent.OnHit();
		return OnMapPreLoad_call(a1, a2, a3, a4, a5, a6, a7, a8, a9);
	}
	
	void OnFilesystemInit() {
		ChloeEvents::FilesystemInitEvent.OnHit();
	}
	
	uintptr_t OnFilesystemInitASM_jmp = 0x4398C0;
	void __attribute__((naked)) OnFilesystemInitASM() {
		__asm__ (
			"pushad\n\t"
			"call %1\n\t"
			"popad\n\t"
			"jmp %0\n\t"
				:
				:  "m" (OnFilesystemInitASM_jmp), "i" (OnFilesystemInit)
		);
	}
	
	void Init() {
		NyaHookLib::Patch<uint16_t>(0x45314F, 0x9090); // enable map drawing in stunt maps
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x451CE3, &OnMapLoad);
		
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x468DA9, &OnMapPreLoad); // ingame
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4655EF, &OnMapPreLoad); // menu
		
		OnFilesystemInitASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A7261, &OnFilesystemInitASM);
	}
}