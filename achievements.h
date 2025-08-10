namespace Achievements {
	bool IsRaceMode() {
		if (pGameFlow->nEventType != eEventType::RACE) return false;
		if (pGameFlow->nSubEventType != eSubEventType::RACE_NORMAL) return false;
		return true;
	}

	int nTotalProgression = 0;
	int nCurrentSaveSlot = 0;

	std::string GetAchievementSavePath(int id) {
		return std::format("Savegame/customsave{:03}.ach", id);
	}

	enum eAchievementCategory {
		CAT_GENERAL = 1,
		CAT_SINGLEPLAYER = 2,
		CAT_MULTIPLAYER = 4,
		CAT_CAREER = 8,
		CAT_GAMEMODES = 16,
		CAT_TRACKS = 32,
		CAT_HIDDEN = 64,
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
		new CAchievement("WIN_RACE", "Starting Point", "Win a race", CAT_GENERAL),
		//new CAchievement("WIN_RACE_WRECK", "Eliminator", "Win a race after wrecking everyone", CAT_SINGLEPLAYER),
		new CAchievement("WIN_RACE_NODAMAGE", "Not a Scratch", "Win a race without taking any damage", CAT_GENERAL),
		new CAchievement("BUY_CUSTOM_SKIN", "Community-Run", "Purchase a car with a custom livery", CAT_CAREER),
		new CAchievement("LOW_HP", "Dead Man Walking", "Win a race on less than 10%% health", CAT_GENERAL),
		new CAchievement("CASH_AWARD", "Makin' it Big", "Reach a total balance of 100,000CR", CAT_CAREER),
		new CAchievement("ALL_CARS", "Car Collector", "Unlock all cars in the game", CAT_GENERAL),
		new CAchievement("COMPLETE_CAREER", "Race Master", "Complete FlatOut mode", CAT_CAREER),
		new CAchievement("COMPLETE_CAREER_GOLD", "Race Wizard", "Complete FlatOut mode with all gold", CAT_CAREER),
		new CAchievement("TRACKMASTER", "FlatOut Map Veteran", "Win an event on every track", CAT_TRACKS),
		new CAchievement("WRECK_CAR_RACE", "Takedown", "Wreck an opponent in a race", CAT_TRACKS),
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
		return DrawString(data, text, &DrawStringFO2_Ingame12);
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

		if (achievement->bUnlocked) return;
		achievement->bUnlocked = true;
		Save(nCurrentSaveSlot);

		aUnlockBuffer.push_back(achievement);
	}

	void Load(int saveSlot) {
		for (auto& achievement : gAchievements) {
			achievement->nProgress = 0;
			achievement->fInternalProgress = 0;
			achievement->bUnlocked = false;
		}

		nCurrentSaveSlot = saveSlot;

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
		pThis->nProgress = (pThis->fInternalProgress / 0.9) * 100;

		if (GetGameState() == GAME_STATE_RACE) {
			static bool bLast = false;
			bool bCurrent = IsRaceMode() && GetPlayerScore<PlayerScoreRace>(1)->bHasFinished;
			if (bCurrent) {
				if (!bLast && GetPlayerScore<PlayerScoreRace>(1)->nPosition == 1 && !GetPlayerScore<PlayerScoreRace>(1)->bIsDNF) {
					auto damage = GetPlayer(0)->pCar->fDamage;
					if (damage > pThis->fInternalProgress) pThis->fInternalProgress = damage;
					if (damage >= 0.9) {
						AwardAchievement(pThis);
					}
				}
			}
			bLast = bCurrent;
		}
	}
	void OnTick_CashAward(CAchievement* pThis, double delta) {
		pThis->fInternalProgress = pGameFlow->Profile.nMoney;
		pThis->nProgress = (pThis->fInternalProgress / 100000.0) * 100;
		if (pGameFlow->Profile.nMoney >= 100000) AwardAchievement(pThis);
	}
	void OnTick_Trackmaster(CAchievement* pThis, double delta) {
		int numTracks = 0;
		int numTracksWon = 0;
		for (int i = 1; i < GetNumTracks() + 1; i++) {
			if (!DoesTrackExist(i)) continue;
			if (DoesTrackValueExist(i, "StuntMode")) continue;

			numTracks++;
			if (gCustomSave.tracksWon[i]) numTracksWon++;
		}

		pThis->fInternalProgress = numTracksWon;
		pThis->fMaxInternalProgress = numTracks;
	}
	void OnTick_BuyCustomSkin(CAchievement* pThis, double delta) {
		for (int i = 0; i < aDealerCars.size(); i++) {
			int carId = aDealerCars[i].carId;
			auto careerCar = &gCustomSave.aCareerGarage[carId-1];
			if (!careerCar->bIsPurchased) continue;
			if (GetSkinAuthor(carId, careerCar->nSkinId, false).empty()) continue;
			AwardAchievement(pThis);
		}
	}
	void OnTick_AllCars(CAchievement* pThis, double delta) {
		int numUnlocked = 0;
		for (int i = 0; i < aDealerCars.size(); i++) {
			int classId = aDealerCars[i].classId-1;
			if (classId < 0 || classId > 2) {
				numUnlocked++;
				break;
			}
			if (gCustomSave.bCareerClassUnlocked[classId]) numUnlocked++;
		}

		pThis->fInternalProgress = numUnlocked;
		pThis->fMaxInternalProgress = aDealerCars.size();
	}
	std::string OnTrack_GenericProgress(CAchievement* pThis) {
		return std::format("Progress: {:.0f}/{}", pThis->fInternalProgress, pThis->fMaxInternalProgress);
	}
	std::string OnTrack_GenericString(CAchievement* pThis) {
		return pThis->sTrackString;
	}
	std::string OnTrack_LowHP(CAchievement* pThis) {
		if (GetGameState() == GAME_STATE_RACE && IsRaceMode()) {
			return std::format("Health: {:.0f}%%", (1.0 - GetPlayer(0)->pCar->fDamage) * 100);
		}
		return "Health: N/A";
	}

	const float fTrackPosX = 0.04;
	const float fTrackPosY = 0.04;
	const float fTrackSize = 0.02;
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
			DrawString(data, achievement->sName, &DrawStringFO2_Ingame12);
			data.y += fTrackSpacing;
			DrawString(data, achievement->sDescription, &DrawStringFO2_Ingame12);
			data.y += fTrackSpacing;
			DrawString(data, string, &DrawStringFO2_Ingame12);
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

		if (GetGameState() == GAME_STATE_RACE) {
			auto ply = GetPlayerScore<PlayerScoreRace>(1);
			if (IsRaceMode() && ply->bHasFinished && pPlayerHost->GetNumPlayers() > 1) {
				if (ply->nPosition == 1) {
					AwardAchievement(GetAchievement("WIN_RACE"));
					if (GetPlayer(0)->pCar->fDamage <= 0.0) {
						AwardAchievement(GetAchievement("WIN_RACE_NODAMAGE"));
					}
					//if (bIsInMultiplayer) {
					//	AwardAchievement(GetAchievement("WIN_MP_RACE"));
					//}
					// SP and 8+ players only for the all wreck achievement
					if (pPlayerHost->GetNumPlayers() >= 8) {
						bool anyoneAlive = false;
						for (int i = 1; i < pPlayerHost->GetNumPlayers(); i++) {
							auto ply = GetPlayer(i);
							if (!ply) continue;
							if (ply->pCar->fDamage < 1.0) anyoneAlive = true;
						}
						if (!anyoneAlive) AwardAchievement(GetAchievement("WIN_RACE_WRECK"));
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
		GetAchievement("BUY_CUSTOM_SKIN")->pTickFunction = OnTick_BuyCustomSkin;
		GetAchievement("ALL_CARS")->pTickFunction = OnTick_AllCars;

		GetAchievement("LOW_HP")->pTrackFunction = OnTrack_LowHP;
	}
}

Achievements::CAchievement* GetAchievement(const std::string& identifier) {
	return Achievements::GetAchievement(identifier);
}