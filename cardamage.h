float GetCarDamage(Car* pCar) {
	if (pGameFlow->nEventType == eEventType::DERBY) return pCar->GetDerbyDamage();
	return pCar->fDamage;
}

float fDerbyContactTimer[32];
GameRules::KeyValue DerbyMaxContactTimer("ContactTimer");

GameRules::KeyValue GlobalDamageScale("GlobalDamageScale");
GameRules::KeyValue CarDamageMultiplier("CarDamageMultiplier");
GameRules::KeyValue ObjectDamageMultiplier("ObjectDamageMultiplier");
GameRules::KeyValue WorldDamageMultiplier("WorldDamageMultiplier");

bool IsPlayerWrecked(Player* ply) {
	if (pGameFlow->nEventType == eEventType::DERBY) return ply->pCar->nIsRagdolled;
	if (GetCarDamage(ply->pCar) < 1.0) return false;
	auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
	if (score->bHasFinished) return ply->pCar->nIsRagdolled;
	return score->bIsDNF;
}

void ProcessDerbyContactTimer() {
	if (IsInSplitScreen()) return;

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
		if (fDerbyContactTimer[i] >= DerbyMaxContactTimer) {
			auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
			score->nFinishTime = pPlayerHost->nRaceTime;
			// ragdoll ai players out if they run out of time
			if (ply->nPlayerType == PLAYERTYPE_AI) {
				if (!ply->pCar->nIsRagdolled) Car::LaunchRagdoll(ply->pCar, ply->pCar->GetVelocity()->length());
			}
			else {
				score->bIsDNF = true;
			}

			ChloeEvents::DerbyTimeoutEvent.OnHit(ply);
		}
	}
}

Player* GetPlayerLastHit(int playerId) {
	int32_t lastHitTimestamp = -1;
	Player* lastHitPlayer = nullptr;

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		if (i == playerId) continue;

		auto ply = GetPlayer(i);
		if (!bIsFragDerby && IsPlayerWrecked(ply)) continue;
		if (ply->pCar->aCarCollisions[playerId].lastHitTimestamp > lastHitTimestamp) {
			lastHitTimestamp = ply->pCar->aCarCollisions[playerId].lastHitTimestamp;
			lastHitPlayer = ply;
		}
	}
	return lastHitPlayer;
}

