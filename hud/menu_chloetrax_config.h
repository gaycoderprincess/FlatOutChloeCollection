class CMenu_ChloeTrax_Config : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_chloetrax_config"; }
	
	enum ePlaylist {
		PLAYLIST_TITLE,
		PLAYLIST_INGAME,
		PLAYLIST_DERBY,
		NUM_PLAYLISTS,
	};
	
	static inline const char* aPlaylistNames[] = {
			"Menu",
			"Race",
			"Derby",
	};
	
	int nPlaylist = 0;
	int nSelected = 0;
	int nScroll = 0;
	
	NewMusicPlayer::tPlaylist* GetSelectedPlaylist() {
		switch (nPlaylist) {
			case PLAYLIST_TITLE:
				return NewMusicPlayer::pPlaylistCustomTitle;
			case PLAYLIST_INGAME:
				return NewMusicPlayer::pPlaylistCustomIngame;
			case PLAYLIST_DERBY:
				return NewMusicPlayer::pPlaylistCustomDerby;
			default:
				return nullptr;
		}
	}
	
	void MoveLeft() override {
		if (nSelected == 0) {
			nPlaylist--;
			if (nPlaylist < 0) nPlaylist = NUM_PLAYLISTS-1;
		}
		else {
			auto playlist = GetSelectedPlaylist();
			
			int songId = (nSelected+nScroll)-1;
			if (nSelected > 0 && songId >= playlist->aSongs.size()) return;
			playlist->aSongs[songId].bDisabled = !playlist->aSongs[songId].bDisabled;
			Achievements::AwardAchievement(GetAchievement("CHANGE_MUSIC_TRAX"));
		}
	}
	
	void MoveRight() override {
		if (nSelected == 0) {
			nPlaylist++;
			if (nPlaylist >= NUM_PLAYLISTS) nPlaylist = 0;
		}
		else {
			auto playlist = GetSelectedPlaylist();
			
			int songId = (nSelected+nScroll)-1;
			if (nSelected > 0 && songId >= playlist->aSongs.size()) return;
			playlist->aSongs[songId].bDisabled = !playlist->aSongs[songId].bDisabled;
			Achievements::AwardAchievement(GetAchievement("CHANGE_MUSIC_TRAX"));
		}
	}
	
	void MoveUp() override {
		nSelected--;
		if (nSelected < 1) {
			if (nScroll > 0) {
				nSelected = 1;
				nScroll--;
			}
			else {
				nSelected = 0;
			}
		}
	}
	
	void MoveDown() override {
		nSelected++;
		if (nSelected > 4) {
			nSelected = 4;
			nScroll++;
			
			auto playlist = GetSelectedPlaylist();
			int songId = 3 + nScroll;
			if (songId >= playlist->aSongs.size()) nScroll--;
		}
	}
	
	void Select() override {
		MoveRight();
	}
	
	void Init() override {
		PreloadTexture("data/menu/chloetraxmenu1d.png");
		PreloadTexture("data/menu/chloetraxmenu1s.png");
		PreloadTexture("data/menu/chloetraxmenu2d.png");
		PreloadTexture("data/menu/chloetraxmenu2s.png");
		PreloadTexture("data/menu/chloetraxmenu3d.png");
		PreloadTexture("data/menu/chloetraxmenu3s.png");
		PreloadTexture("data/menu/chloetraxmenu4d.png");
		PreloadTexture("data/menu/chloetraxmenu4s.png");
		PreloadTexture("data/menu/chloetraxmenuicon.png");
		PreloadTexture("data/menu/chloetraxmenutops.png");
		PreloadTexture("data/menu/chloetraxmenutopd.png");
	}
	
	tDrawPositions gTopBar = {0.514, 0.227, 0.045};
	tDrawPositions gSongList = {0.113, 0.3325, 0.04, 0.57, 0.1235};
	float fOnOffYOffset = 0.021;

	void Reset() override {
		nPlaylist = 0;
		nSelected = 0;
		nScroll = 0;
	}

	void Process() override {
		if (!bEnabled) return;
		
		NewMusicPlayer::bCustomPlaylistsEnabled = true;
		nMenuSoundtrack = NewMusicPlayer::aPlaylistsTitle.size()-1;
		nIngameSoundtrack = NewMusicPlayer::aPlaylistsIngame.size()-1;
		nIngameDerbySoundtrack = NewMusicPlayer::aPlaylistsIngame.size()-1;
		
		static IDirect3DTexture9* textures[] = {
				LoadTextureFromBFS("data/menu/chloetraxmenutopd.png"),
				LoadTextureFromBFS("data/menu/chloetraxmenu1d.png"),
				LoadTextureFromBFS("data/menu/chloetraxmenu2d.png"),
				LoadTextureFromBFS("data/menu/chloetraxmenu3d.png"),
				LoadTextureFromBFS("data/menu/chloetraxmenu4d.png"),
		};
		static IDirect3DTexture9* texturesSelected[] = {
				LoadTextureFromBFS("data/menu/chloetraxmenutops.png"),
				LoadTextureFromBFS("data/menu/chloetraxmenu1s.png"),
				LoadTextureFromBFS("data/menu/chloetraxmenu2s.png"),
				LoadTextureFromBFS("data/menu/chloetraxmenu3s.png"),
				LoadTextureFromBFS("data/menu/chloetraxmenu4s.png"),
		};
		
		static auto textureBg = LoadTextureFromBFS("data/menu/chloetraxmenuicon.png");
		DrawRectangle(0, 1, 0, 1, {255,255,255,255}, 0, textureBg);
	
		auto playlist = GetSelectedPlaylist();
		for (int i = 0; i < 5; i++) {
			auto draw = i == 0 ? gTopBar : gSongList;
			
			bool selected = nSelected == i;
			DrawRectangle(0, 1, 0, 1, {255,255,255,255}, 0, selected ? texturesSelected[i] : textures[i]);
			
			int songId = (i+nScroll)-1;
			if (i > 0 && songId >= playlist->aSongs.size()) continue;
			
			std::string leftStr;
			if (i > 0) {
				auto song = &playlist->aSongs[songId];
				leftStr = std::format("{}\n{}", song->sArtist, song->sTitle);
			}
			else {
				leftStr = aPlaylistNames[nPlaylist];
			}
			
			tNyaStringData data;
			data.x = draw.fPosX;
			data.y = draw.fPosY;
			if (i > 0) data.y += draw.fSpacingY * (i-1);
			data.size = draw.fSize;
			data.XCenterAlign = i == 0;
			DrawString(data, leftStr, &DrawStringFO2_Ingame12);
			
			if (i != 0) {
				auto song = &playlist->aSongs[songId];
				
				data.x += draw.fSpacingX;
				data.y += fOnOffYOffset;
				data.XCenterAlign = true;
				DrawString(data, song->bDisabled ? "Off" : "On", &DrawStringFO2_Ingame12);
			}
		}
	}
} Menu_ChloeTrax_Config;