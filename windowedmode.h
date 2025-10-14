bool bForceFullscreen = false;
int nWindowedMode = 0;

RECT GetMonitorRect(HWND hwnd) {
	RECT rect;
	auto monitor = MonitorFromWindow(hwnd, 0);
	MONITORINFO mi;
	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);
	if (monitor && GetMonitorInfoA(monitor, &mi)) rect = mi.rcMonitor;
	else {
		rect.left = 0;
		rect.right = GetSystemMetrics(SM_CXSCREEN);
		rect.top = 0;
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	return rect;
}

void SetWindowedMode() {
	if (bForceFullscreen) return;

	static int nLastWindowed = -1;

	// reset borderless a bit later, fixes misplacement
	static CNyaTimer gTimer;
	if (gTimer.fTotalTime < 1) {
		gTimer.Process();
		if (gTimer.fTotalTime >= 1 && nLastWindowed == 0) nLastWindowed = -1;
	}

	if (nLastWindowed != nWindowedMode) {
		auto hwnd = DeviceD3d::hWnd;
		auto rect = GetMonitorRect(hwnd);
		if (nWindowedMode) {
			rect.right = rect.left + nGameResolutionX;
			rect.bottom = rect.top + nGameResolutionY;
		}

		auto style = GetWindowLongA(hwnd, GWL_STYLE);
		uint32_t targetStyle = (WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		if (nWindowedMode) style |= targetStyle;
		else style &= ~targetStyle;
		SetWindowLongA(hwnd, GWL_STYLE, style);

		if (nWindowedMode) {
			AdjustWindowRect(&rect, style, false);
		}
		SetWindowPos(hwnd, nullptr, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		SetFocus(hwnd);
		nLastWindowed = nWindowedMode;
	}
}

uintptr_t SetWindowedModeASM_jmp = 0x504EFE;
void __attribute__((naked)) SetWindowedModeASM() {
	__asm__ (
		"mov dword ptr [esi+0x224], 1\n\t"
		"mov dword ptr [esp+0x10], 0x90080000\n\t"
		"jmp %0\n\t"
			:
			:  "m" (SetWindowedModeASM_jmp)
	);
}

ChloeHook gWindowedMode([]() {
	bForceFullscreen = std::filesystem::exists("fullscreen");
	if (bForceFullscreen) return;

	// don't show cursor
	//NyaHookLib::Patch(0x60F23B, 0x9090D231);

	// always run in windowed - this'll be borderless
	NyaHookLib::Patch<uint16_t>(0x438B5C, 0x9090);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x504EF0, &SetWindowedModeASM);

	ChloeEvents::FinishFrameEvent.AddHandler(SetWindowedMode);
});