namespace NewMusicPlayer {
	void SetMenuSongNames(const wchar_t* artist, const wchar_t* title) {
		NyaHookLib::Patch(0x464D2A + 1, artist);
		NyaHookLib::Patch(0x464D7E + 1, title);
	}

	int GetMusicVolume() {
		if (GetGameState() == GAME_STATE_RACE) {
			return nIngameMusicVolume;
		}
		return nInterfaceMusicVolume;
	}

	struct tSong {
		std::string sPath;
		std::string sArtist;
		std::string sTitle;
		std::wstring wsArtist;
		std::wstring wsTitle;
		NyaAudio::NyaSound pStream = 0;
		int nStreamVolume = 0;
		bool bFinished = false;
		bool bAlreadyPlayed = false;

		void Load() {
			size_t size;

			auto data = (char*)ReadFileFromBfs(sPath.c_str(), size);
			if (!data) return;

			pStream = NyaAudio::LoadMemory(data, size);
		}

		void Play() {
			nStreamVolume = GetMusicVolume();
			bFinished = true;

			if (pStream) {
				bFinished = false;
				NyaAudio::SetVolume(pStream, nStreamVolume / 100.0);
				NyaAudio::SkipTo(pStream, 0);
				NyaAudio::Play(pStream);
				bAlreadyPlayed = true;

				if (GetGameState() == GAME_STATE_MENU) {
					pGameFlow->pMenuInterface->bMusicPopupFinished = false;
					pGameFlow->pMenuInterface->bMusicPopupActive = true;
					pGameFlow->pMenuInterface->nMusicPopupTimer = pGameFlow->pMenuInterface->fMenuTimer * 1000;
				}

				SetMenuSongNames(wsArtist.c_str(), wsTitle.c_str());
			}
		}

		void Stop() {
			if (pStream) {
				NyaAudio::Stop(pStream);
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
		std::string sName;
		std::vector<tSong> aSongs;

		tSong* GetNextSong() {
			std::vector<tSong*> songs;
			for (auto& song : aSongs) {
				if (song.bAlreadyPlayed) continue;
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
	};
	std::vector<tPlaylist> aPlaylists;

	tPlaylist* pPlaylistMenu = nullptr;
	tPlaylist* pPlaylistIngame = nullptr;

	tPlaylist* pCurrentPlaylist = nullptr;
	tSong* pCurrentSong = nullptr;
	tSong* pLastSong = nullptr;

	void StopPlayback() {
		if (!pCurrentSong) return;
		pCurrentSong->Stop();
		pCurrentSong = nullptr;
	}

	void OnTick() {
		if (!pPlaylistIngame || !pPlaylistMenu) return;
		if (GetGameState() == GAME_STATE_NONE) return;

		pCurrentPlaylist = GetGameState() == GAME_STATE_RACE ? pPlaylistIngame : pPlaylistMenu;

		if (pLoadingScreen || IsKeyJustPressed(VK_END)) {
			StopPlayback();
			return;
		}

		if (GetGameState() == GAME_STATE_MENU) {
			nMusicPopupTimeOffset = 0;
		}

		if (pCurrentSong) {
			pCurrentSong->Update();
			if (pCurrentSong->bFinished) {
				pCurrentSong = nullptr;
			}
		}
		if (!pCurrentSong) {
			pCurrentSong = pCurrentPlaylist->GetNextSong();
			while (pCurrentPlaylist->aSongs.size() > 1 && pCurrentSong == pLastSong) {
				pCurrentSong = pCurrentPlaylist->GetNextSong();
			}
			pCurrentSong->Play();
			pLastSong = pCurrentSong;

			if (pPlayerHost) {
				nMusicPopupTimeOffset = pPlayerHost->nRaceTime;
			}
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

	tPlaylist LoadPlaylist(const char* path = "data/music/playlist_0.toml") {
		size_t size;
		auto file = (char*)ReadFileFromBfs(path, size);

		// this is fucking stupid, but tomlplusplus crashes trying to read from memory
		std::ofstream outFile("cctemp_playlist.toml", std::ios::out | std::ios::binary );
		if (!outFile.is_open()) return {};
		outFile.write(file, size);
		outFile.close();
		delete[] file;

		try {
			tPlaylist playlist;
			auto config = toml::parse_file("cctemp_playlist.toml");
			playlist.sName = config["Playlist"]["Name"].value_or("");
			int count = config["Playlist"]["Count"].value_or(0);
			for (int i = 0; i < count; i++) {
				tSong song;
				auto name = std::format("Song{}", i+1);
				song.sPath = config["Playlist"][name]["File"].value_or("");
				song.sArtist = config["Playlist"][name]["Artist"].value_or("");
				song.sTitle = config["Playlist"][name]["Song"].value_or("");
				song.wsArtist = config["Playlist"][name]["Artist"].value_or(L"");
				song.wsTitle = config["Playlist"][name]["Song"].value_or(L"");
				if (song.sPath.empty()) continue;
				if (song.sArtist.empty()) continue;
				if (song.sTitle.empty()) continue;
				playlist.aSongs.push_back(song);
			}
			remove("cctemp_playlist.toml");
			return playlist;
		}
		catch (const toml::parse_error& err) {
			MessageBoxA(0, std::format("Parsing failed: {}", err.what()).c_str(), "Fatal error", MB_ICONERROR);
		}
		remove("cctemp_playlist.toml");

		return {};
	}

	void Init() {
		static bool bInited = false;
		if (bInited) {
			return;
		}
		bInited = true;

		NyaAudio::Init(ghWnd);

		NyaHookLib::Patch<uint8_t>(0x410CB0, 0xC3);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x411210, &GetArtistName);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x411260, &GetSongName);

		aPlaylists.push_back(LoadPlaylist("data/music/playlist_title.toml"));
		pPlaylistMenu = &aPlaylists[aPlaylists.size()-1];
		aPlaylists.push_back(LoadPlaylist("data/music/playlist_ingame.toml"));
		pPlaylistIngame = &aPlaylists[aPlaylists.size()-1];

		for (auto& playlist : aPlaylists) {
			for (auto& song : playlist.aSongs) {
				song.Load();
			}
		}
	}
}