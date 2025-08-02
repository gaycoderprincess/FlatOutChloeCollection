float fNitroAirtimeTolerance = 0.5;
float fNitroAirtimeRate = 2.5;

void ProcessNitroGain() {
	static CNyaTimer gTimer;
	gTimer.Process();

	if (GetGameState() != GAME_STATE_RACE) return;
	if (pLoadingScreen) return;
	if (pGameFlow->nEventType != eEventType::RACE) return;

	auto localPlayer = GetPlayerScore<PlayerScoreRace>(1);
	if (localPlayer->bHasFinished || localPlayer->bIsDNF) return;
	if (pPlayerHost->nRaceTime <= 0) return;

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (ply->pCar->fTimeInAir < fNitroAirtimeTolerance) continue;

		ply->pCar->fNitro += fNitroAirtimeRate * gTimer.fDeltaTime;
		if (ply->pCar->fNitro >= ply->pCar->fMaxNitro) ply->pCar->fNitro = ply->pCar->fMaxNitro;
	}
}