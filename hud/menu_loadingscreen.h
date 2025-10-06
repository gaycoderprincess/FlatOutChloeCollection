class CMenu_LoadingScreen : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_loadingscreen"; }

	bool bHasRun = false;

	static constexpr float fLoadingSpriteX = 0.07;
	static constexpr float fLoadingSpriteY = 0.8;
	static constexpr float fLoadingSpriteSize = 0.15;

	static constexpr float nLoadingLogoY = 64;

	static constexpr int nLoadingTextX = 1650;
	static constexpr int nLoadingTextY = 980;
	static constexpr float fLoadingTextSize = 0.04;

	struct tLoadingScreenTip {
		std::string title;
		std::string text;
	};
	static inline tLoadingScreenTip aTips[] = {
			//{"CRASH FLY-BY", "Crash into a car and catapult it into the air for three seconds to get the Crash Fly-by bonus."},
			//{"CHANGING CRASH CAMERA", "After a Crash-Out, press the NITRO button to switch between Driver Cam or Car Cam."},
			//{"SLOWING DOWN THE CRASH CAMERA", "Hold down the acceleration to enable slow-motion after a Crash-Out. Release it to revert back to normal time."},
			//{"HUD TUNING", "Customize the in-game messages and HUD elements shown via the Options menu. The Stunt tutorials can be turned off there as well."},
			{"HUD TUNING", "Customize the in-game messages and HUD elements shown via the Options menu."},
			//{"USE ANY CONTROLLER IN PARTY MODE", "Any controller that is plugged in can be used in Party Mode.\nHowever, the controller used cannot be changed during gameplay."},
			//{"CONTROL FLIGHT PATH WITH AEROBATICS", "Use steering to control your driver's direction during flight. But beware; using aerobatics increases drag, which slows you down!"},
			//{"NUDGE IT UP", "Press the LAUNCH button to exert a small upward force on your driver. This move can only be used once per launch."},
	};

	static inline tLoadingScreenTip aCrashBonusTips[] = {
			{"SUPER FLIP", "Smash into a car and toss it onto its roof to get the Super Flip bonus."},
			{"SLAM, POWERHIT, BLAST-OUT", "Dish out a high power impact to a car for a hit bonus.\nHit bonuses range from Slam (lowest) to Blast-Out (highest)."},
			{"CRASH OUT!", "Crash into other cars hard enough to send their driver flying and earn the Crash Out! bonus."},
			{"WRECKED!", "Destroy a competitor's car to get the Wrecked! bonus. "},
	};

	static inline tLoadingScreenTip aRaceTips[] = {
			//{"TOP DRIVERS: BEST WRECKER", "Do the most damage to competitors' cars and destroy them to be the Best Wrecker!"},
			//{"TOP DRIVERS: BLAST MASTER", "Deal out the hardest hits and get the most Blast-Out bonuses\nto show that you are the ultimate Blast Master!"},
			//{"TOP DRIVERS: FASTEST LAP", "Burn the nitro, cut the corners and drive FlatOut to get the Fastest Lap!"},
			//{"TOP DRIVERS: BULLDOZER", "Smash the surroundings and destroy the most scenery to win the Bulldozer title!"},
			{"NITRO: CRASH INTO AN OPPONENT", "Gain nitro by crashing into other cars. The more ferocious the impact, the more nitro awarded!"},
			{"NITRO: ROADSIDE OBJECTS", "Gain nitro by crashing into roadside objects. The more objects you hit, the more nitro rewarded."},
			{"NITRO: CRASH-OUT!", "You get a fixed amount of nitro when your driver flies out as a result of a big crash."},
			{"NITRO: BIG AIR", "Jump high and long to generate nitro. The longer the airtime, the more nitro gained. "},
			{"RESET BUTTON", "If you flip your car, crash, come off the track, or your driver crashes through the windshield,\npress the RESET button to reset your position back on the track with a rolling start."},
			{"RESET OFTEN", "If you drive out of the track, crash badly or drive slow enough,\nuse the brake to stop and press the RESET button. This is often much faster than trying to drive back."},
			{"WINDSHIELD CRASHES", "Big crashes can cause driver Crash-Outs, where the driver is thrown through the windshield.\nTo decrease the chances of Crash-Outs, upgrade the strength of your car."},
			{"POWERSLIDE", "Release acceleration, turn and reapply acceleration to powerslide.\nPowersliding enables you to maintain your speed and momentum and drive FlatOut!"},
			{"NITRO CORNERING", "Use the nitro to keep your car on the road during tight cornering.\nThis will allow you to corner at speeds not normally possible and recover from coming into a corner too fast."},
			{"HUG THE CORNERS", "Hug the inside of corners to maintain maximum speed when taking consecutive turns."},
			{"USE THE BRAKE!", "Learn to brake at the right time without losing too much speed.\nSometimes it's faster to brake before a corner than to drive it flat out.\nApplying the brakes midair helps to lose the speed efficiently after landing."},
			{"FAST START", "For the best start avoid full acceleration.\nInstead, use around 30-50% acceleration during the countdown before flooring the pedal."},
			{"HIT 'EM HIGH", "Use other cars as impact absorbers when crashing into obstacles.\nSlam into other cars to take corners at higher speeds."},
			{"SOMEBODY TO LEAN ON", "Use the terrain to your advantage!\nLean on your opponents to keep them from overtaking you or simply shove them into the nearest tree!"},
	};

	static inline tLoadingScreenTip aDerbyTips[] = {
			{"GO FOR POWERFUL HITS", "Get maximum velocity relative to your target before you hit.\nThe higher your speed, the more damage inflicted."},
	};

	static inline tLoadingScreenTip aWreckingDerbyTips[] = {
			{"WRECKING DERBY", "Contests in the Derby Arenas are scored by damage inflicted, wrecks caused and being in the top 3."},
			{"TACTICS", "There are multiple ways to win the derby. You get points for damage done, kills and surviving to the final three.\nAlthough you get bonus points for being in the last three, this isn't always necessary if you've dealt out loads of damage."},
	};

	static inline tLoadingScreenTip aBeatTheBombTips[] = {
			{"EVERY SECOND COUNTS", "Checkpoint times are added to the time you have left. Thus every second counts.\nCutting a second or two in the beginning of the run will give you those few seconds more in the end of the run."},
			{"COLLECT NITRO", "Maximize nitro gain during the run. Crash into scenery and find the big jumps.\nUse nitro to jump further and gain more nitro. Don’t overcollect! If your tank is full, save some nitro gathering places for the second lap."},
			{"TACTICAL NITRO USAGE", "Burn nitro before long straights to get the maximum benefits.\nMake sure tires touch the ground when you use nitro."},
	};

	static inline tLoadingScreenTip aTimeTrialTips[] = {
			{"OPTIMAL RACELINES", "Stay on the optimal racelines to improve your time.\nPlan ahead your route, cut corners and choose lines that help you retain your speed."},
			{"FIND SHORTCUTS", "The road isn’t always the fastest route.\nFind shortcuts and use them, but make sure that they don’t slow you down too much."},
			{"COLLECT NITRO", "Maximize nitro gain during the run. Crash into scenery and find the big jumps.\nUse nitro to jump further and gain more nitro. Don’t overcollect! If your tank is full, save some nitro gathering places for the second lap."},
			{"TACTICAL NITRO USAGE", "Burn nitro before long straights to get the maximum benefits.\nMake sure tires touch the ground when you use nitro."},
	};

	static inline tLoadingScreenTip aArcadeRaceTips[] = {
			{"CRASH 'EM ALL", "Crash into other cars to improve your score.\nWrecking other cars yields big scores, but even small crash bonuses yield some score."},
			{"SMASH 'EM ALL", "Smash the surroundings and destroy the scenery to improve your score."},
			{"FLY LIKE AN EAGLE", "Catch huge air to improve your score. The longer the air time, the higher your score."},
			{"STAY IN THE LEAD", "Stay in the lead to get 5x position bonus. 2nd place gives 4x score, 3rd-4th 3x, 5th-6th 2x and 7th-8th 1x score."},
			{"TIME IS OF THE ESSENCE", "Squeeze the last bits of score by passing as many checkpoints as possible.\nYou’ll get score from each checkpoint, and bonus time for passing them."},
	};

	static inline tLoadingScreenTip aFragDerbyTips[] = {
			//{"EXTRA LIVES (SP only)", "In single player Deathmatch Derby, you earn extra lives by the number of frags you get. The number of frags you need to score for another extra life increases steadily."},
			{"FRAG STREAK", "Get more frags without getting wrecked than anyone else to get on a Frag Streak!\nWhile on streak, you score double for each kill."},
			{"SURVIVOR", "Avoid getting fragged for the longest time to become the Survivor!\nWhile surviving, you earn bonus points for the time you survive."},
			{"VICTIM", "The player who gets fragged the most times is the Victim."},
			//{"POWERUP: POWER RAM", "Inflicts triple damage. Use it to deal powerful hits and collect easy frags."},
			//{"POWERUP: POWER ARMOR", "Triples your durability, making your car an armored fortress."},
			//{"POWERUP: INFINITRO", "Infinite nitro. Especially useful when driving poorly accelerating cars."},
			//{"POWERUP: REPAIR", "Returns your car to full strength. When in low health, avoid contact and wait for one of these."},
			//{"POWERUP: BOMB", "The ultimate wrecker! This baby delivers a punch that will send your competitors into oblivion in a blinding flash."},
			//{"POWERUP: MASS DRIVER", "Increases the mass of your car, making it easier to deliver heavy hits and decreasing the punch of the hits delivered by others."},
			//{"POWERUP: SCORE DOUBLER", "Doubles your score. The powerup of the tactical player. Get half the frags and still win. Especially useful when coupled with Frag Streak status for quadruple score."},
			{"TACTICS", "Target players with Frag Streak or Survivor.\nThis steals the score bonuses from them, and makes it easier for you to get the bonus status."},
			{"BE AGGRESSIVE", "Be aggressive. The faster you rack up kills, the better your chances to catch the Frag Streaker status."},
			//{"MANAGE YOUR LIVES (SP only)", "Cats may have nine lives, but initially you only have three. Make the most of them. Be aggressive to get extra lives, but play it safe when you’re close to running out of lives. Repairs and Power Armors will help you."},
			//{"THREAT FACTOR (SP only)", "In single player, the opponent threat level will increase constantly. Be aggressive early, and play it safer when the threat level increases to stay alive as long as possible."},
	};

	static inline tLoadingScreenTip aSmashySmashTips[] = {
			{"EXPLORE AROUND", "Track limits are a suggestion, explore and search for high-reward objects and larger groups if you can."},
	};

	// todo character bios
	static inline tLoadingScreenTip aMenuTips[] = {
			{"", ""}
	};

	static tLoadingScreenTip GetRandomLoadingScreenTip() {
		std::vector<tLoadingScreenTip> tips;
		for (auto& tip : aTips) {
			tips.push_back(tip);
		}
		if (!bIsTimeTrial && !bIsSmashyRace && pGameFlow->nEventType != eEventType::STUNT) {
			for (auto& tip : aCrashBonusTips) {
				tips.push_back(tip);
			}
		}
		if (pGameFlow->nEventType == eEventType::RACE) {
			for (auto& tip: aRaceTips) {
				tips.push_back(tip);
			}
			if (bIsTimeTrial) {
				for (auto& tip : aTimeTrialTips) {
					tips.push_back(tip);
				}
			}
			if (bIsSmashyRace) {
				for (auto& tip : aSmashySmashTips) {
					tips.push_back(tip);
				}
			}
			if (bIsCarnageRace) {
				for (auto& tip : aArcadeRaceTips) {
					tips.push_back(tip);
				}
			}
		}
		else if (pGameFlow->nEventType == eEventType::DERBY) {
			for (auto& tip: aDerbyTips) {
				tips.push_back(tip);
			}
			if (bIsWreckingDerby) {
				for (auto& tip : aWreckingDerbyTips) {
					tips.push_back(tip);
				}
			}
			if (bIsFragDerby) {
				for (auto& tip : aFragDerbyTips) {
					tips.push_back(tip);
				}
			}
		}
		return tips[rand()%tips.size()];
	}

	static inline std::string sTextureName = "data/menu/loading.tga";
	static const char* GetLoadingScreenForTrack(int track) {
		switch (track) {
			case TRACK_FOREST1A: return "forest1_1";
			case TRACK_FOREST1B: return "forest1_2";
			case TRACK_FOREST1C: return "forest1_3";
			case TRACK_FOREST2A: return "forest2_1";
			case TRACK_FOREST2B: return "forest2_2";
			case TRACK_FOREST2C: return "forest2_3";
			case TRACK_FOREST3A: return "forest3_1";
			case TRACK_FOREST3B: return "forest3_2";
			case TRACK_FOREST3C: return "forest3_3";
			case TRACK_PIT1A: return "pit1_1";
			case TRACK_PIT1B: return "pit1_2";
			case TRACK_PIT1C: return "pit1_3";
			case TRACK_PIT2A: return "pit2_1";
			case TRACK_PIT2B: return "pit2_2";
			case TRACK_PIT2C: return "pit2_3";
			case TRACK_PIT3A: return "pit3_1";
			case TRACK_PIT3B: return "pit3_2";
			case TRACK_PIT3C: return "pit3_3";
			case TRACK_RACING1A: return "racing1_1";
			case TRACK_RACING1B: return "racing1_2";
			case TRACK_RACING1C: return "racing1_3";
			case TRACK_RACING2A: return "racing2_1";
			case TRACK_RACING2B: return "racing2_2";
			case TRACK_RACING2C: return "racing2_3";
			case TRACK_TOWN2A: return "town2_1";
			case TRACK_TOWN2B: return "town2_2";
			case TRACK_TOWN2C: return "town2_3";
			case TRACK_TOWN3A: return "town3_1";
			case TRACK_TOWN3B: return "town3_2";
			case TRACK_TOWN3C: return "town3_3";
			case TRACK_WINTER1A: return "winter1_1";
			case TRACK_WINTER1B: return "winter1_2";
			case TRACK_WINTER1C: return "winter1_3";
			case TRACK_WINTER2A: return "winter2_1";
			case TRACK_WINTER2B: return "winter2_2";
			case TRACK_WINTER2C: return "winter2_3";
			case TRACK_ARENA1A: return "arena1";
			case TRACK_ARENA2A: return "arena2";
			case TRACK_ARENA3A: return "arena3";
			case TRACK_DERBY1A: return "derby1";
			case TRACK_DERBY2A: return "derby2";
			case TRACK_DERBY3A: return "derby3";
			case TRACK_LONGJUMP: return "longjump";
			case TRACK_HIGHJUMP: return "highjump";
			case TRACK_BULLSEYE: return "bullseye";
			case TRACK_BOWLING: return "bowling";
			case TRACK_DARTS: return "darts";
			case TRACK_CLOWN: return "clown";
			case TRACK_DESERT1A: return "loading_bg_desert1a";
			case TRACK_DESERT1B: return "loading_bg_desert1b";
			case TRACK_DESERT1C: return "loading_bg_desert1c";
			case TRACK_FIELDS1A: return "loading_bg_fields1a";
			case TRACK_FIELDS1B: return "loading_bg_fields1b";
			case TRACK_FIELDS1C: return "loading_bg_fields1c";
			case TRACK_FIELDS2A: return "loading_bg_fields2a";
			case TRACK_FIELDS2B: return "loading_bg_fields2b";
			case TRACK_FIELDS2C: return "loading_bg_fields2c";
			case TRACK_FO2FOREST1A: return "loading_bg_forest1a";
			case TRACK_FO2FOREST1B: return "loading_bg_forest1b";
			case TRACK_FO2FOREST1C: return "loading_bg_forest1c";
			case TRACK_FO2FOREST2A: return "loading_bg_forest2a";
			case TRACK_FO2FOREST2B: return "loading_bg_forest2b";
			case TRACK_FO2FOREST2C: return "loading_bg_forest2c";
			case TRACK_FO2DERBY1A: return "loading_bg_derby1";
			case TRACK_FO2DERBY2A: return "loading_bg_derby2";
			case TRACK_FO2DERBY3A: return "loading_bg_derby3";
			case TRACK_FO2ARENA1A: return "loading_bg_arena1";
			case TRACK_FO2ARENA2A: return "loading_bg_arena2";
			case TRACK_FO2ARENA3A: return "loading_bg_arena3";
			case TRACK_RETRODEMO1A:
			case TRACK_RETRODEMO1B:
				return "retrodemo1_bg";
			case TRACK_RETRODEMO2A:
			case TRACK_RETRODEMO2B:
				return "retrodemo2_bg";
			case TRACK_TOUGHTRUCKS1: return "toughtrucks1_bg";
			case TRACK_TOUGHTRUCKS2: return "toughtrucks2_bg";
			case TRACK_TOUGHTRUCKS3: return "toughtrucks3_bg";
			case TRACK_TOUGHTRUCKS4: return "toughtrucks4_bg";
			case TRACK_TOUGHTRUCKS5: return "toughtrucks5_bg";
			case TRACK_TOUGHTRUCKS6: return "toughtrucks6_bg";
			case TRACK_TOUGHTRUCKS7: return "toughtrucks7_bg";
			case TRACK_TOUGHTRUCKS8: return "toughtrucks8_bg";
			case TRACK_TOUGHTRUCKS9: return "toughtrucks9_bg";
			case TRACK_TOUGHTRUCKS10: return "toughtrucks10_bg";
			case TRACK_TOUGHTRUCKS11: return "toughtrucks11_bg";
			case TRACK_TOUGHTRUCKS12: return "toughtrucks12_bg";
			case TRACK_TOUGHTRUCKS13: return "toughtrucks13_bg";
			case TRACK_TOUGHTRUCKS14: return "toughtrucks14_bg";
			case TRACK_TOUGHTRUCKS15: return "toughtrucks15_bg";
			case TRACK_TOUGHTRUCKS16: return "toughtrucks16_bg";
			default:
				return nullptr;
		}
	}

	tDrawPositions1080p gLoadingTipTitle = {960,700,0.03};
	tDrawPositions1080p gLoadingTipText = {960,760,0.035};
	static inline tLoadingScreenTip gLoadingScreenTip;
	void DrawTip() const {
		if (gLoadingScreenTip.text.empty()) return;

		tNyaStringData data;
		//Draw1080pString(JUSTIFY_CENTER, data, gLoadingScreenTip.category, &DrawStringFO2_Italic24);
		data.x = gLoadingTipTitle.nPosX;
		data.y = gLoadingTipTitle.nPosY;
		data.size = gLoadingTipTitle.fSize;
		data.XCenterAlign = true;
		data.SetColor(GetPaletteColor(COLOR_MENU_YELLOW));
		Draw1080pString(JUSTIFY_CENTER, data, gLoadingScreenTip.title, &DrawStringFO2_Italic24);
		data.x = gLoadingTipText.nPosX;
		data.y = gLoadingTipText.nPosY;
		data.size = gLoadingTipText.fSize;
		data.SetColor(GetPaletteColor(COLOR_MENU_WHITE));
		Draw1080pString(JUSTIFY_CENTER, data, gLoadingScreenTip.text, &DrawStringFO2_Condensed12);
	}

	void Process() override {
		if (!bHasRun) {
			static auto tex = LoadTextureFromBFS("data/menu/copyright_cc.png");
			if (tex) {
				DrawRectangle(0, 1, 0, 1, {0,0,0,255});
				Draw1080pSprite(JUSTIFY_480P_CENTER, 0, 640, 0, 480, {255, 255, 255, 255}, tex);
			}
			bHasRun = true;
			return;
		}

		if (!nUseNewLoadingScreen) return;

		if (!pLoadingScreen) return;
		if (sTextureName.empty()) return;

		auto tex = LoadTextureFromBFS(sTextureName.c_str());
		if (!tex) return;

		static auto loadingAnim = LoadTextureFromBFS("data/menu/loading_anim.tga");
		static auto gameLogo = LoadTextureFromBFS("data/menu/flatout_logo.png");
		static std::vector<tHUDData> loadingAnims = LoadHUDData("data/menu/loading_anim.bed", "loading_anim");

		// assuming a size of 640x480
		float loadingAspect = 4.0 / 3.0;
		if (sTextureName.find("retrodemo") != std::string::npos || sTextureName.find("toughtrucks") != std::string::npos) loadingAspect = 16.0 / 9.0;

		float aspectModifier = (GetAspectRatio() / loadingAspect) - 1.0;
		if (sTextureName == "data/menu/loading.tga") aspectModifier = 0.0;
		DrawRectangle(0, 1, 0 - (aspectModifier * 0.5), 1 + (aspectModifier * 0.5), {255,255,255,255}, 0, tex);

		const float fLoadingTimerSpeed = 0.4;

		static CNyaTimer gTimer;

		static int nLoadingSprite = 0;
		static double fLoadingTimer = 0;
		fLoadingTimer += gTimer.Process();
		while (fLoadingTimer > fLoadingTimerSpeed) {
			nLoadingSprite++;
			if (nLoadingSprite > 3) nLoadingSprite = 0;
			fLoadingTimer -= fLoadingTimerSpeed;
		}

		DrawRectangle(1.0 - ((fLoadingSpriteX + fLoadingSpriteSize) * GetAspectRatioInv()), 1.0 - (fLoadingSpriteX * GetAspectRatioInv()), fLoadingSpriteY, fLoadingSpriteY + fLoadingSpriteSize, {255,255,255,255}, 0, loadingAnim, 0, loadingAnims[nLoadingSprite].min, loadingAnims[nLoadingSprite].max);

		int nLoadingLogoSizeX = 512;
		int nLoadingLogoSizeY = 166;
		Draw1080pSprite(JUSTIFY_CENTER, 960 - (nLoadingLogoSizeX/2), 960 + (nLoadingLogoSizeX/2), nLoadingLogoY, nLoadingLogoY + nLoadingLogoSizeY, {255,255,255,255}, gameLogo);

		DrawTip();

		tNyaStringData data;
		data.x = nLoadingTextX;
		data.y = nLoadingTextY;
		data.size = fLoadingTextSize;
		data.XRightAlign = true;
		Draw1080pString(JUSTIFY_RIGHT, data, "LOADING", &DrawStringFO2_Italic24);
	}

	static void OnLoadToMenu() {
		gLoadingScreenTip = {"", ""};

		// set image to loading.tga for going to the menu
		sTextureName = "data/menu/loading.tga";
	}

	static void OnLoadToRace() {
		gLoadingScreenTip = GetRandomLoadingScreenTip();

		// set image to track loading when loading into a race
		auto loading = GetLoadingScreenForTrack(pGameFlow->nLevel);
		if (loading) sTextureName = std::format("data/menu/bg/{}.tga", loading);
	}

	static inline uintptr_t OnLoadToMenuASM_jmp = 0x4C0610;
	static void __attribute__((naked)) OnLoadToMenuASM() {
		__asm__ (
			"pushad\n\t"
			"mov ecx, ebx\n\t"
			"call %1\n\t"
			"popad\n\t"

			"jmp %0\n\t"
				:
				:  "m" (OnLoadToMenuASM_jmp), "i" (OnLoadToMenu)
		);
	}

	static inline uintptr_t OnLoadToRaceASM_jmp = 0x4C0610;
	static void __attribute__((naked)) OnLoadToRaceASM() {
		__asm__ (
			"pushad\n\t"
			"mov ecx, ebx\n\t"
			"call %1\n\t"
			"popad\n\t"

			"jmp %0\n\t"
				:
				:  "m" (OnLoadToRaceASM_jmp), "i" (OnLoadToRace)
		);
	}

	void InitHooks() override {
		OnLoadToMenuASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x452458, &OnLoadToMenuASM);
		OnLoadToRaceASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x451CAE, &OnLoadToRaceASM);
	}
} Menu_LoadingScreen;