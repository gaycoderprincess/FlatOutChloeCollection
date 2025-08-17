float GetCarDamage(Car* pCar) {
	if (pGameFlow->nEventType == eEventType::DERBY) return pCar->GetDerbyDamage();
	return pCar->fDamage;
}

enum eCrashBonus {
	CRASHBONUS_CRASHFLYBY,
	CRASHBONUS_SUPERFLIP,
	CRASHBONUS_SLAM,
	CRASHBONUS_POWERHIT,
	CRASHBONUS_BLASTOUT,
	CRASHBONUS_RAGDOLLED,
	CRASHBONUS_WRECKED,
	NUM_CRASHBONUS_TYPES
};
int aCrashBonusesReceived[32][NUM_CRASHBONUS_TYPES] = {};

void AddWreckedNotif(const std::string& player);
void AddWreckedNotifSelf();
void AddTimeoutNotif(const std::string& player);
void AddTimeoutNotifSelf();
void AddCrashBonus(int playerId, int type);

// vanilla game uses 50.0, higher is less damage
float fDamageMultiplier = 50.0;
float fDerbyContactTimer[32];
float fDerbyMaxContactTimer = 40;

float fWhammoCrashVelocity1 = 10.0;
float fPowerHitCrashVelocity1 = 20.0;
float fBlastOutCrashVelocity1 = 35.0;
int nRagdollPiggybagThreshold = 1000;
int nWreckPiggybagThreshold = 1000;
float fCrashVelocityMultiplier = 150;

bool IsPlayerWrecked(Player* ply) {
	if (pGameFlow->nEventType == eEventType::DERBY) return ply->pCar->nIsRagdolled;
	if (GetCarDamage(ply->pCar) < 1.0) return false;
	auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
	if (score->bHasFinished) return ply->pCar->nIsRagdolled;
	return score->bIsDNF;
}

void ProcessDerbyContactTimer() {
	static CNyaRaceTimer gTimer;
	gTimer.Process();

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (IsPlayerWrecked(ply)) continue;

		for (auto& car : ply->pCar->aCarCollisions) {
			if (car.lastHitTimestamp > pPlayerHost->nRaceTime - 50) {
				fDerbyContactTimer[i] = 0;
			}
		}

		fDerbyContactTimer[i] += gTimer.fDeltaTime;
		if (fDerbyContactTimer[i] >= fDerbyMaxContactTimer) {
			auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
			score->nFinishTime = pPlayerHost->nRaceTime;
			// ragdoll ai players out if they run out of time
			if (ply->nPlayerType == PLAYERTYPE_AI) {
				if (!ply->pCar->nIsRagdolled) Car::LaunchRagdoll(ply->pCar, ply->pCar->fRagdollVelocity);
			}
			else {
				score->bIsDNF = true;
			}

			if (ply->nPlayerType == PLAYERTYPE_LOCAL) {
				AddTimeoutNotifSelf();
			}
			else {
				AddTimeoutNotif(GetStringNarrow(ply->sPlayerName.Get()));
			}
		}
	}
}

Player* GetPlayerLastHit(int playerId) {
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
	return lastHitPlayer;
}

template<int playerId>
void ProcessCrashBonuses() {
	static int32_t lastHitTimestamps[32] = {};
	static bool isRagdolled[32] = {};

	auto ply = GetPlayer(playerId);
	if (!ply) return;
	if (IsPlayerWrecked(ply)) return;

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		if (i == playerId) continue;
		auto opponent = GetPlayer(i);
		if (!opponent) continue;
		if (IsPlayerWrecked(opponent)) continue;

		auto& data = ply->pCar->aCarCollisions[i];
		if (data.lastHitTimestamp > lastHitTimestamps[i]) {
			//auto diff = opponent->pCar->GetVelocity()->Dot(*ply->pCar->GetVelocity());
			//auto diff = ply->pCar->GetVelocity()->length() - opponent->pCar->GetVelocity()->length();
			auto diff = data.damage;
			diff *= fCrashVelocityMultiplier;
			if (diff > fBlastOutCrashVelocity1) {
				AddCrashBonus(playerId, CRASHBONUS_BLASTOUT);
			}
			else if (diff > fPowerHitCrashVelocity1) {
				AddCrashBonus(playerId, CRASHBONUS_POWERHIT);
			}
			else if (diff > fWhammoCrashVelocity1) {
				AddCrashBonus(playerId, CRASHBONUS_SLAM);
			}
			if (pGameFlow->nEventType != eEventType::RACE) {
				data.damage = 0;
			}
		}
		if (opponent->pCar->nIsRagdolled != isRagdolled[i] && pGameFlow->nEventType != eEventType::DERBY) {
			if (GetPlayerLastHit(i) == ply && data.lastHitTimestamp > pPlayerHost->nRaceTime - nRagdollPiggybagThreshold) {
				AddCrashBonus(playerId, CRASHBONUS_RAGDOLLED);
			}
		}
		lastHitTimestamps[i] = data.lastHitTimestamp;
		isRagdolled[i] = opponent->pCar->nIsRagdolled;
	}
}

void ProcessCrashBonuses() {
	ProcessCrashBonuses<0>();
	ProcessCrashBonuses<1>();
	ProcessCrashBonuses<2>();
	ProcessCrashBonuses<3>();
	ProcessCrashBonuses<4>();
	ProcessCrashBonuses<5>();
	ProcessCrashBonuses<6>();
	ProcessCrashBonuses<7>();
}

void __fastcall OnCarDamageRewards(Player* pPlayer) {
	ProcessCrashBonuses();
}

void AwardWreck(int playerId) {
	Player* lastHitPlayer = GetPlayerLastHit(playerId);
	if (!lastHitPlayer) return;

	auto lastHitTimestamp = lastHitPlayer->pCar->aCarCollisions[playerId].lastHitTimestamp;
	if (lastHitTimestamp > pPlayerHost->nRaceTime - nWreckPiggybagThreshold) {
		AddCrashBonus(lastHitPlayer->nPlayerId-1, CRASHBONUS_WRECKED);
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

	if (pPlayerHost->nRaceTime <= 0) {
		memset(fDerbyContactTimer,0,sizeof(fDerbyContactTimer));
		memset(aCrashBonusesReceived,0,sizeof(aCrashBonusesReceived));
	}

	if (pGameFlow->nEventType == eEventType::DERBY) {
		ProcessDerbyContactTimer();
		ProcessCrashBonuses();
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

				if (!ply->pCar->nIsRagdolled) Car::LaunchRagdoll(ply->pCar, ply->pCar->fRagdollVelocity);

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

float fCarDurability[32] = {};
void OnCarDamage(Car* pCar) {
	fDamageMultiplier = pGameFlow->nEventType == eEventType::DERBY ? 40.0 : 90.0;
	fDamageMultiplier *= 1 + fCarDurability[pCar->pPlayer->nPlayerId-1];
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

uintptr_t OnCarDamageRewardsASM_jmp = 0x449087;
void __attribute__((naked)) OnCarDamageRewardsASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"

		"cmp [esi+0x1F00], ebx\n\t"
		"jmp %0\n\t"
			:
			:  "m" (OnCarDamageRewardsASM_jmp), "i" (OnCarDamageRewards)
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
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x449081, &OnCarDamageRewardsASM);

	NyaHookLib::Patch<uint8_t>(0x452B7F, 0xEB); // remove stupid slowmo feature when ragdolled
}