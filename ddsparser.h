bool IsNativelySupportedDDSFormat(uint32_t format) {
	if (format == 0x31545844) return true; // DXT1
	if (format == 0x33545844) return true; // DXT3
	if (format == 0x34545844) return true; // DXT4
	if (format == 0x35545844) return true; // DXT5
	return false;
}

DevTexture* __thiscall CreateTextureFromMemoryNew(DeviceD3d* pThis, DevTexture* pTexture, uint8_t* data, uint32_t dataSize, uint32_t flags) {
	if (IsNativelySupportedDDSFormat(*(uint32_t*)(&data[0x54]))) {
		return DeviceD3d::CreateTextureFromMemory(pThis, pTexture, data, dataSize, flags);
	}

	if (!pTexture) {
		// small dummy texture for constructing a DevTexture
		pTexture = DeviceD3d::CreateTextureFromFile(pThis, pTexture, "data/global/overlay/overlaynitro.dds", flags);
	}

	if (pTexture->pD3DTexture) {
		pTexture->pD3DTexture->Release();
		pTexture->pD3DTexture = nullptr;
	}

	WriteLog(std::format("loading texture {} with size {}", pTexture->sPath.Get(), dataSize));

	// fix header
	if (data[0x4C] == 0x18) {
		data[0x4C] = 0x20;
	}

	auto hr = D3DXCreateTextureFromFileInMemory(pDeviceD3d->pD3DDevice, data, dataSize, &pTexture->pD3DTexture);
	if (hr != S_OK) {
		WriteLog(std::format("Failed to load {} - {:X}", pTexture->sPath.Get(), (uint32_t)hr));
		return nullptr;
	}

	pTexture->nLoadState = 5;
	return pTexture;
}

bool __fastcall NewDDSParser(uint8_t* header, void*, DevTexture* pThis, File* pFile) {
	if (IsNativelySupportedDDSFormat(*(uint32_t*)(&header[0x50]))) {
		if (ParseGameDDS(header, nullptr, pThis, pFile)) return true;
	}

	if (pThis->pD3DTexture) {
		pThis->pD3DTexture->Release();
		pThis->pD3DTexture = nullptr;
	}

	//WriteLog(std::format("loading texture {}", pThis->sPath.Get()));

	size_t fileSize = pFile->pFileCodec->GetFileSize();

	auto tmp = new uint8_t[fileSize];
	*(uint32_t*)tmp = 0x20534444; // DDS32
	memcpy(&tmp[4], header, 0x7C);
	pFile->pFileCodec->ReadBytes(&tmp[0x7C+4], fileSize-0x7C-0x4);

	// fix header
	if (tmp[0x4C] == 0x18) {
		tmp[0x4C] = 0x20;
	}

	auto hr = D3DXCreateTextureFromFileInMemory(pDeviceD3d->pD3DDevice, tmp, fileSize, &pThis->pD3DTexture);
	delete[] tmp;
	if (hr != S_OK) {
		WriteLog(std::format("Failed to load {} - {:X}", pThis->sPath.Get(), (uint32_t)hr));
		return false;
	}

	pThis->nLoadState = 5;
	return true;
}

void __attribute__((naked)) NewDDSParserASM() {
	__asm__ (
		"mov ecx, eax\n\t"
		"jmp %0\n\t"
			:
			:  "i" (NewDDSParser)
	);
}

void ApplyDDSParserPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x50B312, &NewDDSParserASM); // file
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x50B614, &NewDDSParserASM2); // memory
	NyaHookLib::Patch(0x667A10, &CreateTextureFromMemoryNew);
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x6279E3, &PreferTGAForUI);
}