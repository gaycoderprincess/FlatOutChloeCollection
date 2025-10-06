class CHUD_PlayerList : public CIngameHUDElement {
public:
	static constexpr float fPosX = 0.07;
	static constexpr float fPosXFragDerby = 0.14;
	static constexpr float fPosY = 0.3;
	static constexpr float fSize = 0.038;
	static constexpr float fSpacing = 0.03;
	static constexpr float fSpacingX = 0.006;

	// from easing-functions by nicolausYes
	static double easeInOutQuart(double t) {
		if (t < 0.5) {
			t *= t;
			return 8 * t * t;
		} else {
			t = (--t) * t;
			return 1 - 8 * t * t;
		}
	}

	void Reset() override {
		bEnabled = nPlayerListDefaultState;
	}

	void Process() override {
		if (!IsRaceHUDUp()) return;
		if (IsInSplitScreen()) return;
		if (pGameFlow->nEventType == eEventType::STUNT) return;
		if (bIsTimeTrial && !CareerMode::IsCareerTimeTrial()) return;

		static CNyaRaceTimer gTimer;
		static double fPlayerListState = 0;
		gTimer.Process();

		// todo rebindable?
		if (!pGameFlow->nIsPaused && (IsKeyJustPressed('L') || IsPadKeyJustPressed(GetNyaControllerKeyForAction(CONFIG_PLAYERLIST)))) {
			bEnabled = !bEnabled;
		}

		if (bEnabled && fPlayerListState < 1) fPlayerListState += gTimer.fDeltaTime * 2;
		if (!bEnabled && fPlayerListState > 0) fPlayerListState -= gTimer.fDeltaTime * 2;
		if (fPlayerListState < 0.333) fPlayerListState = 0.333;
		if (fPlayerListState > 1) fPlayerListState = 1;

		bool displayScores = bIsFragDerby || (bIsInMultiplayer && bIsCarnageRace);

		tNyaStringData data;
		data.x = std::lerp(-0.5, (displayScores ? fPosXFragDerby : fPosX) * fPlayerListState * GetAspectRatioInv(), easeInOutQuart(fPlayerListState));
		data.y = fPosY;
		if (IsInSplitScreen()) data.y *= 0.5;
		data.size = fSize;

		auto aScores = GetSortedPlayerScores();
		for (auto& ply : aScores) {
			auto string1 = std::format("{}.", (&ply - &aScores[0]) + 1);
			if (displayScores) {
				if (bIsFragDerby) string1 = std::format("{}  {}", FragDerby::nPlayerScore[ply->nPlayerId], string1);
				if (bIsCarnageRace) string1 = std::format("{}  {}", CarnageRace::nPlayerScore[ply->nPlayerId], string1);
			}
			auto string2 = std::format("{}", GetStringNarrow(GetPlayer(ply->nPlayerId)->sPlayerName.Get()));
			if (!IsInSplitScreen() && ply->nPlayerId == 0) {
				data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
			}
			else {
				data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
			}
			if (ply->bIsDNF || (bIsFragDerby && GetPlayer(ply->nPlayerId)->pCar->nIsRagdolled)) {
				data.SetColor(64,64,64,255);
			}
			data.XRightAlign = true;
			DrawStringFO2_Condensed12(data, string1);
			data.x += fSpacingX * GetAspectRatioInv();
			data.XRightAlign = false;
			DrawStringFO2_Condensed12(data, string2);
			data.x -= fSpacingX * GetAspectRatioInv();
			data.y += fSpacing;
		}
	}
} HUD_PlayerList;