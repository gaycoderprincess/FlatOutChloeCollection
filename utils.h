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

int GetWreckingDerbyCrashScore(int player) {
	int score = 0;
	score += aCrashBonusesReceived[player][CRASHBONUS_SLAM] * 1;
	score += aCrashBonusesReceived[player][CRASHBONUS_POWERHIT] * 2;
	score += aCrashBonusesReceived[player][CRASHBONUS_BLASTOUT] * 3;
	return score;
}

int GetWreckingDerbyWreckScore(int player) {
	int score = 0;
	score += aCrashBonusesReceived[player][CRASHBONUS_WRECKED] * 5;
	return score;
}

int GetWreckingDerbyBonusScore(int position, bool finished) {
	if (!finished) {
		int playersAlive = 0;
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			if (!GetPlayer(i)->nIsRagdolled) playersAlive++;
		}

		if (playersAlive == 1) { return 15; }
		if (playersAlive == 2) { return 10; }
		if (playersAlive == 3) { return 5; }
		return 0;
	}

	if (position == 1) { return 15; }
	if (position == 2) { return 10; }
	if (position == 3) { return 5; }
	return 0;
}

int GetWreckingDerbyTotalScore(int player, int position, bool finished) {
	return GetWreckingDerbyCrashScore(player) + GetWreckingDerbyWreckScore(player) + GetWreckingDerbyBonusScore(position, finished);
}

std::vector<PlayerScoreRace*> GetSortedPlayerScores() {
	std::vector<PlayerScoreRace*> aScores;
	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		aScores.push_back(GetScoreManager()->aScores[i]);
	}
	if (bIsFragDerby) {
		std::sort(aScores.begin(), aScores.end(), [](PlayerScoreRace *a, PlayerScoreRace *b) {
			return FragDerby::nPlayerScore[a->nPlayerId] > FragDerby::nPlayerScore[b->nPlayerId];
		});
	}
	else if (bIsWreckingDerby) {
		std::sort(aScores.begin(), aScores.end(), [](PlayerScoreRace *a, PlayerScoreRace *b) {
			return GetWreckingDerbyTotalScore(a->nPlayerId, a->nPosition, a->bHasFinished || a->bIsDNF) > GetWreckingDerbyTotalScore(b->nPlayerId, b->nPosition, b->bHasFinished || b->bIsDNF);
		});
	}
	else {
		std::sort(aScores.begin(), aScores.end(), [](PlayerScoreRace *a, PlayerScoreRace *b) {
			if (a->bIsDNF && !b->bIsDNF) { return false; }
			if (b->bIsDNF && !a->bIsDNF) { return true; }
			return a->nPosition < b->nPosition;
		});
	}
	return aScores;
}