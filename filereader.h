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

toml::table ReadTOMLFromBfs(const std::string& path) {
	size_t size = 0;
	auto file = (char*)ReadFileFromBfs(path.c_str(), size);
	if (!file || !size) return {};

	auto nulledFile = new char[size+1];
	memcpy(nulledFile, file, size);
	nulledFile[size] = 0;
	delete[] file;

	std::stringstream ss;
	ss << nulledFile;
	delete[] nulledFile;

	try {
		return toml::parse(ss);
	}
	catch (const toml::parse_error& err) {
		MessageBoxA(0, std::format("Failed to parse {}: {}", path, err.what()).c_str(), "Fatal error", MB_ICONERROR);
	}
	return {};
}

// simple hacks for simple lua files to load as toml, e.g. resetmap.bed
toml::table ReadTOMLFromBfsLUAHack(const std::string& path) {
	size_t size = 0;
	auto file = (char*)ReadFileFromBfs(path.c_str(), size);
	if (!file || !size) return {};

	for (int i = 0; i < size; i++) {
		// array brackets
		if (file[i] == '{') file[i] = '[';
		if (file[i] == '}') file[i] = ']';

		// comments
		if (i+1 < size && file[i] == '-' && file[i+1] == '-') {
			file[i] = '#';
			file[i+1] = '#';
		}
	}

	auto nulledFile = new char[size+1];
	memcpy(nulledFile, file, size);
	nulledFile[size] = 0;
	delete[] file;

	std::stringstream ss;
	ss << nulledFile;
	delete[] nulledFile;

	try {
		return toml::parse(ss);
	}
	catch (const toml::parse_error& err) {
		MessageBoxA(0, std::format("Failed to parse {}: {}", path, err.what()).c_str(), "Fatal error", MB_ICONERROR);
	}
	return {};
}