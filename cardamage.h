float GetCarDamage(Car* pCar) {
	if (pGameFlow->nEventType == eEventType::DERBY) return pCar->GetDerbyDamage();
	return pCar->fDamage;
}

std::string sWreckedNotif;
double fWreckedNotifTimer = 0;

float fWreckedNotifY = 0.1;
float fWreckedNotifSize = 0.05;

void DrawWreckedNotif() {
	tNyaStringData data;
	data.x = 0.5;
	data.y = fWreckedNotifY;
	data.size = fWreckedNotifSize;
	data.XCenterAlign = true;
	int a = 255;
	if (fWreckedNotifTimer >= 2.5) {
		a = (3 - fWreckedNotifTimer) * 2 * 255;
	}
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

void AddWreckedNotifSelf() {
	sWreckedNotif = "YOU ARE WRECKED!";
	fWreckedNotifTimer = 3;
}

void AddTimeoutNotif(std::string player) {
	sWreckedNotif = std::format("{}\nRAN OUT OF TIME", player);
	fWreckedNotifTimer = 3;
}

void AddTimeoutNotifSelf() {
	sWreckedNotif = "OUT OF TIME!";
	fWreckedNotifTimer = 3;
}

// vanilla game uses 50.0, higher is less damage
float fDamageMultiplier = 50.0;
float fDerbyContactTimer[32];
float fDerbyMaxContactTimer = 40;

void ProcessDerbyContactTimer() {
	static CNyaRaceTimer gTimer;
	gTimer.Process();

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (GetCarDamage(ply->pCar) >= 1.0) continue;
		auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
		if (score->bIsDNF) continue;

		for (auto& car : ply->pCar->aCarCollisions) {
			if (car.lastHitTimestamp > pPlayerHost->nRaceTime - 50) {
				fDerbyContactTimer[i] = 0;
				//car.damage = 0;
			}
		}

		fDerbyContactTimer[i] += gTimer.fDeltaTime;
		if (fDerbyContactTimer[i] >= fDerbyMaxContactTimer) {
			score->bIsDNF = true;
			score->nFinishTime = pPlayerHost->nRaceTime;

			if (ply->nPlayerType == PLAYERTYPE_LOCAL) {
				AddTimeoutNotifSelf();
			}
			else {
				AddTimeoutNotif(GetStringNarrow(ply->sPlayerName.Get()));
			}
		}
	}
}

bool IsPlayerWrecked(Player* ply) {
	if (GetCarDamage(ply->pCar) < 1.0) return false;
	auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
	if (score->bHasFinished) return ply->pCar->nIsRagdolled;
	return score->bIsDNF;
}

void AwardWreck(int playerId) {
	const int wreckAwardTimeout = 1000;

	int32_t lastHitTimestamp = -1;
	Player* lastHitPlayer = nullptr;

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		if (i == playerId) continue;

		auto ply = GetPlayer(i);
		if (IsPlayerWrecked(ply)) continue;
		if (ply->pCar->aCarCollisions[playerId].lastHitTimestamp > lastHitTimestamp) {
			lastHitTimestamp = ply->pCar->aCarCollisions[playerId].lastHitTimestamp;
			lastHitPlayer = ply;
		}
	}

	if (lastHitPlayer && lastHitTimestamp > pPlayerHost->nRaceTime - wreckAwardTimeout) {
		if (lastHitPlayer->nPlayerType == PLAYERTYPE_LOCAL) {
			if (pGameFlow->nEventType == eEventType::RACE) Achievements::AwardAchievement(GetAchievement("WRECK_CAR_RACE"));
		}
	}
}

void ProcessCarDamage() {
	NyaHookLib::Patch<uint8_t>(0x4167E8, pGameFlow->nEventType == eEventType::DERBY ? 0xEB : 0x75); // disable auto-ragdolling in derby

	if (GetGameState() != GAME_STATE_RACE) return;
	if (pLoadingScreen) return;
	if (pGameFlow->nEventType == eEventType::STUNT) return;

	static CNyaRaceTimer gTimer;
	gTimer.Process();
	if (fWreckedNotifTimer > 0) {
		fWreckedNotifTimer -= gTimer.fDeltaTime;
	}

	if (pPlayerHost->nRaceTime <= 0) {
		fWreckedNotifTimer = 0;
		memset(fDerbyContactTimer,0,sizeof(fDerbyContactTimer));
	}

	if (fWreckedNotifTimer > 0 && !GetScoreManager()->nHideRaceHUD) {
		DrawWreckedNotif();
	}

	if (pGameFlow->nEventType == eEventType::DERBY) {
		ProcessDerbyContactTimer();
	}

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (GetCarDamage(ply->pCar) < 1.0) continue;

		if (pGameFlow->nEventType == eEventType::DERBY) {
			if (!ply->pCar->nIsRagdolled) {
				AwardWreck(i);

				Car::LaunchRagdoll(ply->pCar, ply->pCar->fRagdollVelocity);

				if (ply->nPlayerType == PLAYERTYPE_LOCAL) {
					AddWreckedNotifSelf();
				}
				else {
					AddWreckedNotif(GetStringNarrow(ply->sPlayerName.Get()));
				}
			}
		}
		else {
			if (!IsPlayerWrecked(ply)) {
				AwardWreck(i);

				Car::LaunchRagdoll(ply->pCar, ply->pCar->fRagdollVelocity);

				auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
				//score->bHasFinished = true;
				if (!score->bHasFinished) score->bIsDNF = true;

				if (ply->nPlayerType == PLAYERTYPE_LOCAL) {
					AddWreckedNotifSelf();
				}
				else {
					AddWreckedNotif(GetStringNarrow(ply->sPlayerName.Get()));
				}
			}
		}
	}
}

void __stdcall CarDamageResetNew(Car* pCar, float* pos, float* matrix) {
	if (GetCarDamage(pCar) >= 1.0) return;
	Car::Reset(pCar, pos, matrix);
}

float GetCarDamageNew() {
	return GetCarDamage(GetPlayer(0)->pCar);
}

void OnCarDamage(Car* pCar) {
	fDamageMultiplier = pGameFlow->nEventType == eEventType::DERBY ? 40.0 : 90.0;
	if (CareerMode::bIsCareerRace && pCar->pPlayer->nPlayerType == PLAYERTYPE_LOCAL) {
		float mult = 1;
		for (int i = 0; i < pGameFlow->Profile.nNumCarUpgrades; i++) {
			auto upgrade = pGameFlow->Profile.aCarUpgrades[i];
			if (upgrade == PlayerProfile::BODYUPGRADE1) mult += 0.30;
			if (upgrade == PlayerProfile::BODYUPGRADE2) mult += 0.35;
			if (upgrade == PlayerProfile::BODYUPGRADE3) mult += 0.35;
		}
		fDamageMultiplier *= mult;
	}
}

uintptr_t OnCarDamageASM_jmp = 0x4161BF;
void __attribute__((naked)) OnCarDamageASM() {
	__asm__ (
		"fld dword ptr [edi+0x29C]\n\t"

		"pushad\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"

		"jmp %0\n\t"
			:
			:  "m" (OnCarDamageASM_jmp), "i" (OnCarDamage)
	);
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

	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x451165, &GetCarDamageNew);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x416052, 0x4160E9); // disable vanilla derby wrecking
	NyaHookLib::Patch(0x4161BF + 2, &fDamageMultiplier);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4161B9, &OnCarDamageASM);

	NyaHookLib::Patch<uint8_t>(0x452B7F, 0xEB); // remove stupid slowmo feature when ragdolled
}