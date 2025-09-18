class CHUD_Minimap : public CIngameHUDElement {
public:
	static inline IDirect3DTexture9* pMapTexture = nullptr;
	static inline IDirect3DTexture9* pMapTextureFO2 = nullptr;
	static constexpr float fArrowSize = 0.011;
	static constexpr float fResetSize = 0.015;

	static inline bool bFO2Minimap = true;
	static inline NyaVec3 vLocalPlayerPosition = {0, 0, 0};
	static inline float fLocalPlayerHeading = 0;

	static NyaDrawing::CNyaRGBA32 GetPlayerColor(Player* ply) {
		if (IsPlayerWrecked(ply)) return {0,0,0,127};

		//NyaDrawing::CNyaRGBA32 aPlayerColors[] = {
		//		{255,255,255,255},
		//		//{21,34,41,255}, // Frank Benton
		//		{206,195,33,255}, // Frank Benton
		//		{148,36,24,255}, // Sue O'Neill
		//		{137,28,33,255}, // Tania Graham
		//		{16,29,104,255}, // Katie Daggert
		//		{49,48,148,255}, // Ray Smith
		//		{183,64,43,255}, // Paul McGuire
		//		//{189,174,16,255}, // Seth Bellinger
		//		{2,8,10,255}, // Seth Bellinger
		//};

		NyaDrawing::CNyaRGBA32 aPlayerColors[] = {
				{255,255,255,255},
				{224,211,14,255}, // Frank Benton
				{172,22,6,255}, // Sue O'Neill
				{119,19,24,255}, // Tania Graham
				{16,29,104,255}, // Katie Daggert
				{64,64,198,255}, // Ray Smith
				{213,51,23,255}, // Paul McGuire
				{2,8,10,255}, // Seth Bellinger
		};

		NyaDrawing::CNyaRGBA32 aPlayerColorsTimeTrial[] = {
				{255,255,255,255},
				{236,221,16,255}, // gold
				{186,186,186,255}, // silver
				{175,100,0,255}, // bronze
				{30,160,0,255}, // author
				{219,100,193,255}, // super author
		};

		int id = ply->nPlayerId - 1;
		if (IsInSplitScreen() && !bIsCareerRace && ply->nPlayerType == PLAYERTYPE_LOCAL) id++; // never use white arrow in splitscreen
		if (CareerMode::IsCareerTimeTrial()) {
			if (id >= 0 && id <= sizeof(aPlayerColorsTimeTrial) / sizeof(aPlayerColorsTimeTrial[0])) return aPlayerColorsTimeTrial[id];
		}
		if (id >= 0 && id <= sizeof(aPlayerColors) / sizeof(aPlayerColors[0])) return aPlayerColors[id];
		return aPlayerColors[0];
	}

	static inline float fFO2MapPos[2] = {95, 340};
	static inline float fFO2MapSize = 0.001;
	static inline float fFO2MapSizeRace = 0.001;
	static inline float fFO2MapSizeDerby = 0.001;
	static inline float fFO2MapClipSize = 128;
	static void GetMapExtents(float* left, float* right, float* top, float* bottom) {
		auto posX = pEnvironment->pMinimap->fScreenPos[0];
		auto posY = pEnvironment->pMinimap->fScreenPos[1];
		auto sizeX = pEnvironment->pMinimap->fScreenSize[0];
		auto sizeY = pEnvironment->pMinimap->fScreenSize[1];

		posX /= 640.0;
		posY /= 480.0;
		sizeX /= 640.0;
		sizeY /= 480.0;
		posX *= 1440.0;
		posY *= 1080.0;
		sizeX *= 1440.0;
		sizeY *= 1080.0;

		auto justify = JUSTIFY_LEFT;
		if (IsInSplitScreen()) {
			if (IsInQuarteredSplitScreen()) {
				posX = 960.0 - sizeX * 0.5;
				justify = JUSTIFY_CENTER;
			}
			else {
				posX = 0.0;
			}
			posY = 540.0 - sizeY * 0.5;
		}

		DoJustify(justify, posX, posY);
		DoJustify(justify, sizeX, sizeY);

		*left = posX;
		*right = posX + sizeX;
		*top = posY;
		*bottom = posY + sizeY;
	}

	static void GetFO2MapPosition(float* x, float* y) {
		auto posX = fFO2MapPos[0];
		auto posY = fFO2MapPos[1];
		DoJustify(JUSTIFY_480P_LEFT, posX, posY);

		*x = posX;
		*y = posY;
	}

	static NyaVec3 GetPositionOnMap(NyaVec3 pos, bool useOffset = true) {
		if (bFO2Minimap) {
			NyaMat4x4 mat;
			mat.SetIdentity();
			mat.p = pos;

			NyaMat4x4 plyMat;
			plyMat.SetIdentity();
			plyMat.Rotate({0, 0, -fLocalPlayerHeading});
			plyMat.p = vLocalPlayerPosition;
			plyMat = plyMat.Invert();
			mat = plyMat * mat;

			pos = mat.p;

			float x, y;
			GetFO2MapPosition(&x, &y);

			if (!useOffset) {
				x = 0;
				y = 0;
			}
			return NyaVec3(x+(pos.x*fFO2MapSize*GetAspectRatioInv()), y-pos.z*fFO2MapSize, 0);
		}
		else {
			auto startX = pEnvironment->pMinimap->fWorldTopLeft[0];
			auto startY = pEnvironment->pMinimap->fWorldBottomRight[1];
			auto endX = pEnvironment->pMinimap->fWorldBottomRight[0];
			auto endY = pEnvironment->pMinimap->fWorldTopLeft[1];

			float left, right, top, bottom;
			GetMapExtents(&left, &right, &top, &bottom);

			pos.x -= startX;
			pos.x /= startX - endX;
			pos.z -= startY;
			pos.z /= startY - endY;
			float spritePosX = std::lerp(left, right, -pos.x);
			float spritePosY = std::lerp(bottom, top, -pos.z);
			return {spritePosX, spritePosY, 0};
		}
	}

	static void DrawPlayerOnMap(Player* ply) {
		static auto arrow = LoadTextureFromBFS("data/global/overlay/map_playerarrow.png");
		static auto arrowPlayer = LoadTextureFromBFS("data/global/overlay/map_playerarrow_local.png");

		auto plyMatrix = ply->pCar->GetMatrix();
		auto plyPos = GetPositionOnMap(plyMatrix->p);
		auto plyDir = std::atan2(plyMatrix->z.x, plyMatrix->z.z) - fLocalPlayerHeading;
		DrawRectangle(plyPos.x - (fArrowSize * GetAspectRatioInv()), plyPos.x + (fArrowSize * GetAspectRatioInv()), plyPos.y - fArrowSize, plyPos.y + fArrowSize, GetPlayerColor(ply), 0, ply->nPlayerType == PLAYERTYPE_LOCAL ? arrowPlayer : arrow, plyDir);
	}

	static void DrawPlayerResetOnMap(Player* ply) {
		static auto arrow = LoadTextureFromBFS("data/global/overlay/map_resetpoint.png");

		auto plyPos = GetPositionOnMap(NyaVec3(ply->fLastValidPosition[0], ply->fLastValidPosition[1], ply->fLastValidPosition[2]));
		DrawRectangle(plyPos.x - (fResetSize * GetAspectRatioInv()), plyPos.x + (fResetSize * GetAspectRatioInv()), plyPos.y - fResetSize, plyPos.y + fResetSize, {255,255,255,255}, 0, arrow);
	}

	NyaVec3 gArcadeCheckpoint;
	static void DrawCheckpointOnMap(NyaVec3 pos) {
		static auto texture = LoadTextureFromBFS("data/global/overlay/map_checkpoint.tga");

		auto spritePos = GetPositionOnMap(pos);
		DrawRectangle(spritePos.x - (fArrowSize * GetAspectRatioInv()), spritePos.x + (fArrowSize * GetAspectRatioInv()), spritePos.y - fArrowSize, spritePos.y + fArrowSize, {8,200,8,255}, 0, texture);
	}

	static void LoadMinimaps() {
		auto config = ReadTOMLFromBfsLUAHack(std::format("{}data/map.bed", pEnvironment->sStagePath.Get()));
		pMapTexture = LoadTextureFromBFS(config["MapTexture"].value_or(""));

		bool useFO2Minimap = nUseFO2Minimap == 2;
		if (pGameFlow->nGameMode == eGameMode::SPLITSCREEN && pGameFlow->nEventType != eEventType::STUNT) {
			useFO2Minimap = false;
		}
		
		// load FO2 minimap
		auto fo2ConfigPath = std::format("{}data/map_fo2.bed", pEnvironment->sStagePath.Get());
		if (useFO2Minimap && DoesFileExist(fo2ConfigPath.c_str())) {
			config = ReadTOMLFromBfsLUAHack(fo2ConfigPath);
			pMapTextureFO2 = LoadTextureFromBFS(config["MapTexture"].value_or(""));
			pEnvironment->pMinimap->fWorldTopLeft[0] = config["MapTopLeft"][0].value_or(0.0f);
			pEnvironment->pMinimap->fWorldTopLeft[1] = config["MapTopLeft"][2].value_or(0.0f);
			pEnvironment->pMinimap->fWorldBottomRight[0] = config["MapBottomRight"][0].value_or(0.0f);
			pEnvironment->pMinimap->fWorldBottomRight[1] = config["MapBottomRight"][2].value_or(0.0f);
			//pEnvironment->pMinimap->fScreenPos[0] = config["ScreenPos"][0].value_or(0.0f);
			//pEnvironment->pMinimap->fScreenPos[1] = config["ScreenPos"][1].value_or(0.0f);
			//pEnvironment->pMinimap->fScreenSize[0] = config["ScreenSize"][0].value_or(0.0f);
			//pEnvironment->pMinimap->fScreenSize[1] = config["ScreenSize"][1].value_or(0.0f);
		}
	}

	virtual void Init() {
		PreloadTexture("data/global/overlay/map_playerarrow.png");
		PreloadTexture("data/global/overlay/map_playerarrow_local.png");
		PreloadTexture("data/global/overlay/map_resetpoint.png");
		PreloadTexture("data/global/overlay/map_checkpoint.tga");

		ChloeEvents::MapLoadEvent.AddHandler(LoadMinimaps);
	}

	virtual void Process() {
		if (!nIngameMap) return;
		if (!bIsInMultiplayer && !IsRaceHUDUp()) return;
		if (pGameFlow->nEventType == eEventType::STUNT) return;

		fFO2MapSize = pGameFlow->nEventType == eEventType::DERBY ? fFO2MapSizeDerby : fFO2MapSizeRace;

		if (nUseFO2Minimap == 2) {
			bFO2Minimap = !IsInSplitScreen();
		}
		else {
			bFO2Minimap = nUseFO2Minimap && !IsInSplitScreen() && DoesTrackValueExist(pGameFlow->nLevel, "UseFO2Minimap");
		}

		auto plyMatrix = GetPlayer(0)->pCar->GetMatrix();
		fLocalPlayerHeading = bFO2Minimap ? std::atan2(plyMatrix->z.x, plyMatrix->z.z) : 0;
		vLocalPlayerPosition = bFO2Minimap ? plyMatrix->p : NyaVec3(0, 0, 0);

		if (bFO2Minimap && pGameFlow->nEventType != eEventType::DERBY) {
			DrawCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
				auto pos = GetPositionOnMap(vLocalPlayerPosition);
				pos.x *= nResX;
				pos.y *= nResY;
				float size = fFO2MapClipSize;
				size /= 1080.0;
				size *= nResY;
				ImGui::GetForegroundDrawList()->PushClipRect(ImVec2(pos.x-size, pos.y-size), ImVec2(pos.x+size, pos.y+size), false);
			}, true);
		}

		if (pMapTexture) {
			if (bFO2Minimap) {
				auto texture = pMapTextureFO2;
				if (!texture) texture = pMapTexture;

				auto startX = pEnvironment->pMinimap->fWorldTopLeft[0];
				auto startY = pEnvironment->pMinimap->fWorldBottomRight[1];
				auto endX = pEnvironment->pMinimap->fWorldBottomRight[0];
				auto endY = pEnvironment->pMinimap->fWorldTopLeft[1];
				auto midX = std::lerp(startX, endX, 0.5);
				auto midY = std::lerp(startY, endY, 0.5);
				auto sizeX = (endX - startX) * fFO2MapSize;
				auto sizeY = (endY - startY) * fFO2MapSize;

				auto plyPos = GetPositionOnMap(NyaVec3(midX, 0, midY));
				DrawRectangle(plyPos.x - (sizeX * 0.5 * GetAspectRatioInv()), plyPos.x + (sizeX * 0.5 * GetAspectRatioInv()), plyPos.y - sizeY * 0.5, plyPos.y + sizeY * 0.5, {255,255,255,255}, 0, texture, -fLocalPlayerHeading);
			}
			else {
				float left, right, top, bottom;
				GetMapExtents(&left, &right, &top, &bottom);
				DrawRectangle(left, right, top, bottom, {255,255,255,255}, 0, pMapTexture);
			}
		}

		// draw wrecked players first, opponents after, then arcade checkpoint, local player on top
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			if (ply->nPlayerType == PLAYERTYPE_LOCAL) continue;
			if (!IsPlayerWrecked(ply)) continue;
			DrawPlayerOnMap(ply);
		}
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			if (ply->nPlayerType == PLAYERTYPE_LOCAL) continue;
			if (IsPlayerWrecked(ply)) continue;
			DrawPlayerOnMap(ply);
		}

		if (bIsCarnageRace) {
			DrawCheckpointOnMap(gArcadeCheckpoint);
		}

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			if (ply->nPlayerType != PLAYERTYPE_LOCAL) continue;
			if (!ply->nIsOutOfTrack) continue;
			DrawPlayerResetOnMap(ply);
		}

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			if (ply->nPlayerType != PLAYERTYPE_LOCAL) continue;
			DrawPlayerOnMap(ply);
		}

		if (bFO2Minimap && pGameFlow->nEventType != eEventType::DERBY) {
			DrawCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) { ImGui::GetForegroundDrawList()->PopClipRect(); }, true);
		}
	}
} HUD_Minimap;