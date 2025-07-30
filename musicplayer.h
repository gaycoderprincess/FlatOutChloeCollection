namespace NewMusicPlayer {
	struct tSong {
		std::string sPath;
		std::string sArtist;
		std::string sTitle;
		NyaAudio::NyaSound pStream = 0;
		int nStreamVolume = 0;
		bool bFinished = false;
		bool bAlreadyPlayed = false;

		void Load() {
			size_t size;

			auto data = (char*)ReadFileFromBfs(sPath.c_str(), size);
			if (!data) return;

			pStream = NyaAudio::LoadMemory(data, size);
			WriteLog(std::format("pStream {} size {}", sPath, size));
		}

		void Play() {
			nStreamVolume = nIngameMusicVolume;
			bFinished = true;

			if (pStream) {
				bFinished = false;
				NyaAudio::SetVolume(pStream, nStreamVolume / 100.0);
				NyaAudio::Play(pStream);
			}
		}

		void Update() {
			if (nStreamVolume != nIngameMusicVolume) {
				nStreamVolume = nIngameMusicVolume;
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

	tPlaylist* pCurrentPlaylist = nullptr;
	tSong* pCurrentSong = nullptr;

	void OnTick() {
		if (!pCurrentPlaylist) return;
		//if (!pPlayerHost && !pGameFlow->pMenuInterface) return;
		if (pLoadingScreen) return;

		if (pCurrentSong) {
			pCurrentSong->Update();
			if (pCurrentSong->bFinished) {
				pCurrentSong = nullptr;
			}
		}
		if (!pCurrentSong) {
			pCurrentSong = pCurrentPlaylist->GetNextSong();
			pCurrentSong->Play();
		}
	}

	void Init() {
		static bool bInited = false;
		if (bInited) {
			return;
		}
		bInited = true;

		NyaHookLib::Patch<uint8_t>(0x410CB0, 0xC3);

		size_t size;
		auto file = (char*)ReadFileFromBfs("data/music/playlist_0.toml", size);

		// this is fucking stupid, but tomlplusplus crashes trying to read from memory
		std::ofstream outFile("cctemp_playlist.toml", std::ios::out | std::ios::binary );
		if (!outFile.is_open()) return;
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
				if (song.sPath.empty()) continue;
				if (song.sArtist.empty()) continue;
				if (song.sTitle.empty()) continue;
				playlist.aSongs.push_back(song);
			}
			aPlaylists.push_back(playlist);
			pCurrentPlaylist = &aPlaylists[0];
		}
		catch (const toml::parse_error& err) {
			MessageBoxA(0, std::format("Parsing failed: {}", err.what()).c_str(), "Fatal error", MB_ICONERROR);
		}

		remove("cctemp_playlist.toml");

		for (auto& playlist : aPlaylists) {
			for (auto& song : playlist.aSongs) {
				song.Load();
			}
		}
	}
}