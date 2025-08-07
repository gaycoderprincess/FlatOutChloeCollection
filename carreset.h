double fCarResetFadeTimer = 0;
bool bCarResetRequested = 0;

float fCarResetSpeed = 15;

void ProcessCarReset() {
	if (!bCarResetRequested && fCarResetFadeTimer <= 0) return;
	if (pLoadingScreen || GetGameState() != GAME_STATE_RACE) {
		bCarResetRequested = false;
		return;
	}

	static CNyaRaceTimer gTimer;
	gTimer.Process();

	auto ply = GetPlayer(0);
	if (!ply || pPlayerHost->nRaceTime <= 0) {
		bCarResetRequested = false;
		return;
	}

	if (!bCarResetRequested) {
		fCarResetFadeTimer -= gTimer.fDeltaTime * 1.5;
	}
	else {
		fCarResetFadeTimer += gTimer.fDeltaTime * 1.5;
		if (fCarResetFadeTimer >= 1) {
			ply->ResetCar(ply, 0);
			*ply->pCar->GetVelocity() = ply->pCar->GetMatrix()->z * fCarResetSpeed;
			bCarResetRequested = false;
		}
	}

	if (fCarResetFadeTimer < 0) fCarResetFadeTimer = 0;
	if (fCarResetFadeTimer > 1) fCarResetFadeTimer = 1;

	uint8_t alpha = fCarResetFadeTimer*255;
	if (!pGameFlow->nIsPaused) DrawRectangle(0,1,0,1,{0,0,0,alpha});
}

int __thiscall ResetCarNew(Player* pPlayer, int a2) {
	if (pGameFlow->nEventType == eEventType::STUNT) return 1;
	if (pPlayerHost->nRaceTime < 5000) return 1;
	bCarResetRequested = true;
	return 1;
}

void ApplyCarResetPatches() {
	NyaHookLib::Patch(0x6605CC, &ResetCarNew);
}