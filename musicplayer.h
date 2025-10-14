namespace NewMusicPlayer {
	void SetMenuSongNames(const wchar_t* artist, const wchar_t* title) {
		NyaHookLib::Patch(0x464D2A + 1, artist);
		NyaHookLib::Patch(0x464D7E + 1, title);
	}

	int GetMusicVolume() {
		if (GetGameState() == GAME_STATE_MENU) {
			return nInterfaceMusicVolume;
		}
		return nIngameMusicVolume;
	}

	bool bCustomPlaylistsEnabled = false;
	bool bLastSongPlayedInReplay = false;
	bool bIsFirstIngameSong = false;
	bool bIsInCredits = false;

	struct tSong {
		std::string sPath;
		std::string sArtist;
		std::string sTitle;
		std::wstring wsArtist;
		std::wstring wsTitle;
		uint32_t nStartPos;
		NyaAudio::NyaSound pStream = 0;
		int nStreamVolume = 0;
		bool bFinished = false;
		bool bAlreadyPlayed = false;
		bool bIsFromFile = false;
		bool bDisabled = false;

		void Load(bool loadStreams) {
			WriteLogDebug("MUSIC", std::format("Loading song {} - {}", sArtist, sTitle));

			// file exists in filesystem, use streaming
			if (std::filesystem::exists(sPath)) {
				WriteLogDebug("MUSIC", "Song found in loose files, setting up streaming");
				if (loadStreams) {
					WriteLogDebug("MUSIC", std::format("Initializing streaming for {}", sPath));
					pStream = NyaAudio::LoadFileStreamed(sPath.c_str());
				}
				bIsFromFile = true;
			}
			else {
				WriteLogDebug("MUSIC", "Song not found in loose files, searching for fallback in BFS");

				bIsFromFile = false;

				size_t size;
				auto data = (char*)ReadFileFromBfs(sPath.c_str(), size);
				if (!data) return;

				WriteLogDebug("MUSIC", std::format("Song found in BFS, preloading {} bytes of data", size));
				pStream = NyaAudio::LoadMemory(data, size);
			}
		}

		void Play(bool setAsPlayed) {
			nStreamVolume = GetMusicVolume();
			bFinished = true;

			if (bIsFromFile && !pStream) {
				Load(true);
			}

			if (pStream) {
				bFinished = false;
				NyaAudio::SetVolume(pStream, nStreamVolume / 100.0);
				if (!bIsFromFile) NyaAudio::SkipTo(pStream, 0);
				NyaAudio::Play(pStream);
				if (bIsFirstIngameSong && nStartPos > 0) {
					NyaAudio::SkipTo(pStream, nStartPos, true);
				}
				bIsFirstIngameSong = false;
				if (setAsPlayed) bAlreadyPlayed = true;

				if (GetGameState() == GAME_STATE_MENU) {
					pGameFlow->pMenuInterface->bMusicPopupFinished = false;
					pGameFlow->pMenuInterface->bMusicPopupActive = true;
					pGameFlow->pMenuInterface->nMusicPopupTimer = pGameFlow->pMenuInterface->fMenuTimer * 1000;
				}

				SetMenuSongNames(wsArtist.c_str(), wsTitle.c_str());

				if (pPlayerHost) {
					nMusicPopupTimeOffset = pPlayerHost->nRaceTime;
					bLastSongPlayedInReplay = GetGameState() == GAME_STATE_REPLAY;
				}
				else {
					nMusicPopupTimeOffset = -3000;
				}
			}
		}

		void Stop() {
			if (pStream) {
				NyaAudio::Stop(pStream);
				if (bIsFromFile) {
					NyaAudio::Delete(&pStream);
				}
			}
			bFinished = true;
		}

		void Update() {
			if (nStreamVolume != GetMusicVolume()) {
				nStreamVolume = GetMusicVolume();
				NyaAudio::SetVolume(pStream, nStreamVolume / 100.0);
			}

			if (!bFinished && NyaAudio::IsFinishedPlaying(pStream)) {
				bFinished = true;
			}
		}
	};

	struct tPlaylist {
		std::wstring wsName;
		std::vector<tSong> aSongs;

		int GetNumSongsEnabled() {
			int count = 0;
			for (auto& song : aSongs) {
				if (song.bDisabled) continue;
				count++;
			}
			return count;
		}

		bool IsEmpty() {
			return GetNumSongsEnabled() <= 0;
		}

		tSong* GetNextSong() {
			if (IsEmpty()) return nullptr;

			std::vector<tSong*> songs;
			for (auto& song : aSongs) {
				if (song.bAlreadyPlayed) continue;
				if (song.bDisabled) continue;
				songs.push_back(&song);
			}

			if (songs.empty()) {
				for (auto& song : aSongs) {
					song.bAlreadyPlayed = false;
				}
				return GetNextSong();
			}

			return songs[rand()%songs.size()];
		}

		void Load() {
			WriteLogDebug("MUSIC", std::format("Preloading playlist {}", GetStringNarrow(wsName.c_str())));

			for (auto& song : aSongs) {
				song.Load(false);
			}
		}
	};
	std::vector<tPlaylist> aPlaylistsTitle;
	std::vector<tPlaylist> aPlaylistsIngame;
	std::vector<tPlaylist> aPlaylistsStunt;
	tPlaylist gPlaylistCredits;
	tPlaylist gTraxExtraSongs;

	tPlaylist* pPlaylistTitle = nullptr;
	tPlaylist* pPlaylistIngame = nullptr;
	tPlaylist* pPlaylistDerby = nullptr;
	tPlaylist* pPlaylistStunt = nullptr;

	tPlaylist* pPlaylistCustomTitle = nullptr;
	tPlaylist* pPlaylistCustomIngame = nullptr;
	tPlaylist* pPlaylistCustomDerby = nullptr;

	tPlaylist* pCurrentPlaylist = nullptr;
	tSong* pCurrentSong = nullptr;
	tSong* pLastSong = nullptr;

	void StopPlayback() {
		if (!pCurrentSong) return;
		pCurrentSong->Stop();
		pCurrentSong = nullptr;
	}

	void SavePlaylist(const char* path, tPlaylist* playlist) {
		auto file = std::ofstream(path, std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		for (auto& song : playlist->aSongs) {
			WriteStringToFile(file, song.sPath.c_str());
			file.write((char*)&song.bDisabled, sizeof(song.bDisabled));
		}
	}

	void LoadPlaylist(const char* path, tPlaylist* playlist) {
		auto file = std::ifstream(path, std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		bCustomPlaylistsEnabled = true;
		for (auto& song : playlist->aSongs) {
			song.bDisabled = true;
		}

		auto songToEnable = ReadStringFromFile(file);
		while (!songToEnable.empty()) {
			for (auto& song : playlist->aSongs) {
				if (song.sPath == songToEnable) song.bDisabled = false;
			}
			songToEnable = ReadStringFromFile(file);
		}

		// convert to new format
		file.close();
		std::filesystem::remove(path);
		SavePlaylist(std::format("{}2", path).c_str(), playlist);
	}

	void LoadPlaylistNew(const char* path, tPlaylist* playlist) {
		auto file = std::ifstream(path, std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		bCustomPlaylistsEnabled = true;

		auto songName = ReadStringFromFile(file);
		bool disabled = false;
		file.read((char*)&disabled, sizeof(disabled));
		while (!songName.empty()) {
			for (auto& song : playlist->aSongs) {
				if (song.sPath == songName) song.bDisabled = disabled;
			}
			songName = ReadStringFromFile(file);
			file.read((char*)&disabled, sizeof(disabled));
		}
	}

	void SaveCustomPlaylists(int saveSlot) {
		if (!bCustomPlaylistsEnabled) return;

		SavePlaylist("Savegame/playlist_title.sav2", pPlaylistCustomTitle);
		SavePlaylist("Savegame/playlist_ingame.sav2", pPlaylistCustomIngame);
		SavePlaylist("Savegame/playlist_derby.sav2", pPlaylistCustomDerby);
	}

	void LoadCustomPlaylists() {
		LoadPlaylist("Savegame/playlist_title.sav", pPlaylistCustomTitle);
		LoadPlaylist("Savegame/playlist_ingame.sav", pPlaylistCustomIngame);
		LoadPlaylist("Savegame/playlist_derby.sav", pPlaylistCustomDerby);
		LoadPlaylistNew("Savegame/playlist_title.sav2", pPlaylistCustomTitle);
		LoadPlaylistNew("Savegame/playlist_ingame.sav2", pPlaylistCustomIngame);
		LoadPlaylistNew("Savegame/playlist_derby.sav2", pPlaylistCustomDerby);
	}

	void OnTick() {
		static CNyaTimer gTimer;
		gTimer.Process();

		if (GetGameState() == GAME_STATE_NONE) return;

		if (nMenuSoundtrack < 0 || nMenuSoundtrack >= aPlaylistsTitle.size()) nMenuSoundtrack = 0;
		if (nIngameStuntSoundtrack < 0 || nIngameStuntSoundtrack >= aPlaylistsStunt.size()) nIngameStuntSoundtrack = 0;
		if (nIngameSoundtrack < 0 || nIngameSoundtrack >= aPlaylistsIngame.size()) nIngameSoundtrack = 0;
		if (nIngameDerbySoundtrack < 0 || nIngameDerbySoundtrack >= aPlaylistsIngame.size()) nIngameDerbySoundtrack = 0;
		// on change playlist in menu
		if (pPlaylistTitle != &aPlaylistsTitle[nMenuSoundtrack]) {
			StopPlayback();
			pPlaylistTitle = &aPlaylistsTitle[nMenuSoundtrack];
		}
		pPlaylistIngame = &aPlaylistsIngame[nIngameSoundtrack];
		pPlaylistDerby = &aPlaylistsIngame[nIngameDerbySoundtrack];
		pPlaylistStunt = &aPlaylistsStunt[nIngameStuntSoundtrack];

		// hack for separate chloe trax for derbies
		if (pPlaylistDerby == pPlaylistCustomIngame) {
			pPlaylistDerby = pPlaylistCustomDerby;
		}

		if (pLoadingScreen) {
			StopPlayback();
			return;
		}

		bool isSkipMusicPressed = IsKeyJustPressed(VK_END);
		auto skipMusicPadKey = GetNyaControllerKeyForAction(CONFIG_SKIPMUSIC);
		// prevent overlap with menu navigation if skip music is on dpad
		if (skipMusicPadKey == NYA_PAD_KEY_DPAD_UP || skipMusicPadKey == NYA_PAD_KEY_DPAD_DOWN || skipMusicPadKey == NYA_PAD_KEY_DPAD_LEFT || skipMusicPadKey == NYA_PAD_KEY_DPAD_RIGHT) {
			if (GetGameState() == GAME_STATE_RACE && !pGameFlow->nIsPaused && !GetScoreManager()->nHideRaceHUD && IsPadKeyJustPressed(skipMusicPadKey)) isSkipMusicPressed = true;
		}
		else if (IsPadKeyJustPressed(skipMusicPadKey)) isSkipMusicPressed = true;

		if (isSkipMusicPressed) {
			StopPlayback();
		}

		if (bIsInCredits) pCurrentPlaylist = &gPlaylistCredits;
		else if (GetGameState() == GAME_STATE_MENU) {
			pCurrentPlaylist = pPlaylistTitle;
			bIsFirstIngameSong = true;
		}
		else {
			if (pGameFlow->nEventType == eEventType::STUNT) pCurrentPlaylist = pPlaylistStunt;
			else if (pGameFlow->nEventType == eEventType::DERBY) pCurrentPlaylist = pPlaylistDerby;
			else pCurrentPlaylist = pPlaylistIngame;
		}

		if (!pCurrentPlaylist || pCurrentPlaylist->IsEmpty()) return;

		if (GetGameState() == GAME_STATE_MENU) {
			nMusicPopupTimeOffset = 0;
		}

		if (pCurrentSong) {
			pCurrentSong->Update();

			// hide dummy stunt song names
			if (pCurrentSong->sTitle == "Henri") {
				nMusicPopupTimeOffset = -15000;
			}

			// remove duplicate music popup during replays
			if ((GetGameState() == GAME_STATE_REPLAY) != bLastSongPlayedInReplay) {
				nMusicPopupTimeOffset = -15000;
			}

			if (GetGameState() != GAME_STATE_MENU && (pGameFlow->nIsPaused || pPlayerHost->nRaceTime == -3000)) {
				nMusicPopupTimeOffset -= gTimer.fDeltaTime * 1000;
			}

			if (pCurrentSong->bFinished) {
				pCurrentSong->Stop();
				pCurrentSong = nullptr;
			}
		}
		if (!pCurrentSong && GetMusicVolume() > 0) {
			pCurrentSong = pCurrentPlaylist->GetNextSong();
			while (pCurrentPlaylist->GetNumSongsEnabled() > 1 && pCurrentSong == pLastSong) {
				pCurrentSong = pCurrentPlaylist->GetNextSong();
			}
			pCurrentSong->Play(true);
			pLastSong = pCurrentSong;
		}
	}

	const char* GetArtistName() {
		if (!pCurrentSong) return "NULL";
		return pCurrentSong->sArtist.c_str();
	}

	const char* GetSongName() {
		if (!pCurrentSong) return "NULL";
		return pCurrentSong->sTitle.c_str();
	}

	bool LoadPlaylist(tPlaylist* out, const char* fileName) {
		auto path = std::format("data/music/{}.toml", fileName);
		if (!DoesFileExist(path.c_str())) return false;

		auto config = ReadTOMLFromBfs(path.c_str());
		int count = config["Playlist"]["Count"].value_or(0);
		for (int i = 0; i < count; i++) {
			tSong song;
			auto name = std::format("Song{}", i+1);
			song.sPath = config["Playlist"][name]["File"].value_or("");
			song.sArtist = config["Playlist"][name]["Artist"].value_or("");
			song.sTitle = config["Playlist"][name]["Song"].value_or("");
			song.wsArtist = config["Playlist"][name]["Artist"].value_or(L"");
			song.wsTitle = config["Playlist"][name]["Song"].value_or(L"");
			song.nStartPos = config["Playlist"][name]["StartPos"].value_or(0);
			if (song.sPath.empty()) continue;
			if (song.sArtist.empty()) continue;
			if (song.sTitle.empty()) continue;
			std::transform(song.sPath.begin(), song.sPath.end(), song.sPath.begin(), [](wchar_t c){ return std::tolower(c); });
			out->aSongs.push_back(song);
		}
		return !out->aSongs.empty();
	}

	void BuildCustomPlaylist(tPlaylist* customPlaylist, const tPlaylist& playlist, bool enabledByDefault) {
		if (&playlist == customPlaylist) return;

		for (auto& song : playlist.aSongs) {
			bool found = false;
			for (auto& customSong : customPlaylist->aSongs) {
				if (customSong.sPath == song.sPath) {
					if (enabledByDefault) customSong.bDisabled = false;
					found = true;
				}
			}
			if (found) continue;
			customPlaylist->aSongs.push_back(song);
			customPlaylist->aSongs[customPlaylist->aSongs.size()-1].bDisabled = !enabledByDefault;
		}

		std::sort(customPlaylist->aSongs.begin(), customPlaylist->aSongs.end(), [] (const tSong& a, const tSong& b) {
			if (a.sArtist == b.sArtist) {
				return a.sTitle < b.sTitle;
			}
			return a.sArtist < b.sArtist;
		});
	}

	void BuildCustomPlaylist(tPlaylist* customPlaylist, const std::vector<tPlaylist>& playlists, bool enabledByDefault) {
		for (auto& playlist : playlists) {
			BuildCustomPlaylist(customPlaylist, playlist, enabledByDefault);
		}

		std::sort(customPlaylist->aSongs.begin(), customPlaylist->aSongs.end(), [] (const tSong& a, const tSong& b) {
			if (a.sArtist == b.sArtist) {
				return a.sTitle < b.sTitle;
			}
			return a.sArtist < b.sArtist;
		});
	}

	void LoadPlaylistConfig() {
		static auto config = toml::parse_file("Config/Music.toml");
		int numPlaylists = config["main"]["playlist_count"].value_or(1);
		int numMenuPlaylists = config["main"]["menuplaylist_count"].value_or(1);
		int numStuntPlaylists = config["main"]["stuntplaylist_count"].value_or(1);
		int defaultMenu = config["main"]["default_menu"].value_or(1) - 1;
		int defaultIngame = config["main"]["default_race"].value_or(1) - 1;
		int defaultDerby = config["main"]["default_derby"].value_or(1) - 1;
		int defaultStunt = config["main"]["default_stunt"].value_or(1) - 1;
		for (int i = 0; i < numPlaylists; i++) {
			tPlaylist playlist;
			playlist.wsName = config[std::format("playlist{}", i+1)]["name"].value_or(L"");
			LoadPlaylist(&playlist, config[std::format("playlist{}", i+1)]["file"].value_or(""));
			if (playlist.wsName.empty()) continue;
			if (playlist.aSongs.empty()) continue;
			WriteLogDebug("MUSIC", std::format("Adding ingame playlist {} with {} songs", GetStringNarrow(playlist.wsName.c_str()), playlist.aSongs.size()));
			aPlaylistsIngame.push_back(playlist);
		}
		for (int i = 0; i < numMenuPlaylists; i++) {
			tPlaylist playlist;
			playlist.wsName = config[std::format("menuplaylist{}", i+1)]["name"].value_or(L"");
			LoadPlaylist(&playlist, config[std::format("menuplaylist{}", i+1)]["file"].value_or(""));
			if (playlist.wsName.empty()) continue;
			if (playlist.aSongs.empty()) continue;
			WriteLogDebug("MUSIC", std::format("Adding menu playlist {} with {} songs", GetStringNarrow(playlist.wsName.c_str()), playlist.aSongs.size()));
			aPlaylistsTitle.push_back(playlist);
		}
		for (int i = 0; i < numStuntPlaylists; i++) {
			tPlaylist playlist;
			playlist.wsName = config[std::format("stuntplaylist{}", i+1)]["name"].value_or(L"");
			LoadPlaylist(&playlist, config[std::format("stuntplaylist{}", i+1)]["file"].value_or(""));
			if (playlist.wsName.empty()) continue;
			if (playlist.aSongs.empty()) continue;
			WriteLogDebug("MUSIC", std::format("Adding stunt playlist {} with {} songs", GetStringNarrow(playlist.wsName.c_str()), playlist.aSongs.size()));
			aPlaylistsStunt.push_back(playlist);
		}

		gPlaylistCredits.wsName = L"CREDITS";
		LoadPlaylist(&gPlaylistCredits, "playlist_credits");
		WriteLogDebug("MUSIC", std::format("Adding playlist {} with {} songs", GetStringNarrow(gPlaylistCredits.wsName.c_str()), gPlaylistCredits.aSongs.size()));

		gTraxExtraSongs.wsName = L"STANDALONE TRAX SONGS";
		LoadPlaylist(&gTraxExtraSongs, "playlist_trax");
		WriteLogDebug("MUSIC", std::format("Adding playlist {} with {} songs", GetStringNarrow(gTraxExtraSongs.wsName.c_str()), gTraxExtraSongs.aSongs.size()));

		tPlaylist custom;
		custom.wsName = L"CHLOE TRAX";
		aPlaylistsTitle.push_back(custom);
		aPlaylistsIngame.push_back(custom);

		pPlaylistCustomTitle = &aPlaylistsTitle[aPlaylistsTitle.size()-1];
		pPlaylistCustomIngame = &aPlaylistsIngame[aPlaylistsIngame.size()-1];
		pPlaylistCustomDerby = new tPlaylist;
		*pPlaylistCustomDerby = custom;

		BuildCustomPlaylist(pPlaylistCustomTitle, aPlaylistsTitle, true);
		BuildCustomPlaylist(pPlaylistCustomTitle, aPlaylistsIngame, false);
		BuildCustomPlaylist(pPlaylistCustomTitle, gTraxExtraSongs, true);
		BuildCustomPlaylist(pPlaylistCustomIngame, aPlaylistsTitle, false);
		BuildCustomPlaylist(pPlaylistCustomIngame, aPlaylistsIngame, true);
		BuildCustomPlaylist(pPlaylistCustomIngame, gTraxExtraSongs, false);
		pPlaylistCustomDerby->aSongs = pPlaylistCustomIngame->aSongs;
		LoadCustomPlaylists();

		if (defaultMenu < 0 || defaultMenu >= aPlaylistsTitle.size()) defaultMenu = 0;
		if (defaultStunt < 0 || defaultStunt >= aPlaylistsStunt.size()) defaultStunt = 0;
		if (defaultIngame < 0 || defaultIngame >= aPlaylistsIngame.size()) defaultIngame = 0;
		if (defaultDerby < 0 || defaultDerby >= aPlaylistsIngame.size()) defaultDerby = 0;
		nMenuSoundtrack = defaultMenu;
		nIngameSoundtrack = defaultIngame;
		nIngameDerbySoundtrack = defaultDerby;
		nIngameStuntSoundtrack = defaultStunt;

		for (auto& setting : aNewGameSettings) {
			if (setting.value == &nMenuSoundtrack) setting.maxValue = aPlaylistsTitle.size()-1;
			if (setting.value == &nIngameSoundtrack) setting.maxValue = aPlaylistsIngame.size()-1;
			if (setting.value == &nIngameDerbySoundtrack) setting.maxValue = aPlaylistsIngame.size()-1;
			if (setting.value == &nIngameStuntSoundtrack) setting.maxValue = aPlaylistsStunt.size()-1;
		}
	}

	void OnRaceRestart() {
		nMusicPopupTimeOffset -= pPlayerHost->nRaceTime;
		nMusicPopupTimeOffset -= 3000;
	}

	void InitPlaylists() {
		static bool bInited = false;
		if (bInited) {
			return;
		}
		bInited = true;

		NyaAudio::Init(ghWnd);

		WriteLogDebug("MUSIC", "--- Discovering playlists ---");

		LoadPlaylistConfig();

		WriteLogDebug("MUSIC", "--- Finished discovering playlists ---");

		WriteLogDebug("MUSIC", "--- Preloading playlists ---");

		for (auto& playlist : aPlaylistsTitle) { playlist.Load(); }
		for (auto& playlist : aPlaylistsIngame) { playlist.Load(); }
		for (auto& playlist : aPlaylistsStunt) { playlist.Load(); }
		gPlaylistCredits.Load();
		gTraxExtraSongs.Load();
		pPlaylistCustomDerby->Load();

		WriteLogDebug("MUSIC", "--- Finished preloading playlists ---");
	}

	ChloeHook Init([]() {
		ChloeEvents::FilesystemInitEvent.AddHandler(InitPlaylists);
		ChloeEvents::DrawAboveUIEvent.AddHandler(OnTick);
		ChloeEvents::RaceRestartEvent.AddHandler(OnRaceRestart);
		ChloeEvents::SaveCreatedEvent.AddHandler(SaveCustomPlaylists);

		// get rid of all vanilla playlist stuff
		NyaHookLib::Patch<uint8_t>(0x410CB0, 0xC3); // StartMusic
		NyaHookLib::Patch<uint8_t>(0x4112B0, 0xC3); // PickGameSongAndStartMenuSong
		NyaHookLib::Patch<uint8_t>(0x411350, 0xC3); // StartSelectedSongFromMusicPlaylist
		NyaHookLib::Patch<uint8_t>(0x410ED0, 0xC3); // ReadMusicPlaylist

		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x411210, &GetArtistName);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x411260, &GetSongName);
	});
}