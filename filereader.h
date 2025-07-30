uint8_t* ReadFileFromBfs(const char* _path, size_t& outSize) {
	std::string path = _path;
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

	uint32_t dataSize = file.pFileCodec->GetFileSize();

	auto data = new uint8_t[dataSize];
	file.pFileCodec->ReadBytes(data, dataSize);
	outSize = dataSize;
	return data;
}