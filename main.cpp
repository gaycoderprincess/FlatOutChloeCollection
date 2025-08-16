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
#include "chloemenulib.h"

void WriteLog(const std::string& str) {
	static auto file = std::ofstream("FlatOutChloeCollection_gcp.log");

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
#include "debugmenu.h"

void SetHandlingDamage() {
	if (nHandlingDamage == HANDLINGDAMAGE_ON) return;
	if (pLoadingScreen) return;
	if (GetGameState() != GAME_STATE_RACE) return;

	auto ply = GetPlayer(0);
	if (!ply) return;
	ply->pCar->FixPart(eCarFixPart::SUSPENSION);
	if (nHandlingDamage == HANDLINGDAMAGE_OFF) {
		ply->pCar->FixPart(eCarFixPart::WHEELS);
	}
}

void SetHandlingMode() {
	static int nLast = -1;
	if (nLast != nHandlingMode) {
		NyaHookLib::Patch<uint64_t>(0x418B39, nHandlingMode == HANDLING_HARDCORE ? 0x86D9900000038DE9 : 0x86D90000038C840F);
		NyaHookLib::Patch<uint64_t>(0x419211, nHandlingMode == HANDLING_HARDCORE ? 0x44D990000003D7E9 : 0x44D9000003D6850F);
		nLast = nHandlingMode;
	}
	pGameFlow->Profile.nEasyDifficulty = nHandlingMode == HANDLING_NORMAL;
}

void CustomSetterThread() {
	pGameFlow->nAutoTransmission = !nTransmission;
	if (gCustomSave.bInitialized) {
		gCustomSave.aCareerGarage[pGameFlow->Profile.nCarType+1].ApplyUpgradesToCar();
	}

	SetWindowedMode();
	SetHandlingDamage();
	SetHandlingMode();
	ProcessCarDamage();
	CareerMode::OnTick();
	ProcessNitroGain();
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
			ApplyCarDealerPatches();
			ApplyCarDamagePatches();
			ApplyCarResetPatches();
			ApplyNitroGainPatches();
			//ApplyUltrawidePatches(); // todo finish this
			ApplyIngameMapPatches();
			ApplyXInputPatches();
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
		} break;
		default:
			break;
	}
	return TRUE;
}