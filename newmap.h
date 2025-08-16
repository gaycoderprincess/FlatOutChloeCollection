float fIngameMapSize = 0.011;

NyaDrawing::CNyaRGBA32 GetPlayerColor(Player* ply) {
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
	if (CareerMode::IsCareerTimeTrial()) {
		if (id >= 0 && id <= sizeof(aPlayerColorsTimeTrial) / sizeof(aPlayerColorsTimeTrial[0])) return aPlayerColorsTimeTrial[id];
	}
	if (id >= 0 && id <= sizeof(aPlayerColors) / sizeof(aPlayerColors[0])) return aPlayerColors[id];
	return aPlayerColors[0];
}

void DrawPlayerOnIngameMap(Player* ply) {
	static auto arrow = CHUDElement::LoadTextureFromBFS("data/global/overlay/playerarrow.png");
	static auto arrowPlayer = CHUDElement::LoadTextureFromBFS("data/global/overlay/playerarrow_local.png");

	auto startX = pEnvironment->pMinimap->fMapTopLeft[0];
	auto startY = pEnvironment->pMinimap->fMapBottomRight[1];
	auto endX = pEnvironment->pMinimap->fMapBottomRight[0];
	auto endY = pEnvironment->pMinimap->fMapTopLeft[1];

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

	auto plyMatrix = ply->pCar->GetMatrix();
	auto plyPos = plyMatrix->p;
	auto plyDir = std::atan2(plyMatrix->z.x, plyMatrix->z.z);
	plyPos.x -= startX;
	plyPos.x /= startX - endX;
	plyPos.z -= startY;
	plyPos.z /= startY - endY;
	float spritePosX = std::lerp(posX, posX + sizeX, -plyPos.x);
	float spritePosY = std::lerp(posY + sizeY, posY, -plyPos.z);
	CHUDElement::DoJustify(CHUDElement::JUSTIFY_LEFT, spritePosX, spritePosY);
	DrawRectangle(spritePosX - (fIngameMapSize * GetAspectRatioInv()), spritePosX + (fIngameMapSize * GetAspectRatioInv()), spritePosY - fIngameMapSize, spritePosY + fIngameMapSize, GetPlayerColor(ply), 0, ply->nPlayerType == PLAYERTYPE_LOCAL ? arrowPlayer : arrow, plyDir);
}

void DrawIngameMap() {
	// draw wrecked players first, opponents after, local player on top
	for (int i = 1; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (!ply) continue;
		if (!IsPlayerWrecked(ply)) continue;
		DrawPlayerOnIngameMap(ply);
	}
	for (int i = 1; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = GetPlayer(i);
		if (!ply) continue;
		if (IsPlayerWrecked(ply)) continue;
		DrawPlayerOnIngameMap(ply);
	}
	DrawPlayerOnIngameMap(GetPlayer(0));
}

void D3DIngameMap() {
	bIsDrawingMap = true;
	D3DHookMain();
}

uintptr_t DrawIngameMapASM_jmp = 0x478371;
void __attribute__((naked)) DrawIngameMapASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (DrawIngameMapASM_jmp), "i" (D3DIngameMap)
	);
}

void ApplyIngameMapPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x47823D, &DrawIngameMapASM);
}