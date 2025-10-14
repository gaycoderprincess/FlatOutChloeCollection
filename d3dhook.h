int nDrawingGameUILayer = -1;
void HookLoop() {
	//DeviceD3d::nDesiredCullMode = bTrackMirrored ? D3DCULL_NONE : D3DCULL_CCW;

	DrawCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
		//g_pd3dDevice->SetRenderState(D3DRS_WRAP0, 0);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	}, false);

	if (nDrawingGameUILayer >= 0) {
		ChloeEvents::DrawRaceUIEvent.OnHit(nDrawingGameUILayer);
		bDontRefreshInputsThisLoop = true;
		CommonMain();
		nDrawingGameUILayer = -1;
		return;
	}

	if (Menu_LoadingScreen.bHasRun) ChloeMenuHud::Init();
	ChloeMenuHud::InitHooks();
	NewGameHud::Init();
	ChloeEvents::DrawAboveUIEvent.OnHit();
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

void InitD3D() {
	// d3d hooks done later so the custommp chat ui has priority
	NyaFO2Hooks::PlaceD3DHooks();
	NyaFO2Hooks::aEndSceneFuncs.push_back(D3DHookMain);
	NyaFO2Hooks::aD3DResetFuncs.push_back(OnD3DReset);
}

void __stdcall D3DGameUI(int) {
	for (int i = 0; i < (int)eHUDLayer::NUM_LAYERS; i++) {
		nDrawingGameUILayer = i;
		D3DHookMain();
	}
}

void ClearD3D() {
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
}

ChloeHook Hook_D3DDraws([]() {
	NyaFO2Hooks::aPostPresentFuncs.push_back(ClearD3D);
	ChloeEvents::FilesystemInitEvent.AddHandler(InitD3D);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x45315E, &D3DGameUI);
});