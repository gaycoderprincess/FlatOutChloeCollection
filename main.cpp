#include <windows.h>
#include <fstream>
#include <thread>
#include <d3dx9.h>
#include "toml++/toml.hpp"
#include "nya_dx9_hookbase.h"
#include "nya_commonhooklib.h"
#include "nya_commonmath.h"

#include "fo1.h"
#include "../nya-common-fouc/fo2versioncheck.h"
#include "../FlatOutTimeTrialGhosts/include/chloetimetrial.h"
#include "chloemenulib.h"
#include "include/chloenet.h"

#include "consts.h"
#include "utils.h"
#include "mallochook.h"
#include "events.h"
#include "filereader.h"
#include "config.h"
#include "customsave.h"
#include "cardealer.h"
#include "achievements.h"
#include "customsettings.h"
#include "quickrace.h"
#include "cardamage.h"
#include "carlimitadjuster.h"
#include "reversetracks.h"
#include "carreset.h"
#include "arcademode.h"
#include "careermode.h"
#include "cardatabase.h"
#include "aiextender.h"
#include "ddsparser.h"
#include "hud/common.h"
#include "hud/ingame.h"
#include "hud/menu.h"
#include "musicplayer.h"
#include "bfsload.h"
#include "xinputsupport.h"
#include "d3dhook.h"
#include "windowedmode.h"
#include "profiles.h"
#include "carnagerace.h"
#include "smashyrace.h"
#include "luafunctions.h"
#include "nitrogain.h"
#include "ultrawide.h"
#include "debugmenu.h"

void SetHandlingDamage() {
	int handlingDamage = nHandlingDamage;
	if (CareerMode::IsCareerTimeTrial()) handlingDamage = HANDLINGDAMAGE_REDUCED;
	if (QuickRace::bIsQuickRace && QuickRace::fDamageLevel == 0.0) handlingDamage = HANDLINGDAMAGE_OFF;
	if (ArcadeMode::bIsArcadeMode) handlingDamage = HANDLINGDAMAGE_OFF;
	if (bIsInMultiplayer) {
		handlingDamage = HANDLINGDAMAGE_REDUCED;
		if (fMultiplayerDamageLevel == 0.0) handlingDamage = HANDLINGDAMAGE_OFF;
	}

	if (handlingDamage == HANDLINGDAMAGE_ON) return;
	if (pLoadingScreen) return;
	if (GetGameState() != GAME_STATE_RACE) return;

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (!ply) continue;

		ply->pCar->FixPart(eCarFixPart::SUSPENSION);
		if (handlingDamage == HANDLINGDAMAGE_OFF) {
			ply->pCar->FixPart(eCarFixPart::WHEELS);
		}
	}
}

void SetHandlingMode() {
	int handlingMode = nHandlingMode;
	if (CareerMode::IsCareerTimeTrial()) handlingMode = HANDLING_NORMAL;
	if (bIsInMultiplayer) handlingMode = nMultiplayerHandlingMode;

	static int nLast = -1;
	if (nLast != handlingMode) {
		NyaHookLib::Patch<uint64_t>(0x418B39, handlingMode == HANDLING_HARDCORE ? 0x86D9900000038DE9 : 0x86D90000038C840F);
		NyaHookLib::Patch<uint64_t>(0x419211, handlingMode == HANDLING_HARDCORE ? 0x44D990000003D7E9 : 0x44D9000003D6850F);
		nLast = handlingMode;
	}
	pGameFlow->Profile.nEasyDifficulty = handlingMode == HANDLING_NORMAL;
}

void CustomSetterThread() {
	pGameFlow->nAutoTransmission = !nTransmission;
	nRagdoll = 1;

	SetHandlingDamage();
	SetHandlingMode();
	ProcessPlayStats();
	ChloeEvents::FinishFrameEvent.OnHit();

	// rng buffer overrun hack-fix
	if (RNGGenerator::nNumValuesLeft <= 1) {
		RNGGenerator::Reset();
	}
}

void InitD3D() {
	// d3d hooks done later so the custommp chat ui has priority
	NyaFO2Hooks::PlaceD3DHooks();
	NyaFO2Hooks::aEndSceneFuncs.push_back(CustomSetterThread);
	NyaFO2Hooks::aEndSceneFuncs.push_back(D3DHookMain);
	NyaFO2Hooks::aD3DResetFuncs.push_back(OnD3DReset);
}

void OnFilesystemInit() {
	ChloeEvents::FilesystemInitEvent.OnHit();
}

