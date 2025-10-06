tGameSetting aNewGameSettings[] = {
		{ "Version", "Settings", (void*)0x6C0268, 0, 0, 1e+06 },
		{ "Game", "ImperialUnits", (void*)0x6C027C, 1, 0, 0 },
		{ "Game", "OverlayGauges", (void*)0x6C0280, 1, 0, 0 },
		{ "Game", "IngameMap", (void*)0x6C0284, 1, 0, 0 },
		{ "Game", "Camera", (void*)0x6C029C, 0, 0, 9 },
		{ "Game", "UseNewLoadingScreen", &nUseNewLoadingScreen, 1, 0, 0 },
		{ "Game", "HandlingDamage", &nHandlingDamage, 0, 0, 2 },
		{ "Game", "HandlingMode", &nHandlingMode, 0, 0, 2 },
		{ "Game", "PlayerListDefaultState", &nPlayerListDefaultState, 0, 0, 1 },
		{ "Game", "Soundtrack", &nIngameSoundtrack, 0, 0, 1 },
		{ "Game", "DerbySoundtrack", &nIngameDerbySoundtrack, 0, 0, 1 },
		{ "Game", "StuntSoundtrack", &nIngameStuntSoundtrack, 0, 0, 1 },
		{ "Game", "MenuSoundtrack", &nMenuSoundtrack, 0, 0, 1 },
		{ "Game", "ShowBonus", &nShowBonus, 1, 0, 0 },
		{ "Game", "HighCarCam", &nHighCarCam, 0, 0, 1 },
		{ "Game", "SplitScreenDamageMeter", &nSplitScreenDamageMeter, 1, 0, 0 },
		{ "Game", "ChatColor", &nPlayerColor, 0, 0, 100 },
		{ "Game", "PlayerFlag", &nPlayerFlag, 0, 0, 100 },
		{ "Game", "UseFO2Minimap", &nUseFO2Minimap, 0, 0, 2 },
		{ "Control", "ControllerSupport", &nControllerSupport, 1, 0, 0 },
		{ "Control", "ControllerDeadzone", (void*)0x6BEE0C, 0, 0, 100 },
		{ "Control", "ControllerLayout", &nControllerLayout, 0, 0, 10 },
		{ "Control", "DigitalCenteringSpeed", (void*)0x6BEE18, 2, 0, 100 },
		{ "Control", "DigitalSteeringMaxSpeed", (void*)0x6BEE1C, 2, 0, 100 },
		{ "Control", "DigitalSteeringMinSpeed", (void*)0x6BEE20, 2, 0, 100 },
		{ "Control", "Transmission", &nTransmission, 1, 0, 0 },
		{ "Control", "SplitScreenPad1", &nSplitScreenController[0], 0, 0, SPLITSCREEN_P1_PAD4 },
		{ "Control", "SplitScreenPad2", &nSplitScreenController[1], 0, 0, SPLITSCREEN_PAD4 },
		{ "Control", "SplitScreenPad3", &nSplitScreenController[2], 0, 0, SPLITSCREEN_PAD4 },
		{ "Control", "SplitScreenPad4", &nSplitScreenController[3], 0, 0, SPLITSCREEN_PAD4 },
		{ "Visual", "Windowed", &nWindowedMode, 1, 0, 0 },
		{ "Visual", "AlphaBlend", (void*)0x6C02C4, 1, 0, 0 },
		{ "Visual", "SunFlare", (void*)0x6C02CC, 1, 0, 0 },
		{ "Visual", "TextureQuality", (void*)0x6C02D0, 0, 0, 4 },
		{ "Visual", "TrackDetail", (void*)0x6C02D4, 0, 0, 2 },
		{ "Visual", "PS2MotionBlurIngame", (void*)0x6C02D8, 0, 0, 255 },
		{ "Visual", "Visibility", (void*)0x6C02DC, 0, 0, 100 },
		{ "Audio", "InterfaceMusicVolume", (void*)0x6C02E0, 0, 0, 100 },
		{ "Audio", "InterfaceSfxVolume", (void*)0x6C02E4, 0, 0, 100 },
		{ "Audio", "IngameMusicVolume", (void*)0x6C02E8, 0, 0, 100 },
		{ "Audio", "IngameAmbientVolume", (void*)0x6C02EC, 0, 0, 100 },
		{ "Audio", "IngameSfxVolume", (void*)0x6C02F0, 0, 0, 100 },
		{ "Audio", "DopplerEffects", (void*)0x6C02F4, 1, 0, 0 },
		{ "Network", "Interpolation", &nMPInterpolation, 0, 0, 1 },
		{ "Network", "DefaultTickRate", &nDefaultTickRate, 0, 0, 4 },
		{ "Network", "AllowCheats", &nAllowCheats, 1, 0, 0 },
		{ "Network", "JoinNotifs", &nJoinNotifs, 1, 0, 0 },
		{ nullptr, nullptr, nullptr, 0, 0, 0 },
};

void WriteSettingsToTestFile() {
	auto file = std::ofstream("test.cpp");
	auto setting = (tGameSetting*)0x68B7E8;
	while (setting->category) {
		file << std::format("{{ \"{}\", \"{}\", (void*)0x{:X}, {}, {}, {} }},\n", setting->category, setting->name, (uintptr_t)setting->value, setting->type, setting->minValue, setting->maxValue);
		setting++;
	}
}

void ApplyCustomSettingsPatches() {
	uintptr_t aCategoryAddresses[] = {
			0x4399E2,
			0x43A0AF,
			0x43A17B,
			0x43A29F,
			0x43A36B,
			0x43A580,
			0x43A589,
			0x43A650,
			0x43A659,
	};
	for (auto& addr : aCategoryAddresses) {
		NyaHookLib::Patch(addr, &aNewGameSettings[0].category);
	}
	uintptr_t aNameAddresses[] = {
			0x43A0C5,
			0x43A2B5,
	};
	for (auto& addr : aNameAddresses) {
		NyaHookLib::Patch(addr, &aNewGameSettings[0].name);
	}
	NyaHookLib::Patch(0x4399ED, &aNewGameSettings[0].maxValue);
}