template<int playerId>
void ProcessCrashBonuses() {
	static CNyaRaceTimer gTimer;
	gTimer.Process();

	static int32_t lastHitTimestamps[nMaxPlayers] = {};
	static bool isRagdolled[nMaxPlayers] = {};
	static double rotateAmount[nMaxPlayers] = {};

	auto ply = GetPlayer(playerId);
	if (!ply) return;
	if (!bIsFragDerby && IsPlayerWrecked(ply)) return;

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		if (i == playerId) continue;
		auto opponent = GetPlayer(i);
		if (!opponent) continue;
		if (IsPlayerWrecked(opponent)) continue;

		auto& data = ply->pCar->aCarCollisions[i];
		if (data.lastHitTimestamp > lastHitTimestamps[i]) {
			auto diff = data.damage;
			//diff /= CarDamageMultiplier;
			diff *= CrashVelocityMultiplier;
			if (diff > BlastOutCrashVelocity1) {
				ChloeEvents::CrashBonusEvent.OnHit(ply, CRASHBONUS_BLASTOUT);
			}
			else if (diff > PowerHitCrashVelocity1) {
				ChloeEvents::CrashBonusEvent.OnHit(ply, CRASHBONUS_POWERHIT);
			}
			else if (diff > WhammoCrashVelocity1) {
				ChloeEvents::CrashBonusEvent.OnHit(ply, CRASHBONUS_SLAM);
			}
			if (pGameFlow->nEventType != eEventType::RACE && !IsNitroEnabledInDerby()) {
				data.damage = 0;
			}
		}
		if (opponent->pCar->nIsRagdolled && !isRagdolled[i] && pGameFlow->nEventType != eEventType::DERBY) {
			if (GetPlayerLastHit(i) == ply && data.lastHitTimestamp > pPlayerHost->nRaceTime - RagdollPiggybagThreshold) {
				ChloeEvents::CrashBonusEvent.OnHit(ply, CRASHBONUS_RAGDOLLED);
			}
		}

		if (data.lastHitTimestamp < 0 || data.lastHitTimestamp < pPlayerHost->nRaceTime - FlipPiggybagThreshold) {
			rotateAmount[i] = 0;
		}
		else {
			auto carRotation = *opponent->pCar->GetMatrix();
			carRotation.p = {0, 0, 0};
			carRotation = carRotation.Invert();

			auto angVelRelative = carRotation * *opponent->pCar->GetAngVelocity();
			rotateAmount[i] += angVelRelative.z * gTimer.fDeltaTime;
		}

		if (std::abs(rotateAmount[i]) > std::numbers::pi * 0.9) {
			ChloeEvents::CrashBonusEvent.OnHit(ply, CRASHBONUS_SUPERFLIP);
			rotateAmount[i] = 0;
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
	if (lastHitTimestamp > pPlayerHost->nRaceTime - WreckPiggybagThreshold) {
		ChloeEvents::CrashBonusEvent.OnHit(lastHitPlayer, CRASHBONUS_WRECKED);
		if (lastHitPlayer->nPlayerType == PLAYERTYPE_LOCAL) {
			if (pGameFlow->nEventType == eEventType::RACE) Achievements::AwardAchievement(GetAchievement("WRECK_CAR_RACE"));
		}
	}
}

void ProcessCarDamage() {
	NyaHookLib::Patch<uint8_t>(0x4167E8, pGameFlow->nEventType == eEventType::DERBY || bIsInMultiplayer ? 0xEB : 0x75); // disable auto-ragdolling in multiplayer and derby

	if (GetGameState() != GAME_STATE_RACE) return;
	if (pLoadingScreen) return;
	if (pGameFlow->nEventType == eEventType::STUNT) return;

	static CNyaRaceTimer gTimer;
	gTimer.Process();

	if (pPlayerHost->nRaceTime <= 0) {
		memset(fDerbyContactTimer,0,sizeof(fDerbyContactTimer));
		memset(aCrashBonusesReceived,0,sizeof(aCrashBonusesReceived));
		return;
	}

	if (pGameFlow->nEventType == eEventType::DERBY) {
		if (!bIsFragDerby) ProcessDerbyContactTimer();
		if (!IsNitroEnabledInDerby()) ProcessCrashBonuses();
	}

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		bool shouldWreck = GetCarDamage(ply->pCar) >= 1.0;
		if (bIsInMultiplayer && ChloeNet::IsReplicatedPlayer(ply)) shouldWreck = ChloeNet::IsReplicatedPlayerWrecked(ply);
		if (!shouldWreck) continue;

		if (pGameFlow->nEventType == eEventType::DERBY) {
			if (!ply->pCar->nIsRagdolled) {
				AwardWreck(i);

				Car::LaunchRagdoll(ply->pCar, ply->pCar->GetVelocity()->length());

				ChloeEvents::PlayerWreckedEvent.OnHit(ply);
			}
		}
		else {
			if (!IsPlayerWrecked(ply)) {
				AwardWreck(i);

				if (!ply->pCar->nIsRagdolled) Car::LaunchRagdoll(ply->pCar, ply->pCar->GetVelocity()->length());

				auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
				//score->bHasFinished = true;
				if (!score->bHasFinished) score->bIsDNF = true;

				ChloeEvents::PlayerWreckedEvent.OnHit(ply);
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
float fDamageMultiplier = 50.0;

void OnCarDamage(Car* pCar) {
	fDamageMultiplier = GlobalDamageScale;
	fDamageMultiplier *= 1 + fCarDurability[pCar->pPlayer->nPlayerId-1];
	if (bIsInMultiplayer) {
		if (fMultiplayerDamageLevel > 0) {
			fDamageMultiplier /= fMultiplayerDamageLevel;
		}
		else {
			fDamageMultiplier = 999999.0;
		}
	}
	if (bIsQuickRace) {
		if (QuickRace::fDamageLevel > 0) {
			fDamageMultiplier /= QuickRace::fDamageLevel;
		}
		else {
			fDamageMultiplier = 999999.0;
		}
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

bool __fastcall IsCarWrecked_DamageCheck(Car* car) {
	NyaHookLib::Patch<uint8_t>(0x4167D2, car->nIsRagdolled ? 0xEB : 0x75); // disable re-ragdolling
	return IsPlayerWrecked(car->pPlayer);
}

uintptr_t IsPlayerWreckedASM_jmp = 0x4166B0;
void __attribute__((naked)) IsPlayerWreckedASM() {
	__asm__ (
		"push ecx\n\t"
		"push edx\n\t"
		"push ebx\n\t"
		"push ebp\n\t"
		"push esi\n\t"
		"push edi\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"pop edi\n\t"
		"pop esi\n\t"
		"pop ebp\n\t"
		"pop ebx\n\t"
		"pop edx\n\t"
		"pop ecx\n\t"
		"jmp %0\n\t"
			:
			:  "m" (IsPlayerWreckedASM_jmp), "i" (IsCarWrecked_DamageCheck)
	);
}

void __fastcall WorldDamageMult(float* a1) {
	a1[9] *= WorldDamageMultiplier;
}

uintptr_t WorldDamageMultASM_jmp = 0x4B2100;
void __attribute__((naked)) WorldDamageMultASM() {
	__asm__ (
		"mov edx, [esp+0xC4]\n\t"
		"mov [eax+0x34], edx\n\t"
		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (WorldDamageMultASM_jmp), "i" (WorldDamageMult)
	);
}

void __fastcall CarDamageMult(DamageData* a1) {
	if (a1->pEntity) {
		if (a1->pEntity->nEntityType == 4) {
			a1->fImpactForce *= CarDamageMultiplier;
			a1->fImpactForce2 /= CarDamageMultiplier; // fix ejection threshold
		}
		else {
			a1->fImpactForce *= ObjectDamageMultiplier;
		}
	}
	else {
		a1->fImpactForce *= WorldDamageMultiplier;
	}
}

void __attribute__((naked)) CarDamageMultASM() {
	__asm__ (
		"mov [eax+0x34], ecx\n\t"
		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %0\n\t"
		"popad\n\t"
		"ret\n\t"
			:
			:  "i" (CarDamageMult)
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

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4166AA, &IsPlayerWreckedASM);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4B20F6, &WorldDamageMultASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x428DC9, &CarDamageMultASM);

	NyaHookLib::Patch(0x416748 + 2, -100); // minimum crash bonus interval, default -500

	ChloeEvents::FinishFrameEvent.AddHandler(ProcessCarDamage);
}