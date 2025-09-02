class CHUD_Minimap : public CIngameHUDElement {
public:
	IDirect3DTexture9* pMapTexture = nullptr;
	static constexpr float fArrowSize = 0.011;

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

	static void GetMapExtents(float* left, float* right, float* top, float* bottom) {
		auto posX = pEnvironment->pMinimap->fScreenPos[0];
		auto posY = pEnvironment->pMinimap->fScreenPos[1];
		auto sizeX = pEnvironment->pMinimap->fScreenSize[0];
		auto sizeY = pEnvironment->pMinimap->fScreenSize[1];

		auto justify = CHUDElement::JUSTIFY_LEFT;
		if (IsInSplitScreen()) {
			if (IsInQuarteredSplitScreen()) {
				posX = 320.0 - pEnvironment->pMinimap->fScreenSize[0] * 0.5;
				justify = CHUDElement::JUSTIFY_CENTER;
			}
			else {
				posX = 0.0;
			}
			posY = 240.0 - pEnvironment->pMinimap->fScreenSize[1] * 0.5;
		}

		posX /= 640.0;
		posY /= 480.0;
		sizeX /= 640.0;
		sizeY /= 480.0;
		// todo quartered splitscreen is squished but not having this branch uncenters it
		if (IsInQuarteredSplitScreen()) {
			posX *= 1920.0;
			posY *= 1080.0;
			sizeX *= 1920.0;
			sizeY *= 1080.0;
		}
		else {
			posX *= 1440.0;
			posY *= 1080.0;
			sizeX *= 1440.0;
			sizeY *= 1080.0;
		}

		CHUDElement::DoJustify(justify, posX, posY);
		CHUDElement::DoJustify(justify, sizeX, sizeY);

		*left = posX;
		*right = posX + sizeX;
		*top = posY;
		*bottom = posY + sizeY;
	}

	static void DrawPlayerOnMap(Player* ply) {
		static auto arrow = CHUDElement::LoadTextureFromBFS("data/global/overlay/map_playerarrow.png");
		static auto arrowPlayer = CHUDElement::LoadTextureFromBFS("data/global/overlay/map_playerarrow_local.png");

		auto startX = pEnvironment->pMinimap->fMapTopLeft[0];
		auto startY = pEnvironment->pMinimap->fMapBottomRight[1];
		auto endX = pEnvironment->pMinimap->fMapBottomRight[0];
		auto endY = pEnvironment->pMinimap->fMapTopLeft[1];

		float left, right, top, bottom;
		GetMapExtents(&left, &right, &top, &bottom);

		auto plyMatrix = ply->pCar->GetMatrix();
		auto plyPos = plyMatrix->p;
		auto plyDir = std::atan2(plyMatrix->z.x, plyMatrix->z.z);
		plyPos.x -= startX;
		plyPos.x /= startX - endX;
		plyPos.z -= startY;
		plyPos.z /= startY - endY;
		if (-plyPos.x < 0) return;
		if (-plyPos.z < 0) return;
		if (-plyPos.x > 1) return;
		if (-plyPos.z > 1) return;
		float spritePosX = std::lerp(left, right, -plyPos.x);
		float spritePosY = std::lerp(bottom, top, -plyPos.z);
		DrawRectangle(spritePosX - (fArrowSize * GetAspectRatioInv()), spritePosX + (fArrowSize * GetAspectRatioInv()), spritePosY - fArrowSize, spritePosY + fArrowSize, GetPlayerColor(ply), 0, ply->nPlayerType == PLAYERTYPE_LOCAL ? arrowPlayer : arrow, plyDir);
	}

	NyaVec3 gArcadeCheckpoint;
	static void DrawCheckpointOnMap(NyaVec3 pos) {
		static auto texture = CHUDElement::LoadTextureFromBFS("data/global/overlay/map_checkpoint.tga");

		auto startX = pEnvironment->pMinimap->fMapTopLeft[0];
		auto startY = pEnvironment->pMinimap->fMapBottomRight[1];
		auto endX = pEnvironment->pMinimap->fMapBottomRight[0];
		auto endY = pEnvironment->pMinimap->fMapTopLeft[1];

		float left, right, top, bottom;
		GetMapExtents(&left, &right, &top, &bottom);

		pos.x -= startX;
		pos.x /= startX - endX;
		pos.z -= startY;
		pos.z /= startY - endY;
		if (-pos.x < 0) return;
		if (-pos.z < 0) return;
		if (-pos.x > 1) return;
		if (-pos.z > 1) return;
		float spritePosX = std::lerp(left, right, -pos.x);
		float spritePosY = std::lerp(bottom, top, -pos.z);
		DrawRectangle(spritePosX - (fArrowSize * GetAspectRatioInv()), spritePosX + (fArrowSize * GetAspectRatioInv()), spritePosY - fArrowSize, spritePosY + fArrowSize, {8,200,8,255}, 0, texture);
	}

	virtual void Process() {
		if (!nIngameMap) return;
		if (!bIsInMultiplayer && !CIngameHUDElement::IsRaceHUDUp()) return;
		if (pGameFlow->nEventType == eEventType::STUNT) return;

		if (pMapTexture) {
			float left, right, top, bottom;
			GetMapExtents(&left, &right, &top, &bottom);
			DrawRectangle(left, right, top, bottom, {255,255,255,255}, 0, pMapTexture);
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
	}
} HUD_Minimap;