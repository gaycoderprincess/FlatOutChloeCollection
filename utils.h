void WriteLog(const std::string& str) {
	static auto file = std::ofstream("FlatOutChloeCollection_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

bool bDebugLog = false;
void WriteLogDebug(const std::string& title, const std::string& str) {
	if (!bDebugLog) return;

	static auto file = std::ofstream("FlatOutChloeCollection_gcp_debug.log");

	file << "[" + title + "] ";
	file << str;
	file << "\n";
	file.flush();
}

auto GetStringWide(const std::string& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(string);
}

auto GetStringNarrow(const std::wstring& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(string);
}

std::string GetCustomSavePath(int id) {
	return std::format("Savegame/customsave{:03}.sav", id);
}

std::string GetAchievementSavePath(int id) {
	return std::format("Savegame/customsave{:03}.ach", id);
}

std::string FormatScore(int a1) {
	if (a1 < 1000) {
		return std::to_string(a1);
	}
	auto v4 = a1 / 1000;
	if (a1 >= 1000000) {
		return std::format("{},{:03},{:03}", a1 / 1000000, v4 % 1000, a1 % 1000);
	}
	return std::format("{},{:03}", v4, a1 % 1000);
}

std::string FormatGameTime(int ms, bool leadingZero = true) {
	if (ms < 0) ms = 0;
	std::string str = GetTimeFromMilliseconds(ms, true);
	str.pop_back(); // remove trailing zero
	// add leading zero
	if (leadingZero && ms < 60 * 10 * 1000) {
		str = "0" + str;
	}
	return str;
}

int32_t GetPlayerLapTime(Player* ply, int lapId) {
	if (lapId > ply->nCurrentLap) return 0;

	auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
	auto lap = score->nLapTimes[lapId];
	if (lap <= 0) return 0;
	if (lapId > 0) lap -= score->nLapTimes[lapId - 1];
	return lap;
}