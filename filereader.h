uint8_t* ReadFileFromBfs(const char* _path, size_t& outSize) {
	if (!pBfsManager) {
		MessageBoxA(0, std::format("Tried to load {} before filesystem init.\nThis is fatal. You will die.", _path).c_str(), "Fatal error", MB_ICONERROR);
		exit(0);
	}

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

toml::table ReadTOMLFromBfs(const std::string& path) {
	size_t size = 0;
	auto file = (char*)ReadFileFromBfs(path.c_str(), size);
	if (!file || !size) return {};
	if (file[size-2] == '\r') file[size-2]=0;
	file[size-1]=0;

	std::stringstream ss;
	ss << file;
	delete[] file;

	try {
		return toml::parse(ss);
	}
	catch (const toml::parse_error& err) {
		MessageBoxA(0, std::format("Failed to parse {}: {}", path, err.what()).c_str(), "Fatal error", MB_ICONERROR);
	}
	return {};
}