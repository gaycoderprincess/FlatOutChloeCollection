double fAspectRatio = 16.0 / 9.0;
double f43AspectCorrection = 480 * fAspectRatio; // 853
double f43AspectCorrection_inv = 1.0 / f43AspectCorrection;
float f43AspectCorrection_inv_flt = f43AspectCorrection_inv;
float fTextCenteringMult = 0.5 * ((4.0 / 3.0) / fAspectRatio);
float fResMultWithNewHudScale = 2.25;
void RecalculateAspectRatio() {
	fAspectRatio = (double)nGameResolutionX / (double)nGameResolutionY;
	f43AspectCorrection = 480 * fAspectRatio;
	f43AspectCorrection_inv = 1.0 / f43AspectCorrection;
	f43AspectCorrection_inv_flt = f43AspectCorrection_inv;
	fResMultWithNewHudScale = nGameResolutionX * (1.0 / f43AspectCorrection);
	fTextCenteringMult = 0.5 * ((4.0 / 3.0) / fAspectRatio);

	static float resX, resY;
	resX = nGameResolutionX;
	resY = nGameResolutionY;
	NyaHookLib::Patch(0x5069DA, resX);
	NyaHookLib::Patch(0x5069E0, resY);
}

float* fTextScale = nullptr;
void UltrawideTextScale() {
	RecalculateAspectRatio();

	//if (!nWidescreenMenu) return;

	fTextScale[0] = fTextScale[1];
	//*fTextScale *= 640.0;
	//*fTextScale /= f43AspectCorrection;
}

uintptr_t UltrawideTextScaleASM_jmp = 0x4F0D06;
void __attribute__((naked)) UltrawideTextScaleASM() {
	__asm__ (
			"add ecx, 0x34\n\t"
			"mov %2, ecx\n\t"
			"sub ecx, 0x34\n\t"
			"pushad\n\t"
			"call %1\n\t"
			"popad\n\t"

			"sub esp, 0x14\n\t"
			"push ebx\n\t"
			"mov ebx, ecx\n\t"
			"jmp %0\n\t"
				:
				: "m" (UltrawideTextScaleASM_jmp), "i" (UltrawideTextScale), "m" (fTextScale)
	);
}

struct CameraExtents {
	uint8_t _0[0x14]; // +0
	float fLeft; // +14
	float fRight; // +18
	float fTop; // +1C
	float fBottom; // +20
};

double fOrigAspect = 4.0 / 3.0;
void __fastcall UltrawideFOV(Camera* pCam) {
	RecalculateAspectRatio();

	auto mult = 1.0 / fOrigAspect;
	auto v12 = tan(pCam->fFOV * 0.5) * pCam->fNearZ;
	pCam->fRight = v12 * fAspectRatio * mult;
	pCam->fLeft = -v12 * fAspectRatio * mult;
	pCam->fBottom = v12 * mult;
	pCam->fTop = -v12 * mult;
	if (IsInHalvedSplitScreen()) {
		pCam->fBottom /= 2;
		pCam->fTop /= 2;
	}
}

uintptr_t UltrawideFOVMenuASM_jmp = 0x47E165;
void __attribute__((naked)) UltrawideFOVMenuASM() {
	__asm__ (
		"fmul dword ptr [esp+0x4C]\n\t"
		"fstp dword ptr [ebx+0xC]\n\t"
		"fld dword ptr [esp+0x4C]\n\t"
		"fmul st, st(1)\n\t"
		"fstp dword ptr [ebx+8]\n\t"

		"pushad\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"

		"jmp %0\n\t"
			:
			: "m" (UltrawideFOVMenuASM_jmp), "i" (UltrawideFOV)
	);
}

uintptr_t UltrawideFOVIngameASM_jmp = 0x47C6D3;
void __attribute__((naked)) UltrawideFOVIngameASM() {
	__asm__ (
		"fstp dword ptr [eax+0x100]\n\t"
		"fstp dword ptr [eax+0x104]\n\t"

		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"popad\n\t"

		"jmp %0\n\t"
			:
			: "m" (UltrawideFOVIngameASM_jmp), "i" (UltrawideFOV)
	);
}

void __fastcall UltrawideFOVSky(CameraExtents* pCam, Camera* pCamera) {
	auto fOriginalMult = 1.0 / 1.33333333;
	auto v12 = tan(pCamera->fFOV * 0.5) * 5.0;
	pCam->fRight = v12 * fAspectRatio * fOriginalMult;
	pCam->fLeft = -v12 * fAspectRatio * fOriginalMult;
	pCam->fBottom = v12 * fOriginalMult;
	pCam->fTop = -v12 * fOriginalMult;
	if (IsInHalvedSplitScreen()) {
		pCam->fBottom /= 2;
		pCam->fTop /= 2;
	}
}

uintptr_t UltrawideFOVSkyASM_jmp = 0x4C8AF8;
void __attribute__((naked)) UltrawideFOVSkyASM() {
	__asm__ (
		"fstp st\n\t"

		"push ecx\n\t"
		"push edx\n\t"
		"lea ecx, [esp+8]\n\t"
		"mov edx, ebp\n\t"
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"pop edx\n\t"
		"pop ecx\n\t"

		"call dword ptr [ebx+0xA0]\n\t"
		"jmp %0\n\t"
			:
			: "m" (UltrawideFOVSkyASM_jmp), "i" (UltrawideFOVSky)
	);
}

void ApplyUltrawidePatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x47E155, &UltrawideFOVMenuASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x47C6C7, &UltrawideFOVIngameASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4C8AF0, &UltrawideFOVSkyASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4F0D00, &UltrawideTextScaleASM);

	// menu
	NyaHookLib::Patch(0x4E0192 + 2, &fResMultWithNewHudScale); // menu title text
	NyaHookLib::Patch(0x4E28B4 + 2, &fResMultWithNewHudScale); // fix TITLE_CENTER_X in menus
	NyaHookLib::Patch(0x4DFDDE + 2, &fTextCenteringMult); // centered menu items
	NyaHookLib::Patch(0x457AC1 + 2, &f43AspectCorrection_inv_flt); // credits text

	// ingame hud
	NyaHookLib::Patch(0x450F6A + 2, &f43AspectCorrection_inv_flt); // speedometer scaling
	NyaHookLib::Patch(0x45B237 + 2, &f43AspectCorrection_inv_flt); // countdown scaling
	NyaHookLib::Patch(0x454D82 + 2, &f43AspectCorrection_inv_flt); // text justify
	NyaHookLib::Patch(0x454B2F + 2, &f43AspectCorrection_inv_flt); // total time text
	NyaHookLib::Patch(0x453923 + 2, &f43AspectCorrection_inv_flt); // music player popup
	return;

	std::ifstream t("Config/WindowFunctions.bed");
	if (t.is_open()) {
		std::stringstream buffer;
		buffer << t.rdbuf();

		int len = buffer.str().length()+1;
		static char* windowFunctions = new char[len];
		strcpy_s(windowFunctions, len, buffer.str().c_str());
		windowFunctions[len-1]=0;
		NyaHookLib::Patch(0x4DD6A0 + 1, windowFunctions);
		NyaHookLib::Patch(0x4DD69B + 1, len-1);
	}
}