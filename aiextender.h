const int nMaxPlayers = 32;
int nNumAIProfiles = 7;

int nOpponentCount = 7;
int __cdecl GetCarCount(int, int) {
	if (SmashyRace::bIsSmashyRace) return 1;
	return nOpponentCount + 1;
}

const wchar_t* aCustomPlayerNames[nMaxPlayers] = {
		L"FRANK BENTON",
		L"SUE O'NEILL",
		L"TANIA GRAHAM",
		L"KATIE DAGGERT",
		L"RAY SMITH",
		L"PAUL MCGUIRE",
		L"SETH BELLINGER",
};

void GetAINames() {
	auto config = toml::parse_file("Config/AIConfig.toml");
	nNumAIProfiles = config["main"]["NumAIProfiles"].value_or(7);
	for (int i = 0; i < nMaxPlayers; i++) {
		auto str = (std::string) config["main"]["AI" + std::to_string(i + 1)].value_or("NULL");
		if (str == "NULL" && aCustomPlayerNames[i]) continue;

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		auto retStr = converter.from_bytes(str);
		auto wchar = new wchar_t[retStr.length() + 1];
		wcscpy_s(wchar, retStr.length() + 1, retStr.c_str());
		aCustomPlayerNames[i] = wchar;
	}
}

int __fastcall GetPlayerStartPosition(Player* pPlayer) {
	static int startPositions[nMaxPlayers];

	if (pPlayer->nPlayerId == 1) {
		int numPlayers = 8;
		auto startPositionTaken = new bool[numPlayers];
		memset(startPositionTaken, 0, numPlayers);
		for (int i = 0; i < nMaxPlayers; i++) {
			startPositions[i] = i;
			if (i < 8) {
				do {
					startPositions[i] = rand() % numPlayers;
				} while (startPositionTaken[startPositions[i]]);
				startPositionTaken[startPositions[i]] = true;
			}
		}
		delete[] startPositionTaken;
	}

	return startPositions[pPlayer->nPlayerId-1]+1;
}

uintptr_t PlayerStartPositionASM_jmp = 0x43F596;
int __attribute__((naked)) PlayerStartPositionASM() {
	__asm__ (
		"push eax\n\t"
		//"push ecx\n\t"
		"push edx\n\t"
		"push ebx\n\t"
		"push ebp\n\t"
		"push esi\n\t"
		"push edi\n\t"
		"mov ecx, ebx\n\t"
		"call %1\n\t"
		"mov ecx, eax\n\t"
		"pop edi\n\t"
		"pop esi\n\t"
		"pop ebp\n\t"
		"pop ebx\n\t"
		"pop edx\n\t"
		//"pop ecx\n\t"
		"pop eax\n\t"
		"jmp %0\n\t"
			:
			:  "m" (PlayerStartPositionASM_jmp), "i" (GetPlayerStartPosition)
	);
}

uintptr_t AIPlayerStartPositionASM_jmp = 0x43F840;
int __attribute__((naked)) AIPlayerStartPositionASM() {
	__asm__ (
		//"push eax\n\t"
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
		//"pop eax\n\t"
		"jmp %0\n\t"
			:
			:  "m" (AIPlayerStartPositionASM_jmp), "i" (GetPlayerStartPosition)
	);
}

void __stdcall OnLoadAIProfile(AIPlayer* player) {
	while (player->nAIId > nNumAIProfiles) {
		player->nAIId -= nNumAIProfiles;
	}
	AIPlayer::LoadProfile(player);
}

void ApplyAIExtenderPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x406482, &OnLoadAIProfile);

	NyaHookLib::Patch(0x43F74E, &aCustomPlayerNames[-1]);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43F05C, &GetCarCount);
	GetAINames();

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43F403, 0x43F487); // don't init start positions
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43F58E, &PlayerStartPositionASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43F838, &AIPlayerStartPositionASM);

	{
		int listnodeCount = nMaxPlayers * 4;
		int listnodeInitCount = listnodeCount - 1;
		int listnodeSize = (listnodeCount * 0xC) + 0x4;
		int listnodeLastOffset = listnodeSize - 0xC;
		NyaHookLib::Patch(0x62F5C0 + 1, listnodeSize);
		NyaHookLib::Patch(0x62F5E0 + 1, listnodeInitCount);
		NyaHookLib::Patch(0x62F5FB + 2, listnodeLastOffset);
	}

	NyaHookLib::Patch(0x4BEB97 + 1, "failed to allocate listnodes! (0x4BEB97)");
	NyaHookLib::Patch(0x4BF260 + 1, "failed to allocate listnodes! (0x4BF260)");
	NyaHookLib::Patch(0x4C1D9C + 1, "failed to allocate listnodes! (0x4C1D9C)");
	NyaHookLib::Patch(0x4C698A + 1, "failed to allocate listnodes! (0x4C698A)");
	NyaHookLib::Patch(0x4CE632 + 1, "failed to allocate listnodes! (0x4CE632)");
	NyaHookLib::Patch(0x504D00 + 1, "failed to allocate listnodes! (0x504D00)");
	NyaHookLib::Patch(0x507DE9 + 1, "failed to allocate listnodes! (0x507DE9)");
	NyaHookLib::Patch(0x507EAD + 1, "failed to allocate listnodes! (0x507EAD)");
	NyaHookLib::Patch(0x50AFA1 + 1, "failed to allocate listnodes! (0x50AFA1)");
	NyaHookLib::Patch(0x50B0CE + 1, "failed to allocate listnodes! (0x50B0CE)");
	NyaHookLib::Patch(0x50B477 + 1, "failed to allocate listnodes! (0x50B477)");
	NyaHookLib::Patch(0x50B6BA + 1, "failed to allocate listnodes! (0x50B6BA)");
	NyaHookLib::Patch(0x50B914 + 1, "failed to allocate listnodes! (0x50B914)");
	NyaHookLib::Patch(0x50BAD8 + 1, "failed to allocate listnodes! (0x50BAD8)");
	NyaHookLib::Patch(0x50BCAB + 1, "failed to allocate listnodes! (0x50BCAB)");

	NyaHookLib::Patch(0x5049C0 + 1, 32 * 8);
	NyaHookLib::Patch(0x695188, 32 * 8);
	NyaHookLib::Patch(0x4AD3AA + 1, 4096 * 4); // push
	NyaHookLib::Patch(0x4AD3EC + 1, 4096 * 4); // add
	NyaHookLib::Patch(0x4AD3EC + 1, 4096 * 4); // push
	NyaHookLib::Patch(0x4AD415 + 1, 4096 * 4); // add
	NyaHookLib::Patch(0x4AD420 + 1, 4096 * 4); // push
	NyaHookLib::Patch(0x4AD449 + 1, 4096 * 4); // add
	NyaHookLib::Patch(0x4AD454 + 1, 4096 * 4); // push
	NyaHookLib::Patch(0x4AD480 + 1, 4096 * 4); // add
	//NyaHookLib::Patch(0x565B22 + 1, 24 * 10);
	//NyaHookLib::Patch(0x49180A + 1, 2560 * 4);
	//NyaHookLib::Patch(0x491826 + 1, (2560 * 4) / 4);

	// todo sub_44BC20
}