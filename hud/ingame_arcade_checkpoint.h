class CHUD_Arcade_Checkpoint : public CIngameHUDElement {
public:

	void Init() override {
		nHUDLayer = eHUDLayer::WORLD;
		PreloadTexture("data/global/overlay/checkpoint.dds");
	}

	static inline float fCheckpointYOffset = 6;
	static inline float fCheckpointSize = 1.5;
	static inline float fCheckpointFadeStart = 100;
	static inline float fCheckpointFadeEnd = 150;

	static float GetGroundY(NyaVec3 pos) {
		tLineOfSightIn in;
		tLineOfSightOut out;

		pos.y += 50;

		NyaVec3 dir = {0,-1,0};
		in.fMaxDistance = 100;
		if (CheckLineOfSight(&pos, &in, nullptr, &dir, &out, -1, nullptr)) {
			return pos.y - out.fHitDistance;
		}
		return pos.y - 50;
	}

	NyaVec3 gArcadeCheckpoint;
	void DrawCheckpoint(NyaVec3 cameraPos) {
		static auto tex = LoadTextureFromBFS("data/global/overlay/checkpoint.dds");

		auto pos = gArcadeCheckpoint;
		pos.y = GetGroundY(pos);
		pos.y += fCheckpointYOffset;

		auto cameraDist = (cameraPos - pos).length();
		if (cameraDist > fCheckpointFadeEnd) return;

		auto drawPos = Get3DTo2D(pos);
		if (drawPos.z <= 0) return;

		NyaDrawing::CNyaRGBA32 color = {255,255,255,255};
		if (cameraDist > fCheckpointFadeStart) {
			color.a = std::lerp(255, 0, (cameraDist - fCheckpointFadeStart) / (fCheckpointFadeEnd - fCheckpointFadeStart));
		}

		auto fAspect = 1024.0 / 256.0;
		auto fSize = fCheckpointSize / drawPos.z;
		DrawRectangle(drawPos.x - fSize * fAspect * GetAspectRatioInv(), drawPos.x + fSize * fAspect * GetAspectRatioInv(), drawPos.y - fSize, drawPos.y + fSize, color, 0, tex);
	}

	void Process() override {
		if (!bIsCarnageRace) return;

		auto cam = pCameraManager->pCamera;
		if (!cam) return;
		DrawCheckpoint(cam->GetMatrix()->p);
	}
} HUD_Arcade_Checkpoint;