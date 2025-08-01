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

#include "bfsload.h"
#include "filereader.h"
#include "config.h"
#include "customsave.h"
#include "customsettings.h"
#include "cardealer.h"
#include "carlimitadjuster.h"
#include "hudextensions.h"
#include "musicplayer.h"
#include "d3dhook.h"
#include "windowedmode.h"
#include "luafunctions.h"
#include "ddsparser.h"
#include "debugmenu.h"

void SetHandlingDamage() {
	static int nLast = -1;
	if (nLast != nHandlingDamage) {
		NyaHookLib::Patch<uint64_t>(0x416312, nHandlingDamage ? 0x9F8D000001B2850F : 0x9F8D90000001B3E9);
		nLast = nHandlingDamage;
	}
}

void CustomSetterThread() {
	pGameFlow->nAutoTransmission = !nTransmission;
	if (gCustomSave.bInitialized) {
		gCustomSave.aCareerGarage[pGameFlow->Profile.nCarType+1].ApplyUpgradesToCar();
	}

	SetHandlingDamage();
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

			// 004E3CDD disable menu ui
		} break;
		default:
			break;
	}
	return TRUE;
}