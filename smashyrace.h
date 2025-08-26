namespace SmashyRace {
	double fPlayerGivenTime;
	int nScoreMultiplier;

	double fPlayerTimeLeft = 0;
	int nPlayerScore = 0;

	void SetIsSmashyRace(bool apply) {
		bIsSmashyRace = apply;
		fPlayerGivenTime = 60;
		nScoreMultiplier = 100;

		// hud changes
		NyaHookLib::Patch<uint64_t>(0x454AFC, apply ? 0xE0A190000001EEE9 : 0xE0A1000001ED850F); // remove total time
		NyaHookLib::Patch<uint8_t>(0x4551E9, apply ? 0xEB : 0x74); // remove lap time title
		NyaHookLib::Patch<uint8_t>(0x45578E, apply ? 0xEB : 0x74); // remove lap time counter
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4553FA, apply ? 0x4F0843 : 0x4F0810); // remove lap count title
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x455543, apply ? 0x4F0843 : 0x4F0810); // remove lap count number
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x455597, apply ? 0x4F0843 : 0x4F0810); // remove lap count max
	}

	void ProcessSceneryCrashes() {
		nPlayerScore = 0;
		auto ply = GetPlayer(0);
		for (int i = 0; i < 10; i++) {
			nPlayerScore += ply->pCar->aObjectsSmashed[i] * fBonusTypePrice[i] * nScoreMultiplier;
		}
	}

	void OnTick() {
		static CNyaRaceTimer gTimer;
		gTimer.Process();

		if (pLoadingScreen) return;
		if (!bIsSmashyRace) return;
		if (GetGameState() != GAME_STATE_RACE) return;

		ArcadeMode::nCurrentEventScore = nPlayerScore;

		ProcessSceneryCrashes();
		if (pPlayerHost->nRaceTime < 0) {
			fPlayerTimeLeft = fPlayerGivenTime;
			nPlayerScore = 0;
			return;
		}

		GetScoreManager()->nNumLaps = 10;

		fPlayerTimeLeft -= gTimer.fDeltaTime;

		auto ply = GetPlayerScore<PlayerScoreRace>(1);
		if (fPlayerTimeLeft <= 0 && !ply->bHasFinished) {
			ply->bHasFinished = true;
			ply->nFinishTime = pPlayerHost->nRaceTime;
		}
	}

	class CHUD_SmashyRace : public CIngameHUDElement {
	public:

		tDrawPositions1080p gCheckpointBonus = {240, 85, 0.03};
		static inline tDrawPositions gBase = {0.008, 0.029, 0.042, 0, 0.034};
		float fElementTotalSpacing = 0.092;

		void DrawElement(int id, const std::string& title, const std::string& value) {
			tNyaStringData data;
			data.x = gBase.fPosX * GetAspectRatioInv();
			data.y = gBase.fPosY + id * fElementTotalSpacing;
			data.size = gBase.fSize;
			data.SetColor(GetPaletteColor(18));
			DrawStringFO2_Ingame12(data, title);
			data.y += gBase.fSpacingY;
			data.SetColor({255,255,255,255});
			DrawStringFO2_Ingame24(data, value);
		}

		void DrawElementCenter(int id, const std::string& title, const std::string& value) {
			tNyaStringData data;
			data.x = 0.5;
			data.y = gBase.fPosY + id * fElementTotalSpacing;
			data.size = gBase.fSize;
			data.XCenterAlign = true;
			data.SetColor(GetPaletteColor(18));
			DrawStringFO2_Ingame12(data, title);
			data.y += gBase.fSpacingY;
			data.SetColor({255,255,255,255});
			DrawStringFO2_Ingame24(data, value);
		}

		virtual void Process() {
			if (!IsRaceHUDUp()) return;
			if (!bIsSmashyRace) return;

			int timeLeft = fPlayerTimeLeft*1000;
			if (timeLeft < 0) timeLeft = 0;

			std::string timeLeftString = GetTimeFromMilliseconds(timeLeft, true);
			timeLeftString.pop_back();
			// leading zero
			if (fPlayerTimeLeft < 60 * 10) {
				timeLeftString = "0" + timeLeftString;
			}

			DrawElement(0, "TIME LEFT", timeLeftString);
			DrawElementCenter(1, "SCORE", std::to_string(nPlayerScore));
		}
	} HUD_SmashyRace;
}