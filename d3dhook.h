void HookLoop() {
	NewMenuHud::OnTick();
	NewGameHud::OnTick();
	NewMusicPlayer::OnTick();
	Achievements::OnTick();
	ProcessPlayStats();
	CommonMain();
}

void UpdateD3DProperties() {
	g_pd3dDevice = DeviceD3d::pD3DDevice;
	ghWnd = DeviceD3d::hWnd;
	nResX = nGameResolutionX;
	nResY = nGameResolutionY;
}

bool bDeviceJustReset = false;
void D3DHookMain() {
	if (!g_pd3dDevice) {
		UpdateD3DProperties();
		InitHookBase();
	}

	if (bDeviceJustReset) {
		ImGui_ImplDX9_CreateDeviceObjects();
		bDeviceJustReset = false;
	}
	HookBaseLoop();
}

void OnD3DReset() {
	if (g_pd3dDevice) {
		UpdateD3DProperties();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		bDeviceJustReset = true;
	}
}