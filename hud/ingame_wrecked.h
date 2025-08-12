class CHUD_Wrecked : public CIngameHUDElement {
public:
	std::string sNotif;
	double fNotifTimer = 0;

	float fNotifY = 0.1;
	float fNotifSize = 0.05;

	void DrawWreckedNotif() {
		tNyaStringData data;
		data.x = 0.5;
		data.y = fNotifY;
		data.size = fNotifSize;
		data.XCenterAlign = true;
		int a = 255;
		if (fNotifTimer >= 2.5) {
			a = (3 - fNotifTimer) * 2 * 255;
		}
		if (fNotifTimer <= 0.5) {
			a = fNotifTimer * 2 * 255;
		}
		data.SetColor(255,255,255,a);
		DrawStringFO2_Ingame12(data, sNotif);
	}

	virtual void Process() {
		if (!IsRaceHUDUp()) return;
		if (GetGameState() == GAME_STATE_REPLAY) return;

		static CNyaRaceTimer gTimer;
		gTimer.Process();
		if (fNotifTimer > 0) {
			fNotifTimer -= gTimer.fDeltaTime;
		}

		if (pPlayerHost->nRaceTime <= 0) {
			fNotifTimer = 0;
		}

		if (fNotifTimer > 0 && !GetScoreManager()->nHideRaceHUD) {
			DrawWreckedNotif();
		}
	}
} HUD_Wrecked;

void AddWreckedNotif(const std::string& player) {
	HUD_Wrecked.sNotif = std::format("{}\nIS WRECKED", player);
	HUD_Wrecked.fNotifTimer = 3;
}

void AddWreckedNotifSelf() {
	HUD_Wrecked.sNotif = "YOU ARE WRECKED!";
	HUD_Wrecked.fNotifTimer = 3;
}

void AddTimeoutNotif(const std::string& player) {
	HUD_Wrecked.sNotif = std::format("{}\nRAN OUT OF TIME", player);
	HUD_Wrecked.fNotifTimer = 3;
}

void AddTimeoutNotifSelf() {
	HUD_Wrecked.sNotif = "OUT OF TIME!";
	HUD_Wrecked.fNotifTimer = 3;
}