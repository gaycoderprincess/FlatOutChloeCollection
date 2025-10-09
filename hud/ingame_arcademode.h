class CHUD_ArcadeMode : public CIngameHUDElement {
public:

	double fEarnPopupTime = 0;
	std::string sEarnPopupString;
	NyaDrawing::CNyaRGBA32 nEarnPopupRGB;

	void TriggerEarnPopup(const std::string& str, int medal) {
		fEarnPopupTime = 4;
		sEarnPopupString = str;
		nEarnPopupRGB = GetMedalRGB(medal);

		static auto sound = NyaAudio::LoadFile("data/sound/trophy.wav");
		if (sound) {
			NyaAudio::SetVolume(sound, nIngameSfxVolume / 100.0);
			NyaAudio::Play(sound);
		}
	}

	void Reset() override {
		fEarnPopupTime = 0;
	}

	void Process() override {
		if (!IsRaceHUDUp()) return;
		if (!bIsArcadeMode) return;

		auto event = ArcadeMode::pCurrentEvent;

		static int lastScore = 0;
		auto score = ArcadeMode::nCurrentEventScore;
		if (pPlayerHost->nRaceTime >= 0) {
			if (lastScore < event->aGoalScores[2] && score >= event->aGoalScores[2]) {
				TriggerEarnPopup("BRONZE EARNED", 3);
			}
			if (lastScore < event->aGoalScores[1] && score >= event->aGoalScores[1]) {
				TriggerEarnPopup("SILVER EARNED", 2);
			}
			if (lastScore < event->aGoalScores[0] && score >= event->aGoalScores[0]) {
				TriggerEarnPopup("GOLD EARNED", 1);
				if (pPlayerHost->nRaceTime <= 75000 && !bIsSmashyRace) {
					Achievements::AwardAchievement(GetAchievement("SPEEDRUN_CARNAGE"));
				}
				if (!gCustomSave.tracksWon[pGameFlow->nLevel]) {
					gCustomSave.tracksWon[pGameFlow->nLevel] = true;
					gCustomSave.Save();
				}
			}
			if (lastScore < event->nPlatinumScore && score >= event->nPlatinumScore) {
				TriggerEarnPopup("AUTHOR SCORE BEATEN", 4);
				Achievements::AwardAchievement(GetAchievement("AUTHOR_MEDAL"));
			}
			if (lastScore < event->nCommunityScore && score >= event->nCommunityScore) {
				TriggerEarnPopup("SUPER AUTHOR SCORE BEATEN", 5);
				//Achievements::AwardAchievement(GetAchievement("SAUTHOR_MEDAL"));
			}
		}
		else {
			fEarnPopupTime = 0;
		}
		lastScore = score;

		NyaDrawing::CNyaRGBA32 rgb = GetMedalRGB(3);
		uint32_t targetScore = event->aGoalScores[2]; // bronze
		std::string scoreTitle = "TARGET";
		if (score >= targetScore) { // silver
			targetScore = event->aGoalScores[1];
			rgb = GetMedalRGB(2);
		}
		if (score >= targetScore) { // gold
			targetScore = event->aGoalScores[0];
			rgb = GetMedalRGB(1);
		}
		if (score >= targetScore && event->nPlatinumScore > 0) { // author
			targetScore = event->nPlatinumScore;
			rgb = GetMedalRGB(4);
		}
		if (score >= targetScore && event->nCommunityScore > 0) { // super author
			targetScore = event->nCommunityScore;
			rgb = GetMedalRGB(5);
		}
		if (score >= targetScore && gCustomSave.aArcadeCareerScores[ArcadeMode::nCurrentEventId] > targetScore) { // PB
			targetScore = gCustomSave.aArcadeCareerScores[ArcadeMode::nCurrentEventId];
			rgb = {255,255,255,255};
			scoreTitle = "PERSONAL BEST";
		}

		int uiId = 1;
		if (bIsCarnageRace || bIsFragDerby) uiId = 2;
		DrawElement(uiId, scoreTitle, FormatScore(targetScore), rgb);

		static CNyaRaceTimer gTimer;
		gTimer.Process();
		if (fEarnPopupTime > 0) {
			auto rgb = nEarnPopupRGB;
			if (fEarnPopupTime < 0.5) rgb.a = std::lerp(0, 255, fEarnPopupTime * 2);
			if (fEarnPopupTime > 3.75) rgb.a = std::lerp(0, 255, (4 - fEarnPopupTime) * 4);
			DrawElementCenter(2.5, "CONGRATULATIONS!", sEarnPopupString, rgb);
			fEarnPopupTime -= gTimer.fDeltaTime;
		}
	}
} HUD_ArcadeMode;