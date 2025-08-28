class CHUD_LapTime : public CIngameHUDElement {
public:
	float fAlpha = 0;

	bool bCentered = false;
	int nNonCenterX = 851;
	int nNonCenterX_1 = 858;

	static inline bool bNewLapRecord = false;
	static void OnLapRecord(Player* pPlayer, uint32_t time) {
		bNewLapRecord = true;
	}

	virtual void Init() {
		ChloeEvents::NewLapRecordEvent.AddHandler(OnLapRecord);
	}

	virtual void Process() {
		static CNyaRaceTimer gTimer;
		gTimer.Process();

		if (!IsRaceHUDUp()) return;
		if (IsInSplitScreen()) return;
		if (bIsSmashyRace) return;
		if (pGameFlow->nEventType != eEventType::RACE) return;
		if (pPlayerHost->nRaceTime < 0) {
			fAlpha = 0;
			bNewLapRecord = false;
			return;
		}

		bool showingLastLap = false;
		auto ply = GetPlayer(0);
		auto lapTime = GetPlayerLapTime(ply, ply->nCurrentLap);
		if (ply->nCurrentLap > 0 && lapTime < 3000) {
			if (lapTime < 2000) {
				fAlpha += gTimer.fDeltaTime * 2;
			}
			else {
				fAlpha -= gTimer.fDeltaTime * 2;
			}
			lapTime = GetPlayerLapTime(ply, ply->nCurrentLap-1);
			showingLastLap = true;
		}
		else if ((ply->nCurrentSplit % pEnvironment->nNumSplitpoints) >= pEnvironment->nNumSplitpoints - 2) {
			fAlpha += gTimer.fDeltaTime * 2;
		}
		else {
			fAlpha -= gTimer.fDeltaTime * 2;
		}
		if (fAlpha < 0) fAlpha = 0;
		if (fAlpha > 1) fAlpha = 1;
		if (fAlpha <= 0) {
			bNewLapRecord = false;
			return;
		}

		NyaDrawing::CNyaRGBA32 rgb = {255,255,255,255};
		rgb.a = fAlpha * 255;
		if (bCentered) {
			DrawElementCenter(0.5, "LAP TIME", FormatGameTime(lapTime), rgb);
		}
		else {
			DrawElementCenter(0.5, "LAP TIME", "", rgb);
			if (lapTime >= 60 * 1000 && lapTime <= 120 * 1000) {
				DrawElementCustomX(JUSTIFY_CENTER, nNonCenterX_1, 0.5, "", FormatGameTime(lapTime), rgb);
			}
			else {
				DrawElementCustomX(JUSTIFY_CENTER, nNonCenterX, 0.5, "", FormatGameTime(lapTime), rgb);
			}
		}

		if (showingLastLap && bNewLapRecord) {
			DrawElementCenter(1.5, "NEW LAP RECORD!", "", rgb);
		}
	}
} HUD_LapTime;