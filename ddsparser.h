bool IsNativelySupportedDDSFormat(const uint8_t* data) {
	auto format = *(uint32_t*)(&data[0x54]);
	if (format == 0x31545844) return true; // DXT1
	if (format == 0x33545844) return true; // DXT3
	if (format == 0x34545844) return true; // DXT4
	if (format == 0x35545844) return true; // DXT5
	return false;
}

bool CreateCustomTexture(DevTexture*& pTexture, uint8_t* data, uint32_t dataSize, uint32_t flags) {
	if (IsNativelySupportedDDSFormat(data)) {
		pTexture = DeviceD3d::CreateTextureFromMemory(pDeviceD3d, pTexture, data, dataSize, flags);
		return pTexture->pD3DTexture;
	}

	if (!pTexture) {
		// small dummy texture for constructing a DevTexture
		pTexture = DeviceD3d::CreateTextureFromFile(pDeviceD3d, pTexture, "data/global/overlay/overlaynitro.dds", flags);
	}

	if (pTexture->pD3DTexture) {
		pTexture->pD3DTexture->Release();
		pTexture->pD3DTexture = nullptr;
	}

	// dds identifier
	if (data[0] == 0x44 && data[1] == 0x44 && data[2] == 0x53 && data[3] == 0x20) {
		// fix header
		if (data[0x4C] == 0x18) {
			data[0x4C] = 0x20;
		}
	}

	auto hr = D3DXCreateTextureFromFileInMemory(pDeviceD3d->pD3DDevice, data, dataSize, &pTexture->pD3DTexture);
	if (hr != S_OK) {
		WriteLog(std::format("Failed to load {} - {:X}", pTexture->sPath.Get(), (uint32_t)hr));
		return false;
	}

	pTexture->nLoadState = 5;
	return true;
}

uint8_t* ReadTextureDataFromFile(const char* _path, size_t* outSize) {
	std::string path = _path;
	// prefer tga files if they exist
	if (path.ends_with(".dds")) {
		path.pop_back();
		path.pop_back();
		path.pop_back();
		path += "tga";
	}
	if (!DoesFileExist(path.c_str()) && path.ends_with(".tga")) {
		path.pop_back();
		path.pop_back();
		path.pop_back();
		path += "dds";
	}
	if (!DoesFileExist(path.c_str())) {
		MessageBoxA(0, std::format("Failed to find file {}", _path).c_str(), "Fatal error", MB_ICONERROR);
		return nullptr;
	}

	File file;
	file.Load(path.c_str(), 9);

	*outSize = file.pFileCodec->GetFileSize();

	auto data = new uint8_t[*outSize];
	memset(data, 0, *outSize);
	file.pFileCodec->ReadBytes(data, *outSize);
	return data;
}

DevTexture* __thiscall CreateTextureFromMemoryNew(DeviceD3d* pThis, DevTexture* pTexture, uint8_t* data, uint32_t dataSize, uint32_t flags) {
	if (CreateCustomTexture(pTexture, data, dataSize, flags)) {
		return pTexture;
	}
	return nullptr;
}

DevTexture* __thiscall CreateTextureFromFileNew(DeviceD3d* pThis, DevTexture* pTexture, const char* path, uint32_t flags) {
	size_t dataSize;
	auto data = ReadTextureDataFromFile(path, &dataSize);
	if (!data) return nullptr;

	if (IsNativelySupportedDDSFormat(data)) {
		delete[] data;
		pTexture = DeviceD3d::CreateTextureFromFile(pDeviceD3d, pTexture, path, flags);
		return pTexture;
	}

	if (!pTexture) {
		// small dummy texture for constructing a DevTexture
		pTexture = DeviceD3d::CreateTextureFromFile(pThis, pTexture, "data/global/overlay/overlaynitro.dds", flags);
	}

	if (CreateCustomTexture(pTexture, data, dataSize, flags)) {
		delete[] data;
		return pTexture;
	}
	delete[] data;
	return nullptr;
}

ChloeHook Hook_DDSParser([]() {
	NyaHookLib::Patch(0x667A0C, &CreateTextureFromFileNew);
	NyaHookLib::Patch(0x667A10, &CreateTextureFromMemoryNew);
});