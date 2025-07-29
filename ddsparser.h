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

DevTexture* __thiscall CreateTextureFromFileNew(DeviceD3d* pThis, DevTexture* pTexture, const char* _path, uint32_t flags) {
	if (!pTexture) {
		// small dummy texture for constructing a DevTexture
		pTexture = DeviceD3d::CreateTextureFromFile(pThis, pTexture, "data/global/overlay/overlaynitro.dds", flags);
	}

	std::string path = _path;
	if (!DoesFileExist(path.c_str()) && path.ends_with(".tga")) {
		path.pop_back();
		path.pop_back();
		path.pop_back();
		path += "dds";
	}

	File file;
	file.Load(path.c_str(), 9);

	uint32_t dataSize = file.pFileCodec->GetFileSize();
	WriteLog(std::format("loading texture {} with size {}", path, dataSize));

	auto data = new uint8_t[dataSize];
	file.pFileCodec->ReadBytes(data, dataSize);

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
	delete[] data;
	if (hr != S_OK) {
		WriteLog(std::format("Failed to load {} - {:X}", pTexture->sPath.Get(), (uint32_t)hr));
		return nullptr;
	}

	pTexture->nLoadState = 5;
	return pTexture;
}

void ApplyDDSParserPatches() {
	NyaHookLib::Patch(0x667A0C, &CreateTextureFromFileNew);
	NyaHookLib::Patch(0x667A10, &CreateTextureFromMemoryNew);
}