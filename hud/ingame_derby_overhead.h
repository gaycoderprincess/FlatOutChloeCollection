class CHUD_Derby_Overhead : public CIngameHUDElement {
public:

	void Init() override {
		nHUDLayer = eHUDLayer::WORLD;
		PreloadTexture("data/global/overlay/derby_health_indicator.tga");
		PreloadTexture("data/global/overlay/frag_derby_symbols.dds");
	}

	int nStreakerId = -1;
	int nSurvivorId = -1;

	void Reset() override {
		nStreakerId = -1;
		nSurvivorId = -1;
	}

	static inline float DamageIndicatorMaxDist = 128;
	static inline float DamageIndicatorScale = 0.025;
	static inline float DamageIndicatorYOffset = 1.6;
	static inline float DamageIndicatorFragScale = 1.8;
	static inline float DamageIndicatorFragYOffset = 2.5;
	static inline float DamageIndicatorAlpha = 0.85;

	static void DrawPlayerIcon(const std::string& icon, float size, NyaVec3 drawPos) {
		static auto tex = LoadTextureFromBFS("data/global/overlay/frag_derby_symbols.dds");
		static auto texData = LoadHUDData("data/global/overlay/frag_derby_symbols.bed", "frag_derby_symbols");

		drawPos.y -= DamageIndicatorFragYOffset * size;
		size *= DamageIndicatorFragScale;

		auto symbol = GetHUDData(texData, icon);
		DrawRectangle(drawPos.x - size * GetAspectRatioInv(), drawPos.x + size * GetAspectRatioInv(), drawPos.y - size, drawPos.y + size, {255,255,255,127}, 0, tex, 0, symbol->min, symbol->max);
	}

	void DrawPlayerHealthIcon(Player* ply, NyaVec3 cameraPos) {
		if (ply->nIsRagdolled) return;

		static auto tex = LoadTextureFromBFS("data/global/overlay/derby_health_indicator.tga");

		auto mat = *ply->pCar->GetMatrix();
		auto cameraDist = (cameraPos - mat.p).length();
		if (cameraDist > DamageIndicatorMaxDist) return;

		auto yUp = mat.y;
		if (yUp.y < 0) yUp.y *= -1;
		mat.p += yUp * ply->pCar->GetCenter()->y;
		mat.p += yUp * ply->pCar->GetRadius()->y;

		auto drawPos = Get3DTo2D(mat.p);
		if (drawPos.z <= 0) return;

		NyaDrawing::CNyaRGBA32 color = {255,255,255,255};
		auto damage = ply->pCar->GetDerbyDamage();
		if (damage <= 0.0) damage = 0.0;
		if (damage >= 1.0) damage = 1.0;
		if (damage < 0.5) {
			color.r = std::lerp(64, 255, damage * 2);
			color.g = std::lerp(255, 200, damage * 2);
			color.b = std::lerp(64, 60, damage * 2);
		}
		else {
			color.r = std::lerp(255, 255, damage - 0.5);
			color.g = std::lerp(200, 0, damage - 0.5);
			color.b = std::lerp(60, 0, damage - 0.5);
		}
		color.a = DamageIndicatorAlpha*255;

		auto fSize = (DamageIndicatorMaxDist - cameraDist) / DamageIndicatorMaxDist * DamageIndicatorScale;
		drawPos.y -= DamageIndicatorYOffset * fSize;
		if (ply->nPlayerType != PLAYERTYPE_LOCAL) DrawRectangle(drawPos.x - fSize * GetAspectRatioInv(), drawPos.x + fSize * GetAspectRatioInv(), drawPos.y - fSize, drawPos.y + fSize, color, 0, tex);

		if (bIsFragDerby) {
			int playerId = ply->nPlayerId - 1;
			if (nStreakerId == playerId && nStreakerId == nSurvivorId) {
				DrawPlayerIcon("frag_derby_symbol_combinated", fSize, drawPos);
			} else {
				if (nStreakerId == playerId) {
					DrawPlayerIcon("frag_derby_symbol_fragstreak", fSize, drawPos);
				}
				if (nSurvivorId == playerId) {
					DrawPlayerIcon("frag_derby_symbol_survivor", fSize, drawPos);
				}
			}
		}
	}

	void Process() override {
		if (!IsRaceHUDUp() || pGameFlow->nEventType != eEventType::DERBY || IsInSplitScreen()) {
			nStreakerId = -1;
			nSurvivorId = -1;
			return;
		}

		auto cam = pCameraManager->pCamera;
		if (!cam) return;
		auto mat = *cam->GetMatrix();

		std::vector<Player*> aPlayers;

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			aPlayers.push_back(GetPlayer(i));
		}

		std::sort(aPlayers.begin(), aPlayers.end(), [](Player *a, Player *b) {
			auto cameraPos = pCameraManager->pCamera->GetMatrix()->p;
			return (cameraPos - a->pCar->GetMatrix()->p).length() > (cameraPos - b->pCar->GetMatrix()->p).length();
		});

		for (auto& ply : aPlayers) {
			DrawPlayerHealthIcon(ply, mat.p);
		}
	}
} HUD_Derby_Overhead;