void CommandlineArgReader(void* a1, const char* a2) {
	auto str = (std::string)a2;
	if (str == "-debug") bDebugLog = true;
	if (str == "-nopreload") bNoPreload = true;
	if (str == "-asyncpreload") bAsyncPreload = true;
	WriteLogDebug("INIT", std::format("Commandline argument {}", a2));

	return lua_setglobal(a1, a2);
}

uintptr_t OnFilesystemInitASM_jmp = 0x4398C0;
void __attribute__((naked)) OnFilesystemInitASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (OnFilesystemInitASM_jmp), "i" (OnFilesystemInit)
	);
}

auto UpdateCameraHooked_call = (void(__thiscall*)(void*, float))0x47F070;
void __fastcall UpdateCameraHooked(void* a1, void*, float a2) {
	if (nHighCarCam) {
		auto bak = pCameraManager->pPlayer->pCar->mMatrix[13];
		pCameraManager->pPlayer->pCar->mMatrix[13] += 0.15;
		UpdateCameraHooked_call(a1, a2);
		pCameraManager->pPlayer->pCar->mMatrix[13] = bak;
	}
	else {
		UpdateCameraHooked_call(a1, a2);
	}
}

void __stdcall D3DGameUI(int) {
	bIsDrawingGameUI = true;
	D3DHookMain();
}

const char* __cdecl OnMapLoad(void* a1, int a2) {
	ChloeEvents::MapLoadedEvent.OnHit();

	auto path = (const char*)lua_tolstring(a1, a2);
	HUD_Minimap.pMapTexture = CHUDElement::LoadTextureFromBFS(path);
	return path;
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			DoFlatOutVersionCheck(FO2Version::FO1_1_1);

			srand(time(0));

			NyaFO2Hooks::PlaceWndProcHook();
			NyaFO2Hooks::aWndProcFuncs.push_back(WndProcHook);

			//HookMalloc();
			ApplyBFSLoadingPatches();
			ApplyWindowedModePatches();
			ApplyLUAPatches();
			ApplyDDSParserPatches();
			ApplyDebugMenuPatches();
			ApplyCustomSettingsPatches();
			ApplyCarLimitAdjusterPatches();
			ApplyCarDamagePatches();
			ApplyCarResetPatches();
			ApplyCarDatabasePatches();
			ApplyNitroGainPatches();
			ApplyUltrawidePatches();
			ApplyXInputPatches();
			ApplyAIExtenderPatches();
			CareerMode::Init();
			ArcadeMode::Init();
			CarnageRace::Init();
			SmashyRace::Init();
			Achievements::Init();
			ChloeEvents::FilesystemInitEvent.AddHandler(NewMusicPlayer::Init);
			ChloeEvents::FilesystemInitEvent.AddHandler(ApplyCarDealerPatches);
			ChloeEvents::FilesystemInitEvent.AddHandler(InitD3D);

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x45315E, &D3DGameUI);
			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4694E5, &OnMapLoad);

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4A74CA, 0x4A757F); // remove copyright screen
			NyaHookLib::Patch<uint8_t>(0x4A6E8F, 0xEB); // remove intro videos

			NyaHookLib::Patch(0x68BDE0, "gamesave"); // change savefile name

			// 004E3CDD disable menu ui

			// use multithreaded flag for preloading textures
			NyaHookLib::Patch(0x505EDE + 1, 0x20 | D3DCREATE_MULTITHREADED);
			NyaHookLib::Patch(0x505EF9 + 1, 0x40 | D3DCREATE_MULTITHREADED);
			NyaHookLib::Patch(0x505F00 + 1, 0x50 | D3DCREATE_MULTITHREADED);

			// swap restart and exit to menu in finish screen
			NyaHookLib::Patch(0x45A62D + 2, 0x194C);
			NyaHookLib::Patch(0x45A641 + 2, 0x194C);
			NyaHookLib::Patch(0x45A667 + 2, 0x18FC);
			NyaHookLib::Patch<uint8_t>(0x45A70C, 0x75);
			NyaHookLib::Patch<uint8_t>(0x45A70F, 0x74);

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4DA47F, 0x4DA5AB); // disable vanilla cheats system

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4D9A93, &CommandlineArgReader);
			OnFilesystemInitASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A7261, &OnFilesystemInitASM);

			UpdateCameraHooked_call = (void(__thiscall*)(void*, float))(*(uintptr_t*)0x662978);
			NyaHookLib::Patch(0x662978, &UpdateCameraHooked);

			for (int i = 0; i < nMaxSplitscreenPlayers; i++) {
				HUD_DamageMeter[i].nPlayerId = i;
			}
		} break;
		default:
			break;
	}
	return TRUE;
}