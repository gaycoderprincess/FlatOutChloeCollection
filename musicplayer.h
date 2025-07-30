namespace NewMusicPlayer {
	struct tSong {
		std::string sPath;
		std::string sArtist;
		std::string sTitle;
		NyaAudio::NyaSound pStream;
		int nStreamVolume = 0;
		bool bFinished = false;

		void Play() {
			nStreamVolume = nIngameMusicVolume;
			bFinished = true;

			size_t size;
			auto file = (char*)ReadFileFromBfs(sPath.c_str(), size);

			if (pStream = NyaAudio::LoadMemory(file)) {
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
	};
	std::vector<tPlaylist> aPlaylists;

	void Init() {
		static bool bInited = false;
		if (bInited) {
			return;
		}
		bInited = true;

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
		}
		catch (const toml::parse_error& err) {
			MessageBoxA(0, std::format("Parsing failed: {}", err.what()).c_str(), "Fatal error", MB_ICONERROR);
		}

		remove("cctemp_playlist.toml");
	}
}