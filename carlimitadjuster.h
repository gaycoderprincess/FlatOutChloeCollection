void __fastcall LoadDevTexture(DevTexture* pTexture) {
	if (pTexture->pD3DTexture) return;

	pDeviceD3d->_vf_CreateTextureFromFile(pTexture, pTexture->sPath.Get(), pTexture->nFlags);
}

uintptr_t LoadMenuTexturesASM_jmp = 0x460E9E;
void __attribute__((naked)) LoadMenuTexturesASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"popad\n\t"

		"mov esi, [esp+0xC]\n\t"
		"mov edx, [esp+0xA4]\n\t"
		"jmp %0\n\t"
			:
			:  "m" (LoadMenuTexturesASM_jmp), "i" (LoadDevTexture)
	);
}

void ApplyCarLimitAdjusterPatches() {
	// remove hardcoded shared path from lights_damaged
	static const char lightsDamagedPath[] = "lights_damaged.tga";
	NyaHookLib::Patch(0x41E483 + 1, lightsDamagedPath);

	// swap folder priorities
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41E41D, 0x4EFDD0);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41E430, 0x4EFD60);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4244B1, 0x4EFDD0);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4244B9, 0x4EFD60);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x460E93, &LoadMenuTexturesASM);
}