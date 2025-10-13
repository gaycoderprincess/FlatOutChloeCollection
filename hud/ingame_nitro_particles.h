class CHUD_Nitro_Particles : public CIngameHUDElement {
public:

	void Init() override {
		nHUDLayer = eHUDLayer::WORLD;

		PreloadTexture("data/global/particles/nitro.tga");
		PreloadTexture("data/global/particles/nitro_once.tga");
	}

	bool DrawInReplay() override { return true; }

	double fPlayerNitroParticleTimer[nMaxPlayers] = {};
	int nPlayerNitroParticleStage[nMaxPlayers] = {};

	static inline const char* aDisallowedCars[] = {
		"Bullet" // wrong exhaust dummies
	};

	static bool IsBlocked(NyaVec3 pos) {
		tLineOfSightIn in;
		tLineOfSightOut out;

		NyaVec3 dir = pos - pCameraManager->pCamera->GetMatrix()->p;
		in.fMaxDistance = dir.Normalize();
		return CheckLineOfSight(&pCameraManager->pCamera->GetMatrix()->p, &in, nullptr, &dir, &out, -1, nullptr);
	}

	static inline float fNitroParticleTimerSpeed = 0.05;
	static inline float fNitroParticleSize = 0.09;
	void DrawPlayerNitroBoost(Player* ply, double delta) {
		if (!bIsInMultiplayer && ply->nPlayerType != PLAYERTYPE_LOCAL) return;
		if (ply->pCar->fNitroButton <= 0.0) return;
		if (ply->pCar->GetNitro() <= 0.0) return;

		for (auto& name : aDisallowedCars) {
			if (!strcmp(ply->pCar->sName.Get(), name)) return;
		}

		auto cam = pCameraManager->pCamera;
		if (!cam) return;
		auto mat = *cam->GetMatrix();

		auto cameraDot = ply->pCar->GetMatrix()->z.Dot(cam->GetMatrix()->z);

		float alphaMult = (cameraDot - 0.5) * 2;
		if (alphaMult <= 0.0) return;

		auto& timer = fPlayerNitroParticleTimer[ply->nPlayerId-1];
		auto& stage = nPlayerNitroParticleStage[ply->nPlayerId-1];
		timer += delta;
		while (timer > fNitroParticleTimerSpeed) {
			stage++;
			if (stage > 3) stage = 0;
			timer -= fNitroParticleTimerSpeed;
		}

		static auto tex = LoadTextureFromBFS("data/global/particles/nitro.tga");

		std::vector<Object*> aExhausts;
		if (auto obj = ply->pCar->GetObjectByName("exhaust_l_dummy_1")) aExhausts.push_back(obj);
		if (auto obj = ply->pCar->GetObjectByName("exhaust_r_dummy_1")) aExhausts.push_back(obj);
		if (auto obj = ply->pCar->GetObjectByName("exhaust_1_dummy_1")) aExhausts.push_back(obj);
		if (auto obj = ply->pCar->GetObjectByName("exhaust_2_dummy_1")) aExhausts.push_back(obj);
		if (auto obj = ply->pCar->GetObjectByName("exhaust_3_dummy_1")) aExhausts.push_back(obj);

		for (auto& exhaust : aExhausts) {
			auto mat = *ply->pCar->GetMatrix() * *exhaust->GetMatrix();

			if (IsBlocked(mat.p)) continue;

			auto drawPos = Get3DTo2D(mat.p);
			if (drawPos.z <= 0) continue;

			ImVec2 uvMin = {(float)(stage * 0.25), 0};
			ImVec2 uvMax = {(float)((stage * 0.25) + 0.25), 1};

			float fSize = fNitroParticleSize / drawPos.z;
			DrawRectangle(drawPos.x - fSize * GetAspectRatioInv(), drawPos.x + fSize * GetAspectRatioInv(), drawPos.y - fSize, drawPos.y + fSize, {255,255,255,(uint8_t)(255*ply->pCar->fNitroButton*alphaMult)}, 0, tex, 0, uvMin, uvMax);
		}
	}

	void Process() override {
		if (!IsRaceHUDUp() || IsInSplitScreen()) return;

		static CNyaRaceTimer gTimer;
		gTimer.Process();

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			DrawPlayerNitroBoost(GetPlayer(i), gTimer.fDeltaTime);
		}
	}
} HUD_Nitro_Particles;