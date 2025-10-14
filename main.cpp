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
#include "mallochook.h"
#include "events.h"
#include "draw3d.h"
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
#include "ultrawide.h"
#include "xinputsupport.h"
#include "d3dhook.h"
#include "profiles.h"
#include "carnagerace.h"
#include "smashyrace.h"
#include "fragderby.h"
#include "luafunctions.h"
#include "nitrogain.h"
#include "newingamemenu.h"
#include "debugmenu.h"

void SetGameSettings() {
	pGameFlow->nAutoTransmission = !nTransmission;
	pGameFlow->Profile.nEasyDifficulty = GetHandlingMode(nullptr) == HANDLING_NORMAL;
	nRagdoll = 1;
}

void SetPlayerColors() {
	for (auto& color : aPlayerColorsMultiplayer) {
		auto& dest = *(NyaDrawing::CNyaRGBA32*)&gPalette[(&color - &aPlayerColorsMultiplayer[0]) + 100];
		dest.r = color.b;
		dest.g = color.g;
		dest.b = color.r;
		dest.a = 255;
	}
}

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
	ChloeEvents::FinishFrameEvent.OnHit();

	// rng buffer overrun hack-fix
	if (RNGGenerator::nNumValuesLeft <= 1) {
		RNGGenerator::Reset();
	}
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
			NyaFO2Hooks::aEndSceneFuncs.push_back(CustomSetterThread);

			//HookMalloc();
			ChloeEventHooks::Init();
			ChloeEvents::FinishFrameEvent.AddHandler(SetGameSettings);
			ChloeEvents::FinishFrameEvent.AddHandler(SetPlayerColors);
			ChloeEvents::FinishFrameEvent.AddHandler(SetHandlingDamage);
			ChloeEvents::FinishFrameEvent.AddHandler(SetHandlingMode);
			ChloeEvents::FinishFrameEvent.AddHandler(SetEngineDamage);

			NyaHookLib::Patch<uint64_t>(0x454AFC, 0xE0A190000001EEE9); // remove total time from hud

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4A74CA, 0x4A757F); // remove copyright screen
			NyaHookLib::Patch<uint8_t>(0x4A6E8F, 0xEB); // remove intro videos

			NyaHookLib::Patch(0x68BDE0, "gamesave"); // change savefile name

			// disable car dummy object memory clear at race start, needed for nitro ptfx code
			NyaHookLib::Patch<uint8_t>(0x41E39A, 0xEB);
			NyaHookLib::Fill(0x41E3E0, 0x90, 6);

			// 004E3CDD disable menu ui

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4DA47F, 0x4DA5AB); // disable vanilla cheats system

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4D9A93, &CommandlineArgReader);

			UpdateCameraHooked_call = (void(__thiscall*)(void*, float))(*(uintptr_t*)0x662978);
			NyaHookLib::Patch(0x662978, &UpdateCameraHooked);

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