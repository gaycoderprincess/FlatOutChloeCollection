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

		void Play() {
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
				bAlreadyPlayed = true;

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
			if (song.bDisabled) continue;
			WriteStringToFile(file, song.sPath.c_str());
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
	}

	void SaveCustomPlaylists(int saveSlot) {
		if (!bCustomPlaylistsEnabled) return;

		SavePlaylist("Savegame/playlist_title.sav", pPlaylistCustomTitle);
		SavePlaylist("Savegame/playlist_ingame.sav", pPlaylistCustomIngame);
		SavePlaylist("Savegame/playlist_derby.sav", pPlaylistCustomDerby);
	}

	void LoadCustomPlaylists() {
		LoadPlaylist("Savegame/playlist_title.sav", pPlaylistCustomTitle);
		LoadPlaylist("Savegame/playlist_ingame.sav", pPlaylistCustomIngame);
		LoadPlaylist("Savegame/playlist_derby.sav", pPlaylistCustomDerby);
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

		if (IsKeyJustPressed(VK_END) || IsPadKeyJustPressed(NYA_PAD_KEY_SELECT)) {
			StopPlayback();
		}

		if (GetGameState() == GAME_STATE_MENU) {
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
			pCurrentSong->Play();
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
		auto config = ReadTOMLFromBfs(std::format("data/music/{}.toml", fileName).c_str());
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

	void BuildCustomPlaylist(tPlaylist* customPlaylist, const std::vector<tPlaylist>& playlists) {
		for (auto& playlist : playlists) {
			if (&playlist == customPlaylist) continue;

			for (auto& song : playlist.aSongs) {
				bool found = false;
				for (auto& customSong : customPlaylist->aSongs) {
					if (customSong.sPath == song.sPath) found = true;
				}
				if (found) continue;
				customPlaylist->aSongs.push_back(song);
			}
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

		tPlaylist custom;
		custom.wsName = L"CHLOE TRAX";
		aPlaylistsTitle.push_back(custom);
		aPlaylistsIngame.push_back(custom);

		pPlaylistCustomTitle = &aPlaylistsTitle[aPlaylistsTitle.size()-1];
		pPlaylistCustomIngame = &aPlaylistsIngame[aPlaylistsIngame.size()-1];
		pPlaylistCustomDerby = new tPlaylist;
		*pPlaylistCustomDerby = custom;

		BuildCustomPlaylist(pPlaylistCustomTitle, aPlaylistsTitle);
		BuildCustomPlaylist(pPlaylistCustomTitle, aPlaylistsIngame);
		BuildCustomPlaylist(pPlaylistCustomIngame, aPlaylistsTitle);
		BuildCustomPlaylist(pPlaylistCustomIngame, aPlaylistsIngame);
		BuildCustomPlaylist(pPlaylistCustomDerby, aPlaylistsTitle);
		BuildCustomPlaylist(pPlaylistCustomDerby, aPlaylistsIngame);
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

	void Init() {
		static bool bInited = false;
		if (bInited) {
			return;
		}
		bInited = true;

		NyaAudio::Init(ghWnd);
		ChloeEvents::DrawAboveUIEvent.AddHandler(OnTick);
		ChloeEvents::RaceRestartEvent.AddHandler(OnRaceRestart);
		ChloeEvents::SaveCreatedEvent.AddHandler(SaveCustomPlaylists);

		// todo credits song

		// get rid of all vanilla playlist stuff
		NyaHookLib::Patch<uint8_t>(0x410CB0, 0xC3); // StartMusic
		NyaHookLib::Patch<uint8_t>(0x4112B0, 0xC3); // PickGameSongAndStartMenuSong
		NyaHookLib::Patch<uint8_t>(0x411350, 0xC3); // StartSelectedSongFromMusicPlaylist
		NyaHookLib::Patch<uint8_t>(0x410ED0, 0xC3); // ReadMusicPlaylist

		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x411210, &GetArtistName);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x411260, &GetSongName);

		WriteLogDebug("MUSIC", "--- Discovering playlists ---");

		LoadPlaylistConfig();

		WriteLogDebug("MUSIC", "--- Finished discovering playlists ---");

		WriteLogDebug("MUSIC", "--- Preloading playlists ---");

		for (auto& playlist : aPlaylistsTitle) { playlist.Load(); }
		for (auto& playlist : aPlaylistsIngame) { playlist.Load(); }
		for (auto& playlist : aPlaylistsStunt) { playlist.Load(); }
		pPlaylistCustomDerby->Load();

		WriteLogDebug("MUSIC", "--- Finished preloading playlists ---");
	}
}