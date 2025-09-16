class CHUD_Minimap : public CIngameHUDElement {
public:
	IDirect3DTexture9* pMapTexture = nullptr;
	static constexpr float fArrowSize = 0.011;

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

	static inline float fFO2MapPos[2] = {-120, 140};
	static inline float fFO2MapSize[2] = {400, 400};
	static inline float fFO2MapClipSize[2] = {128, 128};
	static void GetMapExtents(float* left, float* right, float* top, float* bottom) {
		auto posX = pEnvironment->pMinimap->fScreenPos[0];
		auto posY = pEnvironment->pMinimap->fScreenPos[1];
		auto sizeX = pEnvironment->pMinimap->fScreenSize[0];
		auto sizeY = pEnvironment->pMinimap->fScreenSize[1];
		if (bFO2Minimap) {
			posX = fFO2MapPos[0];
			posY = fFO2MapPos[1];
			sizeX = fFO2MapSize[0];
			sizeY = fFO2MapSize[1];
		}

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

	static NyaVec3 GetPositionOnMap(NyaVec3 pos, bool usePosOffset = true) {
		auto startX = pEnvironment->pMinimap->fMapTopLeft[0];
		auto startY = pEnvironment->pMinimap->fMapBottomRight[1];
		auto endX = pEnvironment->pMinimap->fMapBottomRight[0];
		auto endY = pEnvironment->pMinimap->fMapTopLeft[1];

		float left, right, top, bottom;
		GetMapExtents(&left, &right, &top, &bottom);

		if (bFO2Minimap) {
			NyaMat4x4 mat;
			mat.SetIdentity();
			mat.p = pos;

			NyaMat4x4 plyMat;
			plyMat.SetIdentity();
			plyMat.Rotate({0, 0, -fLocalPlayerHeading});
			if (usePosOffset) plyMat.p = vLocalPlayerPosition;
			plyMat = plyMat.Invert();
			mat = plyMat * mat;

			pos = mat.p;
		}

		pos.x -= startX;
		pos.x /= startX - endX;
		pos.z -= startY;
		pos.z /= startY - endY;
		float spritePosX = std::lerp(left, right, -pos.x);
		float spritePosY = std::lerp(bottom, top, -pos.z);
		return {spritePosX, spritePosY, 0};
	}

	static void DrawPlayerOnMap(Player* ply) {
		static auto arrow = LoadTextureFromBFS("data/global/overlay/map_playerarrow.png");
		static auto arrowPlayer = LoadTextureFromBFS("data/global/overlay/map_playerarrow_local.png");

		auto plyMatrix = ply->pCar->GetMatrix();
		auto plyPos = GetPositionOnMap(plyMatrix->p);
		auto plyDir = std::atan2(plyMatrix->z.x, plyMatrix->z.z) - fLocalPlayerHeading;
		DrawRectangle(plyPos.x - (fArrowSize * GetAspectRatioInv()), plyPos.x + (fArrowSize * GetAspectRatioInv()), plyPos.y - fArrowSize, plyPos.y + fArrowSize, GetPlayerColor(ply), 0, ply->nPlayerType == PLAYERTYPE_LOCAL ? arrowPlayer : arrow, plyDir);
	}

	NyaVec3 gArcadeCheckpoint;
	static void DrawCheckpointOnMap(NyaVec3 pos) {
		static auto texture = LoadTextureFromBFS("data/global/overlay/map_checkpoint.tga");

		auto spritePos = GetPositionOnMap(pos);
		DrawRectangle(spritePos.x - (fArrowSize * GetAspectRatioInv()), spritePos.x + (fArrowSize * GetAspectRatioInv()), spritePos.y - fArrowSize, spritePos.y + fArrowSize, {8,200,8,255}, 0, texture);
	}

	virtual void Process() {
		if (!nIngameMap) return;
		if (!bIsInMultiplayer && !IsRaceHUDUp()) return;
		if (pGameFlow->nEventType == eEventType::STUNT) return;

		auto plyMatrix = GetPlayer(0)->pCar->GetMatrix();
		fLocalPlayerHeading = bFO2Minimap ? std::atan2(plyMatrix->z.x, plyMatrix->z.z) : 0;
		vLocalPlayerPosition = bFO2Minimap ? plyMatrix->p : NyaVec3(0, 0, 0);

		if (bFO2Minimap) {
			DrawCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
				auto pos = GetPositionOnMap(vLocalPlayerPosition);
				pos.x *= nResX;
				pos.y *= nResY;
				float sizeX = fFO2MapClipSize[0];
				float sizeY = fFO2MapClipSize[1];
				ImGui::GetForegroundDrawList()->PushClipRect(ImVec2(pos.x-sizeX, pos.y-sizeY), ImVec2(pos.x+sizeX, pos.y+sizeY), false);
			}, true);
		}

		if (pMapTexture) {
			if (bFO2Minimap) {
				auto startX = pEnvironment->pMinimap->fMapTopLeft[0];
				auto startY = pEnvironment->pMinimap->fMapBottomRight[1];
				auto endX = pEnvironment->pMinimap->fMapBottomRight[0];
				auto endY = pEnvironment->pMinimap->fMapTopLeft[1];
				auto midX = std::lerp(startX, endX, 0.5);
				auto midY = std::lerp(startY, endY, 0.5);

				float left, right, top, bottom;
				GetMapExtents(&left, &right, &top, &bottom);
				auto posOffset = GetPositionOnMap(vLocalPlayerPosition) - GetPositionOnMap({midX, 0, midY});
				DrawRectangle(left-posOffset.x, right-posOffset.x, top-posOffset.y, bottom-posOffset.y, {255,255,255,255}, 0, pMapTexture, -fLocalPlayerHeading);
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
			DrawPlayerOnMap(ply);
		}

		if (bFO2Minimap) {
			DrawCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) { ImGui::GetForegroundDrawList()->PopClipRect(); }, true);
		}
	}
} HUD_Minimap;