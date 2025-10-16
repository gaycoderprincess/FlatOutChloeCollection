class CHUD_Multiplayer_Overhead : public CIngameHUDElement {
public:

	void Init() override {
		nHUDLayer = eHUDLayer::WORLD;
	}

	static inline float fPlayerNameOffset = 0.031;
	static inline float fPlayerNameSize = 0.022;
	static inline float fPlayerNameFadeStart = 50.0;
	static inline float fPlayerNameFadeEnd = 150.0;
	static void DrawPlayerName(Player* ply, NyaVec3 cameraPos) {
		if (ply->nPlayerType == PLAYERTYPE_LOCAL) return;

		auto mat = *ply->pCar->GetMatrix();
		auto cameraDist = (cameraPos - mat.p).length();
		if (cameraDist > fPlayerNameFadeEnd) return;

		//auto yUp = mat.y;
		//if (yUp.y < 0) yUp.y *= -1;
		//mat.p += yUp * fPlayerHealthIconOffset;
		//mat.p.y += fPlayerIconOffset - fPlayerHealthIconOffset;

		mat.p.y += ply->pCar->GetCenter()->y;
		mat.p.y += ply->pCar->GetRadius()->y;

		auto drawPos = Get3DTo2D(mat.p);
		if (drawPos.z <= 0) return;

		tNyaStringData data;
		data.x = drawPos.x;
		data.y = drawPos.y - fPlayerNameOffset;
		data.size = fPlayerNameSize;
		data.XCenterAlign = true;
		data.SetColor(GetPlayerColor(ply));
		if (cameraDist > fPlayerNameFadeStart) {
			data.a = std::lerp(96, 0, (cameraDist - fPlayerNameFadeStart) / (fPlayerNameFadeEnd - fPlayerNameFadeStart));
		}
		else {
			data.a = 96;
		}
		DrawStringFO2_Regular18(data, GetStringNarrow(ply->sPlayerName.Get()));
	}

	void Process() override {
		if (!bIsInMultiplayer) return;
		if (!IsRaceHUDUp()) return;

		auto cam = pCameraManager->pCamera;
		if (!cam) return;
		auto mat = *cam->GetMatrix();

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			DrawPlayerName(GetPlayer(i), mat.p);
		}
	}
} HUD_Multiplayer_Overhead;