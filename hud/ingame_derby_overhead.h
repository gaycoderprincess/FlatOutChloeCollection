class CHUD_Derby_Overhead : public CIngameHUDElement {
public:

	virtual void Init() {
		nHUDLayer = eHUDLayer::WORLD;
	}

	static inline float fPlayerHealthIconOffset = 1.3;
	static inline float fPlayerHealthIconSize = 0.17;
	static inline float fPlayerHealthIconMinSize = 0.01;
	static void DrawPlayerHealthIcon(Player* ply) {
		if (ply->nIsRagdolled) return;
		if (ply->nPlayerType == PLAYERTYPE_LOCAL) return;

		static auto tex = LoadTextureFromBFS("data/global/overlay/derby_health_indicator.png");

		auto mat = *ply->pCar->GetMatrix();
		mat.p += mat.y * fPlayerHealthIconOffset;
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

		float fSize = fPlayerHealthIconSize / drawPos.z;
		if (fSize < fPlayerHealthIconMinSize) fSize = fPlayerHealthIconMinSize;
		DrawRectangle(drawPos.x - fSize * GetAspectRatioInv(), drawPos.x + fSize * GetAspectRatioInv(), drawPos.y - fSize, drawPos.y + fSize, color, 0, tex);
	}

	static inline float fPlayerIconOffset = 1.7;
	static inline float fPlayerIconSize = 0.25;
	static void DrawPlayerIcon(const std::string& icon, Player* ply) {
		static auto tex = LoadTextureFromBFS("data/global/overlay/frag_derby_symbols.png");
		static auto texData = LoadHUDData("data/global/overlay/frag_derby_symbols.bed", "frag_derby_symbols");

		auto mat = *ply->pCar->GetMatrix();
		mat.p += mat.y * fPlayerHealthIconOffset;
		mat.p.y += fPlayerIconOffset - fPlayerHealthIconOffset;
		auto drawPos = Get3DTo2D(mat.p);
		if (drawPos.z <= 0) return;

		float fSize = fPlayerIconSize / drawPos.z;
		auto symbol = GetHUDData(texData, icon);
		DrawRectangle(drawPos.x - fSize * GetAspectRatioInv(), drawPos.x + fSize * GetAspectRatioInv(), drawPos.y - fSize, drawPos.y + fSize, {255,255,255,127}, 0, tex, 0, symbol->min, symbol->max);
	}

	int nStreakerId = -1;
	int nSurvivorId = -1;

	virtual void Reset() {
		nStreakerId = -1;
		nSurvivorId = -1;
	}

	virtual void Process() {
		if (!IsRaceHUDUp() || pGameFlow->nEventType != eEventType::DERBY || IsInSplitScreen()) {
			nStreakerId = -1;
			nSurvivorId = -1;
			return;
		}

		if (bIsFragDerby) {
			if (nStreakerId >= 0 && nStreakerId == nSurvivorId) {
				DrawPlayerIcon("frag_derby_symbol_combinated", GetPlayer(nStreakerId));
			} else {
				if (nStreakerId >= 0) {
					DrawPlayerIcon("frag_derby_symbol_fragstreak", GetPlayer(nStreakerId));
				}
				if (nSurvivorId >= 0) {
					DrawPlayerIcon("frag_derby_symbol_survivor", GetPlayer(nSurvivorId));
				}
			}
		}

		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			DrawPlayerHealthIcon(GetPlayer(i));
		}
	}
} HUD_Derby_Overhead;