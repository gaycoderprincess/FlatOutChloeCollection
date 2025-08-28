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
		NyaHookLib::Patch<uint8_t>(0x454A84, apply ? 0xEB : 0x74); // remove supposedly crashing code that's disabled for time trials
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4553FA, apply ? 0x4F0843 : 0x4F0810); // remove lap count title
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x455543, apply ? 0x4F0843 : 0x4F0810); // remove lap count number
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x455597, apply ? 0x4F0843 : 0x4F0810); // remove lap count max
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x459F79, apply ? (uintptr_t)&ArcadeMode::DrawPressStartMenuASM : 0x45AC50); // remove vanilla press start menu
		NyaHookLib::Patch<uint8_t>(0x44FE9D, apply ? 0xEB : 0x74); // remove out of map popup
		NyaHookLib::Patch<uint8_t>(0x450B20, apply ? 0xC3 : 0x55); // remove out of map arrow
	}

	void ProcessSceneryCrashes() {
		auto score = GetPlayerScore<PlayerScoreRace>(1);
		if (score->bHasFinished || score->bIsDNF) return;

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

		ArcadeMode::ProcessTimerTick(fPlayerTimeLeft*1000);

		auto ply = GetPlayerScore<PlayerScoreRace>(1);
		if (fPlayerTimeLeft <= 0 && !ply->bHasFinished && !ply->bIsDNF) {
			ply->bHasFinished = true;
			ply->nFinishTime = pPlayerHost->nRaceTime;
		}
	}

	void Init() {
		ChloeEvents::FinishFrameEvent.AddHandler(OnTick);
	}

	class CHUD_SmashyRace : public CIngameHUDElement {
	public:

		tDrawPositions1080p gCheckpointBonus = {240, 85, 0.03};

		virtual void Process() {
			if (!IsRaceHUDUp()) return;
			if (!bIsSmashyRace) return;

			int timeLeft = fPlayerTimeLeft*1000;
			if (timeLeft < 0) timeLeft = 0;

			DrawElement(0, "TIME LEFT", FormatGameTime(timeLeft), timeLeft <= 4500 ? NyaDrawing::CNyaRGBA32(200,0,0,255) : NyaDrawing::CNyaRGBA32(255,255,255,255));
			//DrawElement(0, "TIME LEFT", timeLeftString, timeLeft <= 4500 ? GetPaletteColor(22) : NyaDrawing::CNyaRGBA32(255,255,255,255));
			DrawElementCenter(1, "SCORE", FormatScore(nPlayerScore));
		}
	} HUD_SmashyRace;
}