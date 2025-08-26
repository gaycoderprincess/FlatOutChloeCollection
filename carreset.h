double fCarResetFadeTimer[4] = {};
bool bCarResetRequested[4] = {};

float fCarResetSpeed = 15;

void ProcessCarReset(int player, float delta) {
	if (player > 0 && !IsInSplitScreen()) return;

	if (!bCarResetRequested[player] && fCarResetFadeTimer[player] <= 0) return;
	if (pLoadingScreen || GetGameState() != GAME_STATE_RACE) {
		fCarResetFadeTimer[player] = 0;
		bCarResetRequested[player] = false;
		return;
	}

	auto ply = GetPlayer(player);
	if (!ply || pPlayerHost->nRaceTime <= 0) {
		fCarResetFadeTimer[player] = 0;
		bCarResetRequested[player] = false;
		return;
	}

	if (!bCarResetRequested[player]) {
		fCarResetFadeTimer[player] -= delta * 1.5;
	}
	else {
		fCarResetFadeTimer[player] += delta * 1.5;
		if (fCarResetFadeTimer[player] >= 1) {
			if (!IsPlayerWrecked(ply)) {
				ply->ResetCar(ply, 0);
				if (pGameFlow->nEventType != eEventType::DERBY) {
					*ply->pCar->GetVelocity() = ply->pCar->GetMatrix()->z * fCarResetSpeed;
				}
			}
			bCarResetRequested[player] = false;
		}
	}

	if (fCarResetFadeTimer[player] < 0) fCarResetFadeTimer[player] = 0;
	if (fCarResetFadeTimer[player] > 1) fCarResetFadeTimer[player] = 1;

	uint8_t alpha = fCarResetFadeTimer[player]*255;
	if (!pGameFlow->nIsPaused) {
		if (IsInQuarteredSplitScreen()) {
			switch (player) {
				case 0:
					DrawRectangle(0, 0.5, 0, 0.5, {0, 0, 0, alpha});
					break;
				case 1:
					DrawRectangle(0.5, 1, 0, 0.5, {0, 0, 0, alpha});
					break;
				case 2:
					DrawRectangle(0, 0.5, 0.5, 1, {0, 0, 0, alpha});
					break;
				case 3:
					DrawRectangle(0.5, 1, 0.5, 1, {0, 0, 0, alpha});
					break;
			}
		}
		else if (IsInHalvedSplitScreen()) {
			DrawRectangle(0, 1, 0 + (player*0.5), 0.5 + (player*0.5), {0, 0, 0, alpha});
		}
		else {
			DrawRectangle(0, 1, 0, 1, {0, 0, 0, alpha});
		}
	}
}

void ProcessCarReset() {
	static CNyaRaceTimer gTimer;
	gTimer.Process();

	for (int i = 0; i < 4; i++) {
		auto ply = GetPlayer(i);
		if (!ply) continue;
		if (ply->nPlayerType != PLAYERTYPE_LOCAL) continue;
		ProcessCarReset(i, gTimer.fDeltaTime);
	}
}

int __thiscall ResetCarNew(Player* pPlayer, int a2) {
	if (pGameFlow->nEventType == eEventType::STUNT) return 1;
	if (pPlayerHost->nRaceTime < 3000) return 1;
	auto score = GetPlayerScore<PlayerScoreRace>(1);
	if (score->bIsDNF || score->bHasFinished) return 1;
	bCarResetRequested[pPlayer->nPlayerId-1] = true;
	return 1;
}

void ApplyCarResetPatches() {
	NyaHookLib::Patch(0x6605CC, &ResetCarNew);
	NyaHookLib::Patch<uint8_t>(0x44872F, 0xEB); // allow resetting immediately upon ragdoll
}