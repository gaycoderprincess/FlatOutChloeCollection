namespace Achievements {
	bool IsRaceMode() {
		if (pGameFlow->nEventType != eEventType::RACE) return false;
		if (pGameFlow->nSubEventType != eSubEventType::RACE_NORMAL) return false;
		if (CareerMode::IsCareerTimeTrial()) return false;
		if (bIsTimeTrial) return false;
		if (bIsCarnageRace) return false;
		if (bIsSmashyRace) return false;
		return true;
	}

	bool IsSurvivalDerby() {
		if (pGameFlow->nEventType != eEventType::DERBY) return false;
		if (bIsWreckingDerby) return false;
		if (bIsFragDerby) return false;
		return true;
	}

	int nTotalProgression = 0;

	enum eAchievementCategory {
		CAT_GENERAL = 1,
		CAT_SINGLEPLAYER = 2,
		CAT_MULTIPLAYER = 4,
		CAT_CAREER = 8,
		CAT_CARNAGE = 16,
		CAT_GAMEMODES = 32,
		CAT_TRACKS = 64,
		CAT_HIDDEN = 128,
	};

	class CAchievement {
	public:
		const char* sIdentifier;
		const char* sName;
		const char* sDescription;
		int nProgress = 0;
		float fInternalProgress = 0;
		float fMaxInternalProgress = 0;
		bool bUnlocked = false;
		bool bHidden = false;
		uint64_t nCategory = 0;
		void(*pTickFunction)(CAchievement*, double) = nullptr;
		std::string(*pTrackFunction)(CAchievement*) = nullptr;
		std::string sTrackString;

		bool bTracked = false;

		CAchievement() = delete;
		CAchievement(const char* identifier, const char* name, const char* description, uint64_t category, bool hidden = false) {
			sIdentifier = identifier;
			sName = name;
			sDescription = description;
			bHidden = hidden;
			nCategory = category;
		}
	};

	std::vector<CAchievement*> gAchievements = {
		new CAchievement("WIN_RACE", "Starting Point", "Win a race", CAT_GAMEMODES),
		//new CAchievement("WIN_RACE_WRECK", "Eliminator", "Win a race after wrecking everyone", CAT_SINGLEPLAYER),
		new CAchievement("WIN_MP_RACE", "Friendly Competition", "Win a multiplayer race", CAT_MULTIPLAYER),
		new CAchievement("WIN_DERBY_PEP", "Glass Cannon", "Win a Survival Derby with the Pepper", CAT_GAMEMODES),
		new CAchievement("WIN_RACE_NODAMAGE", "Not a Scratch", "Win a race without taking any damage", CAT_GAMEMODES),
		new CAchievement("WIN_CUP_PEPPER", "Real Habanero", "Win the Bronze Finals with a stock Pepper", CAT_CAREER),
		new CAchievement("WRECK_MP", "First Blood", "Wreck a car in multiplayer", CAT_MULTIPLAYER),
		new CAchievement("BLAST_MP", "Unfriendly Competition", "Get 200 crash bonuses in multiplayer", CAT_MULTIPLAYER),
		new CAchievement("BLAST_ALL", "Blast Master", "Get 500 crash bonuses", CAT_GENERAL),
		new CAchievement("BUY_CUSTOM_SKIN", "Community-Run", "Purchase a car with a custom livery", CAT_CAREER),
		new CAchievement("LOW_HP", "Dead Man Walking", "Win a race on less than 25% health", CAT_GENERAL),
		new CAchievement("CASH_AWARD", "Makin' it Big", "Reach a total balance of $50,000", CAT_CAREER),
		new CAchievement("ALL_CARS", "Car Collector", "Unlock all cars in the game", CAT_GENERAL),
		new CAchievement("COMPLETE_CAREER", "Race Master", "Complete career mode", CAT_CAREER),
		new CAchievement("COMPLETE_CAREER_GOLD", "Race Wizard", "Complete career mode with all gold", CAT_CAREER),
		new CAchievement("COMPLETE_CAREER_GOLD_WIN", "Race Legend", "Complete all career cups with all races won", CAT_CAREER, true),
		new CAchievement("COMPLETE_CARNAGE", "Carnage Master", "Complete Arcade Mode", CAT_CARNAGE),
		new CAchievement("COMPLETE_CARNAGE_GOLD", "Carnage Wizard", "Complete Arcade Mode with all gold", CAT_CARNAGE),
		new CAchievement("COMPLETE_CARNAGE_AUTHOR", "Carnage Legend", "Complete Arcade Mode with all author", CAT_CARNAGE, true),
		new CAchievement("TRACKMASTER", "FlatOut 1 Map Veteran", "Win an event on every FlatOut 1 track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_FO2", "FlatOut 2 Map Veteran", "Win an event on every FlatOut 2 track", CAT_TRACKS),
		new CAchievement("WRECK_CAR_RACE", "Takedown", "Wreck an opponent in a race", CAT_GENERAL),
		new CAchievement("CASH_DESTRUCTION", "Big Earner", "Earn over $4000 from a single career race", CAT_CAREER),
		new CAchievement("CARNAGE_FILL_BOARD", "Overkill", "Get 10 separate bonuses in a single combo", CAT_CARNAGE),
		new CAchievement("CARNAGE_MILLIONAIRE", "Carnage Millionaire", "Earn 1,000,000 points in Arcade Mode", CAT_CARNAGE),
		new CAchievement("EJECTED_ALL", "Windshield Tester", "Get ejected 25 times", CAT_GENERAL),
		new CAchievement("SPEEDRUN_CARNAGE", "Speedrunner", "Gold an Arcade Mode event in less than 1:15", CAT_CARNAGE),
		new CAchievement("AUTHOR_MEDAL", "Trackmaster", "Achieve an author score", CAT_CAREER | CAT_CARNAGE),
		//new CAchievement("CHANGE_MUSIC", "Your Own Jukebox", "Change a music playlist", CAT_GENERAL),
		new CAchievement("CHANGE_MUSIC_TRAX", "Your Own Jukebox", "Customize the OST via Chloe Trax", CAT_GENERAL),
		//new CAchievement("FRAGDERBY_NO_WRECKS", "Rasputin", "Win a Deathmatch Derby without dying", CAT_GAMEMODES),
		new CAchievement("BOWLING_STRIKE", "Like an Angel", "Get a strike in Bowling", CAT_GAMEMODES),
		new CAchievement("BOWLING_PERFECT", "Master Bowler", "Get a perfect score in Bowling", CAT_GAMEMODES, true),
		new CAchievement("FRAG_STREAK", "Killing Spree", "Get a 5 frag streak in Frag Derby", CAT_GAMEMODES),
		new CAchievement("SMASHY", "Smashy Smash", "Destroy 5,000 roadside objects", CAT_GENERAL),
		new CAchievement("FULL_UPGRADE", "Junkman", "Fully upgrade a car", CAT_CAREER),
	};

	std::vector<CAchievement*> GetAchievementsInCategory(uint32_t category) {
		std::vector<CAchievement*> achievements;
		for (auto& achievement : gAchievements) {
			if (achievement->bHidden && !achievement->bUnlocked) continue;
			if ((achievement->nCategory & category) != 0) {
				achievements.push_back(achievement);
			}
		}
		return achievements;
	}

	const float fSpriteBGX = 960;
	const float fSpriteBGY = 960;
	const float fSpriteFGX = 1187;
	const float fSpriteFGY = 953;
	const float fSpriteBGSX = 507;
	const float fSpriteFGSX = 202;
	const float fSpriteBGSY = 83;
	const float fSpriteFGSY = 159;
	const float fSpritePopTimerSpeed = 2;
	const float fSpritePopBalloonSize = 1.2;
	const float fSpriteExpandTimerSpeed = 1;
	const float fSpriteStayTimerSpeed = 1.0 / 6.0;
	const float fTextFGX = 740;
	const float fTextFGY = 942;
	const float fTextFGS = 0.03;
	const float fTextBGX = 740;
	const float fTextBGY = 972;
	const float fTextBGS = 0.025;

	double fSpritePopTimer = 0;
	double fSpriteExpandTimer = 0;
	double fSpriteStayTimer = 1;
	double fSpriteHideTimer = 1;
	std::string sTextTitle;
	std::string sTextDesc;
	bool bHiddenAchievementUnlocked = false;

	bool DrawAchievementSprite(float left, float right, float top, float bottom, NyaDrawing::CNyaRGBA32 rgb, TEXTURE_TYPE* texture) {
		left -= 960;
		right -= 960;
		left /= 1920.0;
		right /= 1920.0;
		top /= 1080.0;
		bottom /= 1080.0;

		// aspect correction
		left *= 16.0 / 9.0;
		right *= 16.0 / 9.0;
		left /= GetAspectRatio();
		right /= GetAspectRatio();

		// recenter
		left += 0.5;
		right += 0.5;
		return DrawRectangle(left, right, top, bottom, rgb, 0, texture);
	}

	void DrawAchievementString(tNyaStringData data, const std::string& text) {
		data.x -= 960;
		data.x /= 1920.0;
		data.y /= 1080.0;

		// aspect correction
		data.x *= 16.0 / 9.0;
		data.x /= GetAspectRatio();

		// recenter
		data.x += 0.5;
		return DrawString(data, text, &DrawStringFO2_Condensed12);
	}

	void Save(int saveSlot) {
		auto file = std::ofstream(GetAchievementSavePath(saveSlot), std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		for (auto& achievement : gAchievements) {
			if (!achievement->bUnlocked && achievement->fInternalProgress == 0.0) continue;

			WriteStringToFile(file, achievement->sIdentifier);
			file.write((char*)&achievement->fInternalProgress, sizeof(achievement->fInternalProgress));
			file.write((char*)&achievement->bUnlocked, sizeof(achievement->bUnlocked));
		}
	}

	void Delete(int saveSlot) {
		if (!std::filesystem::exists(GetAchievementSavePath(saveSlot))) return;
		std::filesystem::remove(GetAchievementSavePath(saveSlot));
	}

	void Clear() {
		for (auto& achievement : gAchievements) {
			achievement->nProgress = 0;
			achievement->fInternalProgress = 0;
			achievement->bUnlocked = false;
		}
	}

	std::vector<CAchievement*> aUnlockBuffer;
	auto gFailAchievement = CAchievement("ERROR", "ERROR", "ERROR", 0);

	CAchievement* GetAchievement(const std::string& identifier) {
		for (auto& achievement : gAchievements) {
			if (achievement->sIdentifier == identifier) return achievement;
		}

		static std::string failDesc;
		failDesc = "Failed to find achievement " + identifier;
		gFailAchievement.sDescription = failDesc.c_str();
		aUnlockBuffer.push_back(&gFailAchievement);
		return nullptr;
	}


	void AwardAchievement(CAchievement* achievement) {
		if (!achievement) return;
		if (nCurrentSaveSlot < 0) return;

		if (achievement->bUnlocked) return;
		achievement->bUnlocked = true;

		Save(nCurrentSaveSlot+1);

		aUnlockBuffer.push_back(achievement);
	}

	void Load(int saveSlot) {
		for (auto& achievement : gAchievements) {
			achievement->nProgress = 0;
			achievement->fInternalProgress = 0;
			achievement->bUnlocked = false;
		}

		auto file = std::ifstream(GetAchievementSavePath(saveSlot), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		auto identifier = ReadStringFromFile(file);
		while (!identifier.empty()) {
			if (auto achievement = GetAchievement(identifier)) {
				file.read((char*)&achievement->fInternalProgress, sizeof(achievement->fInternalProgress));
				file.read((char*)&achievement->bUnlocked, sizeof(achievement->bUnlocked));
			}
			else {
				file.read((char*)&gFailAchievement.fInternalProgress, sizeof(gFailAchievement.fInternalProgress));
				file.read((char*)&gFailAchievement.bUnlocked, sizeof(gFailAchievement.bUnlocked));
			}
			identifier = ReadStringFromFile(file);
		}
	}

	// from easing-functions by nicolausYes
	double easeInOutQuart(double t) {
		if (t < 0.5) {
			t *= t;
			return 8 * t * t;
		} else {
			t = (--t) * t;
			return 1 - 8 * t * t;
		}
	}

	int GetNumUnlockedAchievements() {
		int count = 0;
		for (auto& achievement : gAchievements) {
			if (achievement->bHidden) continue;
			if (achievement->bUnlocked) count++;
		}
		return count;
	}

	int GetNumUnlockedHiddenAchievements() {
		int count = 0;
		for (auto& achievement : gAchievements) {
			if (!achievement->bHidden) continue;
			if (achievement->bUnlocked) count++;
		}
		return count;
	}

	int GetNumVisibleAchievements() {
		int count = 0;
		for (auto& achievement : gAchievements) {
			if (achievement->bHidden) continue;
			count++;
		}
		return count;
	}

	int GetNumHiddenAchievements() {
		int count = 0;
		for (auto& achievement : gAchievements) {
			if (!achievement->bHidden) continue;
			count++;
		}
		return count;
	}

	void DrawUnlockUI() {
		if (fSpriteHideTimer >= 1 && fSpritePopTimer <= 0) {
			if (aUnlockBuffer.empty()) return;

			auto achievement = aUnlockBuffer[0];
			fSpritePopTimer = 0;
			fSpriteExpandTimer = 0;
			fSpriteStayTimer = 0;
			fSpriteHideTimer = 0;
			sTextTitle = achievement->sName;
			sTextDesc = achievement->sDescription;
			bHiddenAchievementUnlocked = achievement->bHidden;
			aUnlockBuffer.erase(aUnlockBuffer.begin());

			static auto sound = NyaAudio::LoadFile("data/sound/achievement/unlock.mp3");
			if (sound) {
				NyaAudio::SetVolume(sound, nIngameSfxVolume / 100.0);
				NyaAudio::Play(sound);
			}
		}

		static CNyaTimer timer;
		float delta = 0;
		bool isExpanding = false;
		// stage 4 - remove icon
		if (fSpriteHideTimer >= 1) {
			//if (fSpritePopTimer > 0.5) fSpritePopTimer = 0.5;
			fSpritePopTimer -= timer.Process() * fSpritePopTimerSpeed;
			delta = fSpritePopTimer;
		}
		// stage 3 - hide
		else if (fSpriteStayTimer >= 1) {
			fSpriteHideTimer += timer.Process() * fSpriteExpandTimerSpeed;
			delta = 1.0 - fSpriteHideTimer;
			isExpanding = true;
		}
		// stage 3 - stay for a few seconds
		else if (fSpriteExpandTimer >= 1) {
			fSpriteStayTimer += timer.Process() * fSpriteStayTimerSpeed;
			delta = 1.0;
			isExpanding = true;
		}
		// stage 2 - expand to size
		else if (fSpritePopTimer >= 1) {
			fSpriteExpandTimer += timer.Process() * fSpriteExpandTimerSpeed;
			delta = fSpriteExpandTimer;
			isExpanding = true;
		}
		// stage 1 - pop and deflate
		else {
			fSpritePopTimer += timer.Process() * fSpritePopTimerSpeed;
			delta = fSpritePopTimer;
		}

		static auto bgTex = LoadTexture("data/textures/achievement/bg.png");
		static auto bg2Tex = LoadTexture("data/textures/achievement/flowersponk.png");
		static auto fgTex = LoadTexture("data/textures/achievement/flowers.png");

		if (delta <= 0) delta = 0;
		if (delta >= 1) delta = 1;

		if (isExpanding) {
			float bgLeft = std::lerp(fSpriteBGX + (fSpriteBGSX * 0.5), fSpriteBGX - (fSpriteBGSX * 0.5), easeInOutQuart(delta));
			DrawAchievementSprite(bgLeft, fSpriteBGX + (fSpriteBGSX * 0.5), fSpriteBGY - (fSpriteBGSY * 0.5), fSpriteBGY + (fSpriteBGSY * 0.5), {255,255,255,255}, bgTex);
			float textAlpha = (delta - 0.9) * 10;
			delta = 1;

			// if we've expanded further than the flower's extents, start drawing the background
			if (bgLeft < fSpriteFGX - (fSpriteFGSX * 0.5)) {
				DrawAchievementSprite(fSpriteBGX - (fSpriteBGSX * 0.5), fSpriteBGX + (fSpriteBGSX * 0.5), fSpriteBGY - (fSpriteBGSY * 0.5), fSpriteBGY + (fSpriteBGSY * 0.5), {255,255,255,255}, bg2Tex);
			}

			if (textAlpha > 0) {
				if (fSpriteStayTimer > 2.0 / 3.0 && aUnlockBuffer.empty()) {
					if (bHiddenAchievementUnlocked) {
						sTextTitle = "Hidden achievement progress";
						sTextDesc = std::format("{}/{}", GetNumUnlockedHiddenAchievements(), GetNumHiddenAchievements());
					}
					else {
						sTextTitle = "Achievement progress";
						sTextDesc = std::format("{}/{}", GetNumUnlockedAchievements(), GetNumVisibleAchievements());
					}
				}

				tNyaStringData data;
				data.a = 255 * textAlpha;
				data.x = fTextFGX;
				data.y = fTextFGY;
				data.size = fTextFGS;
				DrawAchievementString(data, sTextTitle);
				data.x = fTextBGX;
				data.y = fTextBGY;
				data.size = fTextBGS;
				DrawAchievementString(data, sTextDesc);
			}
		}

		float fgSize = 1;
		if (delta <= 0.5) {
			float newDelta = delta * 2;
			fgSize = std::lerp(0, fSpritePopBalloonSize, easeInOutQuart(newDelta));
		}
		else {
			float newDelta = (delta - 0.5) * 2;
			fgSize = std::lerp(fSpritePopBalloonSize, 1, easeInOutQuart(newDelta));
		}
		fgSize *= 0.5;

		DrawAchievementSprite(fSpriteFGX - (fSpriteFGSX * fgSize), fSpriteFGX + (fSpriteFGSX * fgSize), fSpriteFGY - (fSpriteFGSY * fgSize), fSpriteFGY + (fSpriteFGSY * fgSize), {255,255,255,255}, fgTex);
	}

	void OnTick_LowHP(CAchievement* pThis, double delta) {
		pThis->nProgress = (pThis->fInternalProgress / 0.75) * 100;

		if (GetGameState() == GAME_STATE_RACE) {
			static bool bLast = false;
			bool bCurrent = IsRaceMode() && GetPlayerScore(1)->bHasFinished;
			if (bCurrent) {
				if (!bLast && GetPlayerScore(1)->nPosition == 1 && !GetPlayerScore(1)->bIsDNF) {
					auto damage = GetPlayer(0)->pCar->fDamage;
					if (damage > pThis->fInternalProgress) pThis->fInternalProgress = damage;
					if (damage >= 0.75) {
						AwardAchievement(pThis);
					}
				}
			}
			bLast = bCurrent;
		}
	}
	void OnTick_CashAward(CAchievement* pThis, double delta) {
		pThis->fInternalProgress = pGameFlow->Profile.nMoney;
		pThis->nProgress = (pThis->fInternalProgress / 50000.0) * 100;
		if (pGameFlow->Profile.nMoney >= 50000) AwardAchievement(pThis);
	}

	bool IsTrackValidForTrackmaster(int i, bool fo2Tracks) {
		if (!DoesTrackExist(i)) return false;
		if (DoesTrackValueExist(i, "StuntMode")) return false;
		if (DoesTrackValueExist(i, "NotInTrackmaster")) return false;
		if (DoesTrackValueExist(i, "IsFO2Track") != fo2Tracks) return false;
		if (!DoesTrackSupportAI(i)) return false;
		return true;
	}

	void OnTick_Trackmaster(CAchievement* pThis, double delta) {
		int numTracks = 0;
		int numTracksWon = 0;
		for (int i = 1; i < GetNumTracks() + 1; i++) {
			if (!IsTrackValidForTrackmaster(i, false)) continue;

			numTracks++;
			if (gCustomSave.tracksWon[i]) numTracksWon++;
		}

		pThis->fInternalProgress = numTracksWon;
		pThis->fMaxInternalProgress = numTracks;
	}
	void OnTick_TrackmasterFO2(CAchievement* pThis, double delta) {
		int numTracks = 0;
		int numTracksWon = 0;
		for (int i = 1; i < GetNumTracks() + 1; i++) {
			if (!IsTrackValidForTrackmaster(i, true)) continue;

			numTracks++;
			if (gCustomSave.tracksWon[i]) numTracksWon++;
		}

		pThis->fInternalProgress = numTracksWon;
		pThis->fMaxInternalProgress = numTracks;
	}
	void OnTick_BuyCustomSkin(CAchievement* pThis, double delta) {
		for (int i = 0; i < aDealerCars.size(); i++) {
			int carId = aDealerCars[i].carId;
			auto careerCar = &gCustomSave.aCareerGarage[carId];
			if (!careerCar->bIsPurchased) continue;
			if (GetSkinAuthor(carId, careerCar->nSkinId, false).empty()) continue;
			AwardAchievement(pThis);
		}
	}
	void OnTick_AllCars(CAchievement* pThis, double delta) {
		pThis->fInternalProgress = gCustomSave.nCarsUnlocked;
		pThis->fMaxInternalProgress = aDealerCars.size();
	}
	void OnTick_CarnageMillionaire(CAchievement* pThis, double delta) {
		pThis->fInternalProgress = gCustomSave.GetArcadeCareerScore();
	}
	void OnTick_EjectedAll(CAchievement* pThis, double delta) {
		static bool bLastEjected = false;
		if (pLoadingScreen || GetGameState() != GAME_STATE_RACE) return;

		auto bEjected = GetPlayer(0)->pCar->nIsRagdolled;
		if (pGameFlow->nEventType != eEventType::RACE) return;
		if (bEjected && !bLastEjected) {
			pThis->fInternalProgress += 1;
		}

		bLastEjected = bEjected;
	}
	void OnTick_BowlingStrike(CAchievement* pThis, double delta) {
		if (pLoadingScreen || GetGameState() != GAME_STATE_RACE) return;
		if (pGameFlow->nSubEventType != eSubEventType::STUNT_BOWLING) return;

		int numPlayers = 1;
		if (pGameFlow->nGameMode == eGameMode::SPLITSCREEN) numPlayers = pGameFlow->nNumSplitScreenPlayers;
		for (int i = 0; i < numPlayers; i++) {
			auto score = GetPlayerScore(i+1);
			if (score->nStuntPointsScore[0] >= 10 || score->nStuntPointsScore[1] >= 10 || score->nStuntPointsScore[2] >= 10) {
				AwardAchievement(pThis);
			}
		}
	}
	void OnTick_BowlingPerfect(CAchievement* pThis, double delta) {
		if (pLoadingScreen || GetGameState() != GAME_STATE_RACE) return;
		if (pGameFlow->nSubEventType != eSubEventType::STUNT_BOWLING) return;

		int numPlayers = 1;
		if (pGameFlow->nGameMode == eGameMode::SPLITSCREEN) numPlayers = pGameFlow->nNumSplitScreenPlayers;
		for (int i = 0; i < numPlayers; i++) {
			auto score = GetPlayerScore(i+1);
			if (score->nStuntPointsScore[0] >= 10 && score->nStuntPointsScore[1] >= 10 && score->nStuntPointsScore[2] >= 10) {
				AwardAchievement(pThis);
			}
		}
	}
	void OnTick_Smashy(CAchievement* pThis, double delta) {
		if (pThis->fInternalProgress < pThis->fMaxInternalProgress) pThis->bUnlocked = false;

		static int lastObjects = 0;
		if (pLoadingScreen || GetGameState() != GAME_STATE_RACE) return;

		int objects = 0;
		auto ply = GetPlayer(0)->pCar;
		for (int i = 0; i < 10; i++) {
			objects += ply->aObjectsSmashed[i];
		}
		if (objects > lastObjects) {
			pThis->fInternalProgress += objects - lastObjects;
		}
		lastObjects = objects;
	}
	void OnTick_FullUpgrade(CAchievement* pThis, double delta) {
		for (auto& car : gCustomSave.aCareerGarage) {
			if (!car.bIsPurchased) continue;
			bool fullyUpgraded = true;
			for (int i = 1; i < PlayerProfile::NUM_UPGRADES; i++) {
				// weird edge case here
				if (i == PlayerProfile::TURBO && car.IsUpgradePurchased(PlayerProfile::SUPERCHARGER)) continue;
				if (i == PlayerProfile::SUPERCHARGER && car.IsUpgradePurchased(PlayerProfile::TURBO)) continue;

				if (!car.IsUpgradePurchased(i)) fullyUpgraded = false;
			}
			if (!fullyUpgraded) continue;
			AwardAchievement(pThis);
		}
	}
	std::string OnTrack_GenericProgress(CAchievement* pThis) {
		return std::format("Progress: {:.0f}/{}", pThis->fInternalProgress, pThis->fMaxInternalProgress);
	}
	std::string OnTrack_GenericString(CAchievement* pThis) {
		return pThis->sTrackString;
	}
	std::string OnTrack_LowHP(CAchievement* pThis) {
		if (GetGameState() == GAME_STATE_RACE && IsRaceMode()) {
			return std::format("Health: {:.0f}%", (1.0 - GetPlayer(0)->pCar->fDamage) * 100);
		}
		return "Health: N/A";
	}
	std::string OnTrack_SpeedrunCarnage(CAchievement* pThis) {
		if (GetGameState() == GAME_STATE_RACE && bIsArcadeMode) {
			std::string timestr = GetTimeFromMilliseconds(75000 - pPlayerHost->nRaceTime, true);
			timestr.pop_back(); // remove trailing 0, the game has a tickrate of 100fps
			return std::format("Time Left: {}", timestr);
		}
		return "Time Left: N/A";
	}

	const float fTrackPosX = 0.04;
	const float fTrackPosY = 0.04;
	const float fTrackSize = 0.03;
	const float fTrackSpacing = 0.03;

	void DrawTrackUI() {
		tNyaStringData data;
		data.x = fTrackPosX * GetAspectRatioInv();
		data.y = fTrackPosY;
		data.size = fTrackSize;

		for (auto achievement: gAchievements) {
			if (!achievement->bTracked) continue;
			if (!achievement->pTrackFunction) continue;
			if (achievement->bUnlocked) continue;
			auto string = achievement->pTrackFunction(achievement);
			if (string.empty()) continue;
			DrawString(data, achievement->sName, &DrawStringFO2_Condensed12);
			data.y += fTrackSpacing;
			DrawString(data, achievement->sDescription, &DrawStringFO2_Condensed12);
			data.y += fTrackSpacing;
			DrawString(data, string, &DrawStringFO2_Condensed12);
			data.y += fTrackSpacing * 2;
		}
	}

	void OnTick() {
		nTotalProgression = ((double)GetNumUnlockedAchievements() / (double)GetNumVisibleAchievements()) * 100;

		static CNyaTimer gTimer;
		gTimer.Process();

		for (auto achievement: gAchievements) {
			if (!pLoadingScreen && achievement->pTickFunction) {
				achievement->pTickFunction(achievement, gTimer.fDeltaTime);
			}

			if (achievement->fMaxInternalProgress > 0) {
				achievement->nProgress = (achievement->fInternalProgress / achievement->fMaxInternalProgress) * 100;
				if (achievement->fInternalProgress >= (achievement->fMaxInternalProgress - 0.001)) {
					AwardAchievement(achievement);
				}
			}
		}

		if (pLoadingScreen) return;

		static double fTrackCheckTimer = 3;
		fTrackCheckTimer += gTimer.fDeltaTime;
		if (fTrackCheckTimer > 3) {
			if (auto achievement = GetAchievement("TRACKMASTER")) {
				achievement->sTrackString = "";
				if (!achievement->bUnlocked) {
					for (int i = 1; i < GetNumTracks() + 1; i++) {
						if (gCustomSave.tracksWon[i]) continue;
						if (!IsTrackValidForTrackmaster(i, false)) continue;

						if (!achievement->sTrackString.empty()) achievement->sTrackString += ", ";
						achievement->sTrackString += GetTrackName(i);
					}
					achievement->sTrackString = "Remaining: " + achievement->sTrackString;
					achievement->pTrackFunction = OnTrack_GenericString;
				}
			}
			if (auto achievement = GetAchievement("TRACKMASTER_FO2")) {
				achievement->sTrackString = "";
				if (!achievement->bUnlocked) {
					for (int i = 1; i < GetNumTracks() + 1; i++) {
						if (gCustomSave.tracksWon[i]) continue;
						if (!IsTrackValidForTrackmaster(i, true)) continue;

						if (!achievement->sTrackString.empty()) achievement->sTrackString += ", ";
						achievement->sTrackString += GetTrackName(i);
					}
					achievement->sTrackString = "Remaining: " + achievement->sTrackString;
					achievement->pTrackFunction = OnTrack_GenericString;
				}
			}
			fTrackCheckTimer = 0;
		}

		if (GetGameState() == GAME_STATE_RACE) {
			auto ply = GetPlayerScore(1);
			if (IsRaceMode() && ply->bHasFinished && pPlayerHost->GetNumPlayers() > 1) {
				if (ply->nPosition == 1) {
					AwardAchievement(GetAchievement("WIN_RACE"));
					if (GetPlayer(0)->pCar->fDamage <= 0.0) {
						AwardAchievement(GetAchievement("WIN_RACE_NODAMAGE"));
					}
					if (bIsInMultiplayer) {
						AwardAchievement(GetAchievement("WIN_MP_RACE"));
					}
					// SP and 8+ players only for the all wreck achievement
					//else if (pPlayerHost->GetNumPlayers() >= 8) {
					//	bool anyoneAlive = false;
					//	for (int i = 1; i < pPlayerHost->GetNumPlayers(); i++) {
					//		auto ply = GetPlayer(i);
					//		if (!ply) continue;
					//		if (!IsPlayerWrecked(ply)) anyoneAlive = true;
					//	}
					//	if (!anyoneAlive) AwardAchievement(GetAchievement("WIN_RACE_WRECK"));
					//}
				}
			}
			if (IsSurvivalDerby() && ply->bHasFinished && pPlayerHost->GetNumPlayers() > 1) {
				if (ply->nPosition == 1) {
					if (GetPlayer(0)->nCarId + 1 == CAR_PEPPER) {
						AwardAchievement(GetAchievement("WIN_DERBY_PEP"));
					}
				}
			}
		}

		DrawTrackUI();
		DrawUnlockUI();
	}

	void Init() {
		GetAchievement("LOW_HP")->pTickFunction = OnTick_LowHP;
		GetAchievement("CASH_AWARD")->pTickFunction = OnTick_CashAward;
		GetAchievement("TRACKMASTER")->pTickFunction = OnTick_Trackmaster;
		GetAchievement("TRACKMASTER_FO2")->pTickFunction = OnTick_TrackmasterFO2;
		GetAchievement("BUY_CUSTOM_SKIN")->pTickFunction = OnTick_BuyCustomSkin;
		GetAchievement("ALL_CARS")->pTickFunction = OnTick_AllCars;
		GetAchievement("CARNAGE_MILLIONAIRE")->pTickFunction = OnTick_CarnageMillionaire;
		GetAchievement("EJECTED_ALL")->pTickFunction = OnTick_EjectedAll;
		GetAchievement("BOWLING_STRIKE")->pTickFunction = OnTick_BowlingStrike;
		GetAchievement("BOWLING_PERFECT")->pTickFunction = OnTick_BowlingPerfect;
		GetAchievement("SMASHY")->pTickFunction = OnTick_Smashy;
		GetAchievement("FULL_UPGRADE")->pTickFunction = OnTick_FullUpgrade;

		GetAchievement("LOW_HP")->pTrackFunction = OnTrack_LowHP;
		GetAchievement("BLAST_MP")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("BLAST_ALL")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("EJECTED_ALL")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("SPEEDRUN_CARNAGE")->pTrackFunction = OnTrack_SpeedrunCarnage;
		GetAchievement("SMASHY")->pTrackFunction = OnTrack_GenericProgress;

		GetAchievement("BLAST_MP")->fMaxInternalProgress = 200;
		GetAchievement("BLAST_ALL")->fMaxInternalProgress = 500;
		GetAchievement("CASH_DESTRUCTION")->fMaxInternalProgress = 4000;
		GetAchievement("CARNAGE_MILLIONAIRE")->fMaxInternalProgress = 1000000;
		GetAchievement("EJECTED_ALL")->fMaxInternalProgress = 25;
		GetAchievement("SMASHY")->fMaxInternalProgress = 5000;

		ChloeEvents::SaveLoadedEvent.AddHandler(Load);
		ChloeEvents::SaveCreatedEvent.AddHandler(Save);
		ChloeEvents::SaveDeletedEvent.AddHandler(Delete);
		ChloeEvents::SaveClearedEvent.AddHandler(Clear);
		ChloeEvents::DrawAboveUIEvent.AddHandler(OnTick);
	}
}

Achievements::CAchievement* GetAchievement(const std::string& identifier) {
	return Achievements::GetAchievement(identifier);
}