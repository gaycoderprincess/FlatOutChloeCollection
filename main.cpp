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

void WriteLog(const std::string& str) {
	static auto file = std::ofstream("FlatOutChloeCollection_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

bool bDebugLog = false;
void WriteLogDebug(const std::string& title, const std::string& str) {
	if (!bDebugLog) return;

	static auto file = std::ofstream("FlatOutChloeCollection_gcp_debug.log");

	file << "[" + title + "] ";
	file << str;
	file << "\n";
	file.flush();
}

std::string GetStringNarrow(const wchar_t* string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(string);
}

#include "filereader.h"
#include "config.h"
#include "customsave.h"
#include "cardealer.h"
#include "achievements.h"
#include "customsettings.h"
#include "cardamage.h"
#include "carlimitadjuster.h"
#include "carreset.h"
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
#include "luafunctions.h"
#include "nitrogain.h"
#include "ultrawide.h"
#include "newmap.h"
#include "carnagerace.h"
#include "debugmenu.h"

void SetHandlingDamage() {
	int handlingDamage = nHandlingDamage;
	if (CareerMode::IsCareerTimeTrial()) handlingDamage = HANDLINGDAMAGE_REDUCED;

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

	SetWindowedMode();
	SetHandlingDamage();
	SetHandlingMode();
	ProcessCarDamage();
	CareerMode::OnTick();
	ProcessNitroGain();

	// rng buffer overrun hack-fix
	if (RNGGenerator::nNumValuesLeft <= 1) {
		RNGGenerator::Reset();
	}
}

void OnFilesystemInit() {
	NewMusicPlayer::Init();
	ApplyCarDealerPatches();
}

void CommandlineArgReader(void* a1, const char* a2) {
	auto str = (std::string)a2;
	if (str == "-debug") bDebugLog = true;
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

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			DoFlatOutVersionCheck(FO2Version::FO1_1_1);

			srand(time(0));

			NyaFO2Hooks::PlaceD3DHooks();
			NyaFO2Hooks::aEndSceneFuncs.push_back(CustomSetterThread);
			NyaFO2Hooks::aEndSceneFuncs.push_back(D3DHookMain);
			NyaFO2Hooks::aD3DResetFuncs.push_back(OnD3DReset);
			NyaFO2Hooks::PlaceWndProcHook();
			NyaFO2Hooks::aWndProcFuncs.push_back(WndProcHook);

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
			//ApplyUltrawidePatches(); // todo finish this
			ApplyIngameMapPatches();
			ApplyXInputPatches();
			ApplyAIExtenderPatches();
			CareerMode::Init();
			Achievements::Init();

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4A74CA, 0x4A757F); // remove copyright screen
			NyaHookLib::Patch<uint8_t>(0x4A6E8F, 0xEB); // remove intro videos

			NyaHookLib::Patch(0x68BDE0, "gamesave"); // change savefile name

			/*uintptr_t addresses[] = {
				0x438482,
				0x43991B,
				0x43A702,
				0x43A721,
				0x444FBA,
				0x445267,
				0x4A6DFC,
				0x4A7258,
				0x4A75A1,
			};
			for (auto& addr : addresses) {
				NyaHookLib::Patch(addr, "SaveGame/gameoptions.cfg"); // change options save name
			}*/

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

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4D9A93, &CommandlineArgReader);
			OnFilesystemInitASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A7261, &OnFilesystemInitASM);

			UpdateCameraHooked_call = (void(__thiscall*)(void*, float))(*(uintptr_t*)0x662978);
			NyaHookLib::Patch(0x662978, &UpdateCameraHooked);
		} break;
		default:
			break;
	}
	return TRUE;
}