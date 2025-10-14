float fNitroAirtimeTolerance = 0.5;
float fNitroAirtimeRate = 2.5;
bool bNitroRegen = false;
float fNitroRegenerationRate = 0.05;

void ProcessNitroGain() {
	if (GetGameState() != GAME_STATE_RACE) return;
	if (pLoadingScreen) return;
	if (pGameFlow->nEventType != eEventType::RACE && !IsNitroEnabledInDerby()) return;
	if (pPlayerHost->nRaceTime <= 0) return;

	if (bIsInMultiplayer) bNitroRegen = bMultiplayerNitroRegen;
	else bNitroRegen = DoesTrackValueExist(pGameFlow->nLevel, "ArenaMode") || bIsCarnageRace || bIsSmashyRace;

	static CNyaRaceTimer gTimer;
	gTimer.Process();

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		auto score = GetPlayerScore(ply->nPlayerId);
		if (score->bHasFinished || score->bIsDNF) continue;

		if (bIsInMultiplayer) {
			if (!bMultiplayerNitroOn) {
				ply->pCar->GetNitro() = 0.0;
				return;
			}
		}

		if (bIsQuickRace) {
			if (QuickRace::nNitroLevel == QuickRace::NITRO_0) {
				ply->pCar->GetNitro() = 0.0;
				return;
			} else if (QuickRace::nNitroLevel == QuickRace::NITRO_INFINITE) {
				ply->pCar->GetNitro() = ply->pCar->GetMaxNitro();
				return;
			}
		}

		if (ply->pCar->fTimeInAir >= fNitroAirtimeTolerance) {
			ply->pCar->GetNitro() += fNitroAirtimeRate * gTimer.fDeltaTime;
			if (ply->pCar->GetNitro() >= ply->pCar->GetMaxNitro()) ply->pCar->GetNitro() = ply->pCar->GetMaxNitro();
		}
		if (bNitroRegen && pPlayerHost->nRaceTime > 5000 && ply->fNitroButton <= 0.0) {
			ply->pCar->GetNitro() += fNitroRegenerationRate * score->nPosition * gTimer.fDeltaTime;
		}
		// make AI use nitro
		if (ply->nPlayerType == PLAYERTYPE_AI) {
			float propNitro = 0;
			for (int j = 0; j < 10; j++) {
				propNitro += ply->pCar->aObjectsSmashed[j] * fBonusTypeMayhem[j];
			}
			ply->pCar->GetNitro() += (ply->fNitroFromPropsLast = (propNitro - ply->fNitroFromPropsTotal));
			ply->fNitroFromPropsTotal = propNitro;

			for (auto& collision : ply->pCar->aCarCollisions) {
				if (collision.damage > 0.0) {
					ply->pCar->GetNitro() += collision.damage * ply->pCar->GetMaxNitro();
					collision.damage = 0.0;
				}
			}

			//ply->fNitroButton = ply->pCar->fNitroButton = ply->fGasPedal >= 0.9 && ply->fBrakePedal <= 0.1;
			ply->fNitroButton = ply->pCar->fNitroButton = ply->pCar->GetNitro() > 0.5 && ply->pCar->GetVelocity()->length() > 10 && ply->fGasPedal >= 0.5 && ply->fBrakePedal <= 0.1;
		}

		if (ply->pCar->GetNitro() >= ply->pCar->GetMaxNitro()) ply->pCar->GetNitro() = ply->pCar->GetMaxNitro();
	}
}

ChloeHook Hook_NitroGain([]() {
	NyaHookLib::Fill(0x4147B5, 0x90, 6); // AI nitro gain for ragdolling
	NyaHookLib::Patch<uint16_t>(0x41B9F0, 0x9090); // record prop hits for ai
	NyaHookLib::Patch<uint16_t>(0x41D915, 0x9090); // register prop hit handler for ai

	// enable car-to-car damage tracking for ai
	NyaHookLib::Patch<uint16_t>(0x416718, 0x9090);
	NyaHookLib::Patch<uint16_t>(0x41671D, 0x9090);

	ChloeEvents::FinishFrameEvent.AddHandler(ProcessNitroGain);
});