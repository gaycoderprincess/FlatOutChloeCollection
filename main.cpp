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
#include "hudextensions.h"
#include "musicplayer.h"
#include "d3dhook.h"
#include "windowedmode.h"
#include "luafunctions.h"
#include "ddsparser.h"
#include "debugmenu.h"

void CustomSetterThread() {
	pGameFlow->nAutoTransmission = !nTransmission;
	if (gCustomSave.bInitialized) {
		gCustomSave.aCareerGarage[pGameFlow->Profile.nCarType+1].ApplyUpgradesToCar();
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

			NyaHookLib::Patch(0x4658C4 + 1, 2097152); // menucar model memory
			NyaHookLib::Patch(0x4658C9 + 3, 2097152); // menucar model memory
			NyaHookLib::Patch(0x4658D8 + 1, 16777216); // menucar skin memory
			NyaHookLib::Patch(0x4658DD + 3, 16777216); // menucar skin memory

			// 004E3CDD disable menu ui
		} break;
		default:
			break;
	}
	return TRUE;
}