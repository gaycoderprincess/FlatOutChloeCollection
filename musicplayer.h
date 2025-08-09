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
		bool bIsFromFile = false;

		void Load() {
			// file exists in filesystem, use streaming
			if (std::filesystem::exists(sPath)) {
				pStream = NyaAudio::LoadFileStreamed(sPath.c_str());
				bIsFromFile = true;
			}
			else {
				bIsFromFile = false;

				size_t size;
				auto data = (char*)ReadFileFromBfs(sPath.c_str(), size);
				if (!data) return;

				pStream = NyaAudio::LoadMemory(data, size);
			}
		}

		void Play() {
			nStreamVolume = GetMusicVolume();
			bFinished = true;

			if (bIsFromFile && !pStream) {
				Load();
			}

			if (pStream) {
				bFinished = false;
				NyaAudio::SetVolume(pStream, nStreamVolume / 100.0);
				if (!bIsFromFile) NyaAudio::SkipTo(pStream, 0);
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
		std::string sName;
		std::vector<tSong> aSongs;

		tSong* GetNextSong() {
			if (aSongs.empty()) return nullptr;

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
	tPlaylist* pPlaylistStunt = nullptr;

	tPlaylist* pCurrentPlaylist = nullptr;
	tSong* pCurrentSong = nullptr;
	tSong* pLastSong = nullptr;

	void StopPlayback() {
		if (!pCurrentSong) return;
		pCurrentSong->Stop();
		pCurrentSong = nullptr;
	}

	void OnTick() {
		if (GetGameState() == GAME_STATE_NONE) return;

		if (pLoadingScreen || IsKeyJustPressed(VK_END)) {
			StopPlayback();
			return;
		}

		if (GetGameState() == GAME_STATE_MENU) {
			pCurrentPlaylist = pPlaylistMenu;
		}
		else {
			if (pGameFlow->nEventType == eEventType::STUNT) {
				pCurrentPlaylist = pPlaylistStunt;
			}
			else {
				pCurrentPlaylist = pPlaylistIngame;
			}
		}

		if (!pCurrentPlaylist || pCurrentPlaylist->aSongs.empty()) return;

		if (GetGameState() == GAME_STATE_MENU) {
			nMusicPopupTimeOffset = 0;
		}

		if (pCurrentSong) {
			pCurrentSong->Update();

			// remove duplicate music popup after a race restart
			if (pPlayerHost && pPlayerHost->nRaceTime < 0 && nMusicPopupTimeOffset > 0) {
				nMusicPopupTimeOffset = -15000;
			}

			if (pCurrentSong->bFinished) {
				pCurrentSong->Stop();
				pCurrentSong = nullptr;
			}
		}
		if (!pCurrentSong && GetMusicVolume() > 0) {
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

	tPlaylist LoadPlaylist(const char* path) {
		size_t size;

		auto file = (char*)ReadFileFromBfs(path, size);
		if (!file) return {};

		std::stringstream ss;
		ss << file;
		delete[] file;

		try {
			tPlaylist playlist;
			auto config = toml::parse(ss);
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
			return playlist;
		}
		catch (const toml::parse_error& err) {
			MessageBoxA(0, std::format("Failed to parse {}: {}", path, err.what()).c_str(), "Fatal error", MB_ICONERROR);
		}

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
		aPlaylists.push_back(LoadPlaylist("data/music/playlist_ingamemodern.toml"));
		aPlaylists.push_back(LoadPlaylist("data/music/playlist_ingame.toml"));
		pPlaylistMenu = &aPlaylists[aPlaylists.size()-3];
		pPlaylistIngame = &aPlaylists[aPlaylists.size()-2];
		pPlaylistStunt = &aPlaylists[aPlaylists.size()-1];

		for (auto& playlist : aPlaylists) {
			for (auto& song : playlist.aSongs) {
				song.Load();
			}
		}
	}
}