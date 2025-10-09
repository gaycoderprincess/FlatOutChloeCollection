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
#include "config.h"
#include "utils.h"
#include "draw3d.h"
#include "mallochook.h"
#include "events.h"
#include "filereader.h"
#include "gamerules.h"
#include "customsave.h"
#include "cardealer.h"
#include "achievements.h"
#include "windowedmode.h"
#include "customsettings.h"
#include "quickrace.h"
#include "instantaction.h"
#include "cardamage.h"
#include "carlimitadjuster.h"
#include "trackextender.h"
#include "resetmap.h"
#include "carreset.h"
#include "arcademode.h"
#include "careermode.h"
#include "handlingmode.h"
#include "cardatabase.h"
#include "aiextender.h"
#include "ddsparser.h"
#include "musicplayer.h"
#include "hud/common.h"
#include "hud/menu.h"
#include "hud/ingame.h"
#include "bfsload.h"
#include "xinputsupport.h"
#include "d3dhook.h"
#include "profiles.h"
#include "carnagerace.h"
#include "smashyrace.h"
#include "fragderby.h"
#include "luafunctions.h"
#include "nitrogain.h"
#include "ultrawide.h"
#include "newingamemenu.h"
#include "debugmenu.h"

void SetHandlingDamage() {
	int handlingDamage = nHandlingDamage;
	if (CareerMode::IsCareerTimeTrial()) handlingDamage = HANDLINGDAMAGE_REDUCED;
	if (bIsQuickRace && QuickRace::fDamageLevel == 0.0) handlingDamage = HANDLINGDAMAGE_OFF;
	if (bIsArcadeMode) handlingDamage = HANDLINGDAMAGE_OFF;
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
			auto enginehp = ply->pCar->Performance.Engine.fHealth;
			ply->pCar->FixPart(eCarFixPart::ENGINE);
			ply->pCar->Performance.Engine.fHealth = enginehp;
		}
	}
}

// make engine fire always correspond to player health
void SetEngineDamage() {
	if (pLoadingScreen) return;
	if (GetGameState() != GAME_STATE_RACE) return;
	if (pGameFlow->nEventType != eEventType::RACE) return;

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (!ply) continue;
		ply->pCar->Performance.Engine.fHealth = 1.0 - ply->pCar->fDamage;
	}
}

void CustomSetterThread() {
	pGameFlow->nAutoTransmission = !nTransmission;
	pGameFlow->Profile.nEasyDifficulty = GetHandlingMode(nullptr) == HANDLING_NORMAL;
	nRagdoll = 1;

	for (auto& color : aPlayerColorsMultiplayer) {
		auto& dest = *(NyaDrawing::CNyaRGBA32*)&gPalette[(&color - &aPlayerColorsMultiplayer[0]) + 100];
		dest.r = color.b;
		dest.g = color.g;
		dest.b = color.r;
		dest.a = 255;
	}

	SetHandlingDamage();
	SetHandlingMode();
	SetEngineDamage();
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
	if (str == "-asyncpreload") {
		bAsyncPreload = true;

		// use multithreaded flag for preloading textures
		NyaHookLib::Patch(0x505EDE + 1, 0x20 | D3DCREATE_MULTITHREADED);
		NyaHookLib::Patch(0x505EF9 + 1, 0x40 | D3DCREATE_MULTITHREADED);
		NyaHookLib::Patch(0x505F00 + 1, 0x50 | D3DCREATE_MULTITHREADED);
	}
	if (str == "-notextures") bNoTextures = true;
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
	for (int i = 0; i < (int)eHUDLayer::NUM_LAYERS; i++) {
		nDrawingGameUILayer = i;
		D3DHookMain();
	}
}

auto OnMapLoad_call = (void(__stdcall*)(void*, void*, void*))0x44AD00;
void __stdcall OnMapLoad(void* a1, void* a2, void* a3) {
	ChloeEvents::RacePreLoadEvent.OnHit();
	OnMapLoad_call(a1, a2, a3);
	ChloeEvents::MapLoadEvent.OnHit();
	gCustomSave.Save();
}

auto OnMapPreLoad_call = (void(__stdcall*)(int, int, int, int, int, int, int, float, char))0x4B9250;
void __stdcall OnMapPreLoad(int a1, int a2, int a3, int a4, int a5, int a6, int a7, float a8, char a9) {
	ChloeEvents::MapPreLoadEvent.OnHit();
	return OnMapPreLoad_call(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

void ClearD3D() {
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			DoFlatOutVersionCheck(FO2Version::FO1_1_1);
			if ((std::filesystem::exists("filesystem") || std::filesystem::exists("patch")) && (!std::filesystem::exists("common1.bfs") || !std::filesystem::exists("common2.bfs"))) {
				MessageBoxA(nullptr, "Unsupported game version! Make sure your game is packed!\n\nIf you don't know what this means, you're probably using the GOG version, which is incompatible with this mod.\nGet a version of the game that has common1.bfs and common2.bfs in the game folder.", "nya?!~", MB_ICONERROR);
				exit(0);
			}

			srand(time(0));

			NyaFO2Hooks::PlaceWndProcHook();
			NyaFO2Hooks::aWndProcFuncs.push_back(WndProcHook);
			NyaFO2Hooks::aPostPresentFuncs.push_back(ClearD3D);

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
			ApplyDraw3DPatches();
			ApplyTrackExtenderPatches();
			ApplyHandlingModePatches();
			CareerMode::Init();
			ArcadeMode::Init();
			CarnageRace::Init();
			SmashyRace::Init();
			FragDerby::Init();
			Achievements::Init();
			NewIngameMenu::Init();
			GameRules::Init();
			NewResetMap::Init();
			ChloeEvents::FilesystemInitEvent.AddHandler(NewMusicPlayer::Init);
			ChloeEvents::FilesystemInitEvent.AddHandler(ApplyCarDealerPatches);
			ChloeEvents::FilesystemInitEvent.AddHandler(InitD3D);

			NyaHookLib::Patch<uint64_t>(0x454AFC, 0xE0A190000001EEE9); // remove total time from hud

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x45315E, &D3DGameUI);
			NyaHookLib::Patch<uint16_t>(0x45314F, 0x9090); // enable map drawing in stunt maps
			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x451CE3, &OnMapLoad);

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4A74CA, 0x4A757F); // remove copyright screen
			NyaHookLib::Patch<uint8_t>(0x4A6E8F, 0xEB); // remove intro videos

			NyaHookLib::Patch(0x68BDE0, "gamesave"); // change savefile name

			// 004E3CDD disable menu ui

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

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x468DA9, &OnMapPreLoad); // ingame
			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4655EF, &OnMapPreLoad); // menu

			for (int i = 0; i < nMaxSplitscreenPlayers; i++) {
				HUD_DamageMeter[i].nPlayerId = i;
			}
			
			NyaHookLib::Patch<uint8_t>(0x47C4FD, 0xEB); // disable splitscreen prop draw distance halving

			//NyaHookLib::Fill(0x506499, 0x90, 5); // remove vanilla culling calls
		} break;
		default:
			break;
	}
	return TRUE;
}