bool CreateCustomTexture(DevTexture*& pTexture, uint8_t* data, uint32_t dataSize, uint32_t flags) {
	if (!pTexture) {
		// small dummy texture for constructing a DevTexture
		pTexture = DeviceD3d::CreateTextureFromFile(pDeviceD3d, pTexture, "data/global/overlay/overlaynitro.dds", flags);
	}

	if (pTexture->pD3DTexture) {
		pTexture->pD3DTexture->Release();
		pTexture->pD3DTexture = nullptr;
	}

	// fix header
	if (data[0x4C] == 0x18) {
		data[0x4C] = 0x20;
	}

	auto hr = D3DXCreateTextureFromFileInMemory(pDeviceD3d->pD3DDevice, data, dataSize, &pTexture->pD3DTexture);
	if (hr != S_OK) {
		WriteLog(std::format("Failed to load {} - {:X}", pTexture->sPath.Get(), (uint32_t)hr));
		return false;
	}

	pTexture->nLoadState = 5;
	return true;
}

DevTexture* __thiscall CreateTextureFromMemoryNew(DeviceD3d* pThis, DevTexture* pTexture, uint8_t* data, uint32_t dataSize, uint32_t flags) {
	if (CreateCustomTexture(pTexture, data, dataSize, flags)) {
		return pTexture;
	}
	return nullptr;
}

DevTexture* __thiscall CreateTextureFromFileNew(DeviceD3d* pThis, DevTexture* pTexture, const char* _path, uint32_t flags) {
	if (!pTexture) {
		// small dummy texture for constructing a DevTexture
		pTexture = DeviceD3d::CreateTextureFromFile(pThis, pTexture, "data/global/overlay/overlaynitro.dds", flags);
	}

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

	File file;
	file.Load(path.c_str(), 9);

	uint32_t dataSize = file.pFileCodec->GetFileSize();

	auto data = new uint8_t[dataSize];
	file.pFileCodec->ReadBytes(data, dataSize);
	if (CreateCustomTexture(pTexture, data, dataSize, flags)) {
		delete[] data;
		return pTexture;
	}
	delete[] data;
	return nullptr;
}

void ApplyDDSParserPatches() {
	NyaHookLib::Patch(0x667A0C, &CreateTextureFromFileNew);
	NyaHookLib::Patch(0x667A10, &CreateTextureFromMemoryNew);
}