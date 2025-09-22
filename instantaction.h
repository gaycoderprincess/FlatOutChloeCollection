namespace InstantAction {
	struct tTrackEntry {
		int level;
		bool reversed;
	};

	bool CanTrackBePicked(int i) {
		if (!DoesTrackExist(i)) return false;
		if (!DoesTrackSupportAI(i)) return false;
		if (DoesTrackValueExist(i, "StuntMode")) return false;
		if (DoesTrackValueExist(i, "NotInInstantAction")) return false;
		return true;
	}

	tTrackEntry GetRandomLevel() {
		const int minAmount = 3;
		
		auto tracks = new bool[GetNumTracks()+1];
		auto tracksRev = new bool[GetNumTracks()+1];
		memset(tracks,0,GetNumTracks()+1);
		memset(tracksRev,0,GetNumTracks()+1);

		std::vector<tTrackEntry> aTracks;

		// first add all entirely unplayed tracks
		for (int i = 1; i < GetNumTracks()+1; i++) {
			if (!CanTrackBePicked(i)) continue;
			if (gCustomSave.tracksWon[i]) continue;
			
			// derby tracks don't have best laps counted, won't know if they've been played or not
			if ((int)GetTrackValueNumber(i, "TrackType") == TRACKTYPE_DERBY) continue;

			if (!gCustomSave.bestLaps[i]) {
				tracks[i] = true;
				aTracks.push_back({i, false});
			}
			if (DoesTrackSupportReversing(i)) {
				if (!gCustomSave.bestLapsReversed[i]) {
					tracksRev[i] = true;
					aTracks.push_back({i, true});
				}
			}
		}

		WriteLogDebug("INSTANTACTION", "InstantAction::GetRandomLevel()");
		for (auto& track : aTracks) {
			WriteLogDebug("INSTANTACTION", std::format("{}{}", GetTrackName(track.level), track.reversed ? " REVERSED" : ""));
		}

		// add played but unwon tracks
		if (aTracks.size() < minAmount) {
			for (int i = 1; i < GetNumTracks()+1; i++) {
				if (!CanTrackBePicked(i)) continue;
				if (gCustomSave.tracksWon[i]) continue;

				if (!tracks[i]) {
					tracks[i] = true;
					aTracks.push_back({i, false});
				}
				if (DoesTrackSupportReversing(i)) {
					if (!tracksRev[i]) {
						tracksRev[i] = true;
						aTracks.push_back({i, true});
					}
				}
			}

			WriteLogDebug("INSTANTACTION", "not enough tracks, searching unwon tracks");
			for (auto& track : aTracks) {
				WriteLogDebug("INSTANTACTION", std::format("{}{}", GetTrackName(track.level), track.reversed ? " REVERSED" : ""));
			}
		}

		// add unplayed reverse tracks
		if (aTracks.size() < minAmount) {
			for (int i = 1; i < GetNumTracks()+1; i++) {
				if (tracksRev[i]) continue;
				if (!CanTrackBePicked(i)) continue;
				if (!DoesTrackSupportReversing(i)) continue;
				if (gCustomSave.bestLapsReversed[i]) continue;

				tracksRev[i] = true;
				aTracks.push_back({i, true});
			}

			WriteLogDebug("INSTANTACTION", "not enough tracks, searching unplayed reverse tracks");
			for (auto& track : aTracks) {
				WriteLogDebug("INSTANTACTION", std::format("{}{}", GetTrackName(track.level), track.reversed ? " REVERSED" : ""));
			}
		}

		// add all tracks
		if (aTracks.size() < minAmount) {
			WriteLogDebug("INSTANTACTION", "not enough unplayed tracks, adding all tracks to selection");

			for (int i = 1; i < GetNumTracks()+1; i++) {
				if (!CanTrackBePicked(i)) continue;

				if (!tracks[i]) {
					tracks[i] = true;
					aTracks.push_back({i, false});
				}
				if (DoesTrackSupportReversing(i)) {
					if (!tracksRev[i]) {
						tracksRev[i] = true;
						aTracks.push_back({i, true});
					}
				}
			}
		}

		return aTracks[rand()%aTracks.size()];
	}

	bool carAlreadyPicked[1024] = {};
	int GetRandomCar() {
		std::vector<int> aCars;
		for (auto& car : aDealerCars) {
			int i = car.carId;
			if (!carAlreadyPicked[i]) aCars.push_back(i);
		}
		if (aCars.empty()) {
			memset(carAlreadyPicked, 0, 1024);
			return GetRandomCar();
		}
		return aCars[rand()%aCars.size()];
	}

	struct tEvent {
		float fUpgradeLevel = 0.0;
		int nNumLaps = 3;
		int nLevel = 0;
		bool bLevelReversed = false;
		int nCar = 0;
		int nCarSkin = 1;
		bool bDerbyTrack = false;
		bool bWreckingDerby = false;
		bool bFragDerby = false;
		bool bArcadeRace = false;
	} gEvent;

	void GenerateEvent() {
		gEvent = tEvent();

		switch (rand() % 3) {
			case 0:
				gEvent.fUpgradeLevel = 0.0;
				break;
			case 1:
				gEvent.fUpgradeLevel = 0.5;
				break;
			case 2:
				gEvent.fUpgradeLevel = 1.0;
				break;
		}
		gEvent.nNumLaps = (rand() % 4) + 2;

		auto level = InstantAction::GetRandomLevel();
		auto car = InstantAction::GetRandomCar();
		gEvent.nLevel = level.level;
		gEvent.bLevelReversed = level.reversed;
		gEvent.nCar = car;
		gEvent.nCarSkin = (rand()%GetNumSkinsForCar(car))+1;

		if ((int)GetTrackValueNumber(level.level, "TrackType") == TRACKTYPE_DERBY) {
			gEvent.bDerbyTrack = true;
			switch (rand() % 3) {
				// survivor derby
				case 0:
					break;
				case 1:
					gEvent.bWreckingDerby = true;
					break;
				case 2:
					gEvent.bFragDerby = true;
					break;
			}
		}
		else {
			if (gCustomSave.tracksWon[gEvent.nLevel]) {
				// 33% chance of arcade race
				gEvent.bArcadeRace = rand() % 3 == 0;
			}
		}
	}
}