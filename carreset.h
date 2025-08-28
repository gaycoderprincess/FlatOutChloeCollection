struct tResetpoint {
	NyaMat4x4 matrix;
	int split = -1;
};
std::vector<tResetpoint> aNewResetPoints;

std::string GetResetPointFilename() {
	return (std::string)"Config/Resets/" + GetTrackName(pGameFlow->nLevel) + (".rst2");
}

void SaveResetPoints(const std::string& filename) {
	std::filesystem::create_directory("Config");
	std::filesystem::create_directory("Config/Resets");

	std::ofstream fout(filename, std::ios::out | std::ios::binary );
	if (!fout.is_open()) return;

	uint32_t count = aNewResetPoints.size();
	fout.write((char*)&count, 4);
	for (int i = 0; i < count; i++) {
		fout.write((char*)&aNewResetPoints[i], sizeof(tResetpoint));
	}
}

void LoadResetPoints() {
	aNewResetPoints.clear();

	std::ifstream fin(GetResetPointFilename(), std::ios::in | std::ios::binary );
	if (!fin.is_open()) return;

	uint32_t count = 0;
	fin.read((char*)&count, 4);
	aNewResetPoints.reserve(count);
	for (int i = 0; i < count; i++) {
		if (fin.eof()) return;

		tResetpoint reset;
		fin.read((char*)&reset.matrix, sizeof(reset.matrix));
		fin.read((char*)&reset.split, sizeof(reset.split));
		aNewResetPoints.push_back(reset);
	}
	return;
}

NyaMat4x4* pPlayerResetpoint = nullptr;
NyaMat4x4* GetClosestResetpoint(NyaVec3 pos, int split, float maxDist = 99999) {
	if (aNewResetPoints.empty()) return nullptr;

	if (bIsTrackReversed) split = (pEnvironment->nNumSplitpoints - 1) - split;

	float dist = maxDist;
	NyaMat4x4* out = nullptr;
	for (auto& reset : aNewResetPoints) {
		if (reset.split >= 0 && reset.split != split) continue;

		auto d = (reset.matrix.p - pos).length();
		if (d < dist) {
			out = &reset.matrix;
			dist = d;
		}
	}
	return out;
}

double fCarResetFadeTimer[nMaxSplitscreenPlayers] = {};
bool bCarResetRequested[nMaxSplitscreenPlayers] = {};

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
				if (player == 0 && pPlayerResetpoint) {
					auto reset = *pPlayerResetpoint;
					if (bIsTrackReversed) {
						reset.x.x *= -1; // x.x
						reset.x.y *= -1; // x.y
						reset.x.z *= -1; // x.z
						reset.z.x *= -1; // z.x
						reset.z.y *= -1; // z.y
						reset.z.z *= -1; // z.z
					}
					Car::Reset(ply->pCar, &reset.p.x, &reset.x.x);
				}
				if (pGameFlow->nEventType != eEventType::DERBY) {
					*ply->pCar->GetVelocity() = ply->pCar->GetMatrix()->z * fCarResetSpeed;
				}
			}
			bCarResetRequested[player] = false;
		}
	}

	if (fCarResetFadeTimer[player] < 0) fCarResetFadeTimer[player] = 0;
	if (fCarResetFadeTimer[player] > 1) fCarResetFadeTimer[player] = 1;
}

void ProcessCarReset() {
	static CNyaRaceTimer gTimer;
	gTimer.Process();

	for (int i = 0; i < nMaxSplitscreenPlayers; i++) {
		auto ply = GetPlayer(i);
		if (!ply) continue;
		if (ply->nPlayerType != PLAYERTYPE_LOCAL) continue;
		ProcessCarReset(i, gTimer.fDeltaTime);
	}

	if (GetGameState() == GAME_STATE_MENU) {
		aNewResetPoints.clear();
	}

	if (!pLoadingScreen && GetGameState() == GAME_STATE_RACE) {
		float fResetpointMaxDist = 15;

		auto ply = GetPlayer(0);
		auto reset = GetClosestResetpoint(ply->pCar->GetMatrix()->p, ply->nCurrentSplit % pEnvironment->nNumSplitpoints, fResetpointMaxDist);
		if (reset) pPlayerResetpoint = reset;
	}
	else {
		pPlayerResetpoint = nullptr;
	}
}

int __thiscall ResetCarNew(Player* pPlayer, int a2) {
	if (bIsSmashyRace && !pPlayer->pCar->nIsRagdolled) return 1;
	if (pGameFlow->nEventType == eEventType::STUNT) return 1;
	if (pPlayerHost->nRaceTime < 3000) return 1;
	auto score = GetPlayerScore<PlayerScoreRace>(pPlayer->nPlayerId);
	if (score->bIsDNF || score->bHasFinished) return 1;
	bCarResetRequested[pPlayer->nPlayerId-1] = true;
	return 1;
}

float fMaxSpeedWhenResetAllowed = 10;
uint32_t __fastcall ResetSpeedCheck(Car* car) {
	return car->GetVelocity()->Length() > fMaxSpeedWhenResetAllowed;
}

uintptr_t ResetSpeedCheckASM_jmp = 0x448764;
void __attribute__((naked)) ResetSpeedCheckASM() {
	__asm__ (
		//"push eax\n\t"
		"push ecx\n\t"
		"push edx\n\t"
		"push ebx\n\t"
		"push ebp\n\t"
		"push esi\n\t"
		"push edi\n\t"
		"mov ecx, ebp\n\t"
		"call %1\n\t"
		"pop edi\n\t"
		"pop esi\n\t"
		"pop ebp\n\t"
		"pop ebx\n\t"
		"pop edx\n\t"
		"pop ecx\n\t"
		//"pop eax\n\t"
		"cmp eax, 1\n\t"
		"jmp %0\n\t"
			:
			:  "m" (ResetSpeedCheckASM_jmp), "i" (ResetSpeedCheck)
	);
}

// remove out of track popup if you've passed a split after it appeared
void OutOfTrackChecker() {
	static int nLastCheckpoint[nMaxPlayers] = {};
	if (pLoadingScreen || GetGameState() != GAME_STATE_RACE) {
		memset(nLastCheckpoint, 0, sizeof(nLastCheckpoint));
		return;
	}

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (ply->nIsOutOfTrack && ply->nCurrentSplit > nLastCheckpoint[i]) ply->nIsOutOfTrack = 0;
		nLastCheckpoint[i] = ply->nCurrentSplit;
	}
}

void ApplyCarResetPatches() {
	NyaHookLib::Patch(0x6605CC, &ResetCarNew);

	// allow resetting immediately upon ragdoll
	// nTimeRagdolled > 2000 -> nIsRagdolled > 0
	NyaHookLib::Patch(0x448727, 0x33C);
	NyaHookLib::Patch(0x448727 + 4, 0);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x448731, &ResetSpeedCheckASM);
	NyaHookLib::Patch<uint8_t>(0x448764, 0x74);

	ChloeEvents::MapLoadedEvent.AddHandler(LoadResetPoints);
	ChloeEvents::FinishFrameEvent.AddHandler(ProcessCarReset);
	ChloeEvents::FinishFrameEvent.AddHandler(OutOfTrackChecker);
}