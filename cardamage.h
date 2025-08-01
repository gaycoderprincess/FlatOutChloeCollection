std::string sWreckedNotif;
double fWreckedNotifTimer = 0;

float fWreckedNotifY = 0.15;
float fWreckedNotifSize = 0.05;

void DrawWreckedNotif() {
	tNyaStringData data;
	data.x = 0.5;
	data.y = fWreckedNotifY;
	data.size = fWreckedNotifSize;
	data.XCenterAlign = true;
	int a = 255;
	if (fWreckedNotifTimer <= 0.5) {
		a = fWreckedNotifTimer * 2 * 255;
	}
	data.SetColor(255,255,255,a);
	DrawStringFO2_Ingame12(data, sWreckedNotif);
}

void AddWreckedNotif(std::string player) {
	sWreckedNotif = std::format("{}\nIS WRECKED", player);
	fWreckedNotifTimer = 3;
}

void ProcessCarDamage() {
	static CNyaTimer gTimer;
	gTimer.Process();
	if (fWreckedNotifTimer > 0) {
		fWreckedNotifTimer -= gTimer.fDeltaTime;
	}

	if (GetGameState() != GAME_STATE_RACE) return;
	if (pLoadingScreen) return;
	if (pGameFlow->nEventType != eEventType::RACE) return;

	auto localPlayer = GetPlayerScore<PlayerScoreRace>(1);
	if (localPlayer->bHasFinished || localPlayer->bIsDNF) return;

	if (pPlayerHost->nRaceTime <= 0) fWreckedNotifTimer = 0;

	if (fWreckedNotifTimer > 0) {
		DrawWreckedNotif();
	}

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (ply->pCar->fDamage < 1.0) continue;

		auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
		if (!score->bIsDNF) {
			Car::LaunchRagdoll(ply->pCar, ply->pCar->fRagdollVelocity);
			//score->bHasFinished = true;
			score->bIsDNF = true;

			AddWreckedNotif(GetStringNarrow(ply->sPlayerName.Get()));
		}
	}
}

void __stdcall CarDamageResetNew(Car* pCar, float* pos, float* matrix) {
	if (pCar->fDamage >= 1.0) return;
	Car::Reset(pCar, pos, matrix);
}

void ApplyCarDamagePatches() {
	uintptr_t addresses[] = {
		0x4078F0,
		0x40792D,
		0x449893,
		0x4498D2,
		0x44B762,
		0x44B8E8,
		0x4920EF,
	};
	for (auto& addr : addresses) {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, addr, &CarDamageResetNew);
	}
}