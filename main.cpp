#include <windows.h>
#include <fstream>
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

#include "bfsload.h"
#include "filereader.h"
#include "config.h"
#include "customsave.h"
#include "customsettings.h"
#include "cardamage.h"
#include "cardealer.h"
#include "carlimitadjuster.h"
#include "careermode.h"
#include "hudextensions.h"
#include "ingamehud.h"
#include "musicplayer.h"
#include "d3dhook.h"
#include "windowedmode.h"
#include "luafunctions.h"
#include "ddsparser.h"
#include "nitrogain.h"
#include "debugmenu.h"

void SetHandlingDamage() {
	static int nLast = -1;
	if (nLast != nHandlingDamage) {
		NyaHookLib::Patch<uint64_t>(0x416312, nHandlingDamage ? 0x9F8D000001B2850F : 0x9F8D90000001B3E9);
		//NyaHookLib::Patch<uint64_t>(0x41624E, nHandlingDamage ? 0x45D900000442850F : 0x45D99000000443E9);
		nLast = nHandlingDamage;
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

			// 004E3CDD disable menu ui
		} break;
		default:
			break;
	}
	return TRUE;
}