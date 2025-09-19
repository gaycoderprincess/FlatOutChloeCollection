void AddWreckedNotif(Player* pPlayer);
void AddTimeoutNotif(Player* pPlayer);

class CHUD_Wrecked : public CIngameHUDElement {
public:
	std::vector<std::string> aNotifs;
	double fNotifTimer = 0;

	float fNotifX = 0.5;
	float fNotifY = 0.1;
	float fNotifSize = 0.05;
	float fNotifSize2 = 0.1;

	static constexpr float fNotifFadeinStart = 3;
	static constexpr float fNotifFadeinEnd = 2.5;
	static constexpr float fNotifFadeinSpeed = 2;
	static constexpr float fNotifFadeoutStart = 0.5;
	static constexpr float fNotifFadeoutSpeed = 2;

	void DrawWreckedNotif() {
		tNyaStringData data;
		data.x = fNotifX;
		data.y = fNotifY;
		data.size = fNotifSize;
		data.XCenterAlign = true;
		int a = 255;
		if (fNotifTimer >= fNotifFadeinEnd) {
			a = (fNotifFadeinStart - fNotifTimer) * fNotifFadeinSpeed * 255;
			data.size = std::lerp(fNotifSize2, fNotifSize, a / 255.0);
		}
		if (fNotifTimer <= fNotifFadeoutStart) {
			a = fNotifTimer * fNotifFadeoutSpeed * 255;
		}
		data.SetColor(255,255,255,a);
		DrawStringFO2_Ingame12(data, aNotifs[0]);
	}

	void Init() override {
		ChloeEvents::PlayerWreckedEvent.AddHandler(AddWreckedNotif);
		ChloeEvents::DerbyTimeoutEvent.AddHandler(AddTimeoutNotif);
	}

	void Process() override {
		if (!IsRaceHUDUp()) return;
		if (IsInSplitScreen()) return;

		static CNyaRaceTimer gTimer;
		gTimer.Process();
		if (fNotifTimer > 0) {
			fNotifTimer -= gTimer.fDeltaTime;
			// increase speed if there's more than one
			if (aNotifs.size() > 1) {
				//fNotifTimer -= gTimer.fDeltaTime;
				if (fNotifTimer < fNotifFadeinEnd && fNotifTimer > fNotifFadeoutStart) {
					fNotifTimer = fNotifFadeoutStart;
				}
			}
			// play next in queue
			if (fNotifTimer <= 0) {
				if (!aNotifs.empty()) aNotifs.erase(aNotifs.begin());
				if (!aNotifs.empty()) fNotifTimer = fNotifFadeinStart;
			}
		}
		else if (!aNotifs.empty()) {
			fNotifTimer = fNotifFadeinStart;
		}

		if (pPlayerHost->nRaceTime <= 0) {
			fNotifTimer = 0;
			aNotifs.clear();
		}

		if (!aNotifs.empty()) {
			DrawWreckedNotif();
		}
	}

	void AddNotif(const std::string& notif) {
		if (!aNotifs.empty() && aNotifs[aNotifs.size()-1] == notif) return; // ignore immediate duplicates
		aNotifs.push_back(notif);
	}
} HUD_Wrecked;

void AddWreckedNotif(Player* pPlayer) {
	if (bIsFragDerby) return;

	if (pPlayer->nPlayerType == PLAYERTYPE_LOCAL) {
		HUD_Wrecked.AddNotif("YOU ARE WRECKED!");
	}
	else {
		HUD_Wrecked.AddNotif(std::format("{}\nIS WRECKED", GetStringNarrow(pPlayer->sPlayerName.Get())));
	}
}

void AddTimeoutNotif(Player* pPlayer) {
	if (pPlayer->nPlayerType == PLAYERTYPE_LOCAL) {
		HUD_Wrecked.AddNotif("OUT OF TIME!");
	}
	else {
		HUD_Wrecked.AddNotif(std::format("{}\nRAN OUT OF TIME", GetStringNarrow(pPlayer->sPlayerName.Get())));
	}
}

void AddTopBarNotif(const std::string& str) {
	HUD_Wrecked.AddNotif(str);
}