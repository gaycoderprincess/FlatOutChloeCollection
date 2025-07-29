bool bForceFullscreen = false;
int nWindowedMode = 0;

RECT GetMonitorRect(HWND hwnd) {
	RECT rect;
	auto monitor = MonitorFromWindow(hwnd, 0);
	MONITORINFO mi;
	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);
	if (monitor && GetMonitorInfoA(monitor, &mi)) rect = mi.rcWork;
	else SystemParametersInfoA(0x30u, 0, &rect, 0);
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
		auto resX = nGameResolutionX;
		auto resY = nGameResolutionY;
		auto rect = GetMonitorRect(hwnd);
		if (!nWindowedMode && rect.right - rect.left > resX) resX = rect.right - rect.left;

		auto style = GetWindowLongA(hwnd, GWL_STYLE);
		uint32_t targetStyle = (WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		if (nWindowedMode) style |= targetStyle;
		else style &= ~targetStyle;
		SetWindowLongA(hwnd, GWL_STYLE, style);
		SetWindowPos(hwnd, nullptr, rect.left, rect.top, resX, resY, SWP_NOZORDER | SWP_FRAMECHANGED);
		SetFocus(hwnd);
		nLastWindowed = nWindowedMode;
	}
}

void ApplyWindowedModePatches() {
	bForceFullscreen = std::filesystem::exists("fullscreen");
	if (bForceFullscreen) return;

	// don't show cursor
	//NyaHookLib::Patch(0x60F23B, 0x9090D231);

	// always run in windowed - this'll be borderless
	NyaHookLib::Patch<uint16_t>(0x438B5C, 0x9090);
}