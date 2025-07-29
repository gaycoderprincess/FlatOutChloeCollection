void MountBFSFiles(const char* path) {
	std::ifstream fin(path);
	if (!fin.is_open()) return;

	for (std::string line; std::getline(fin, line); ) {
		if (line.empty()) continue;
		BfsManager::AddBFSFile(line.c_str());
	}
}

bool __fastcall NewBFSList1(uint32_t flags) {
	MountBFSFiles("filesystem");
	return false;
}
bool __fastcall NewBFSList2(uint32_t flags) {
	MountBFSFiles("patch");
	return false;
}

void ApplyBFSLoadingPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A7014, &NewBFSList1);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A70DF, &NewBFSList2);
}