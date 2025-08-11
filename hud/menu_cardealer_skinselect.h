class CMenu_CarDealerSkinSelect : public CMenuHUDElement {
public:
	static constexpr tDrawPositions1080p gCarSkinSelectTitle = {1720, 328, 0.035};
	//static constexpr tDrawPositions1080p gCarSkinSelectAuthor = {1588,797,0.02};
	static constexpr tDrawPositions1080p gCarSkinSelectAuthor = {1716,801,0.035};

	virtual void Process() {
		static auto textureRight = LoadTextureFromBFS("data/menu/carselect_right.png");
		static auto textureSkinAuthor = LoadTextureFromBFS("data/menu/carselect_skin.png");
		static auto textureArrows = LoadTextureFromBFS("data/menu/carselect_arrows.png");

		if (!bEnabled) return;

		auto menu = pGameFlow->pMenuInterface;
		if (!menu) return;
		if (!menu->pMenuScene) return;

		int car = menu->pMenuScene->nCar;
		int skin = menu->pMenuScene->nCarSkin;
		int numSkins = GetNumSkinsForCar(car);

		Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255,255,255,255}, textureRight);

		bool skinLoop = false;
		if (numSkins > 6) {
			Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255, 255, 255, 255}, textureArrows);
			skinLoop = true;
		}
		else {
			skinLoop = false;
		}

		if (auto author = GetSkinAuthor(car, skin, false); !author.empty()) {
			Draw1080pSprite(JUSTIFY_RIGHT, 0, 1920, 0, 1080, {255, 255, 255, 255}, textureSkinAuthor);

			tNyaStringData data;
			data.x = gCarSkinSelectAuthor.nPosX;
			data.y = gCarSkinSelectAuthor.nPosY;
			data.size = gCarSkinSelectAuthor.fSize;
			data.XCenterAlign = true;
			Draw1080pString(JUSTIFY_RIGHT, data, author, &DrawStringFO2_Ingame12);
		}

		tNyaStringData data;
		data.x = gCarSkinSelectTitle.nPosX;
		data.y = gCarSkinSelectTitle.nPosY;
		data.size = gCarSkinSelectTitle.fSize;
		data.XCenterAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, "SKINS", &DrawStringFO2_Small);

		float fPosX = 0.316;
		float fPosY = 0.372;
		float fSize = 0.035;
		float fSpacing = 0.0495;

		data.XCenterAlign = false;
		data.x = 1.0 - (fPosX * GetAspectRatioInv());
		data.y = fPosY;
		data.size = fSize;
		for (int i = 1; i <= 6; i++) {
			int skinId = 0;
			if (numSkins > 6) {
				skinId = skin - 3 + i;
			}
			else {
				skinId = i;
			}

			if (skinId == skin) {
				data.SetColor(GetPaletteColor(18));
			}
			else {
				data.SetColor(GetPaletteColor(17));
			}

			DrawString(data, GetSkinName(car, skinId, skinLoop), &DrawStringFO2_Ingame12);
			data.y += fSpacing;
		}
	}
} Menu_CarDealerSkinSelect;