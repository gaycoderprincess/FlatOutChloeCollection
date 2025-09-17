namespace InstantAction {
	float fUpgradeLevel = 0.0;
	int nNumLaps = 3;

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
		auto tracks = new bool[GetNumTracks()+1];
		auto tracksRev = new bool[GetNumTracks()+1];
		memset(tracks,0,GetNumTracks()+1);
		memset(tracksRev,0,GetNumTracks()+1);

		std::vector<tTrackEntry> aTracks;

		// first add all entirely unplayed tracks
		for (int i = 1; i < GetNumTracks()+1; i++) {
			if (!CanTrackBePicked(i)) continue;
			if (gCustomSave.tracksWon[i]) continue;

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
		if (aTracks.size() <= 3) {
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
		if (aTracks.size() <= 3) {
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
		if (aTracks.size() <= 3) {
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

	int GetRandomCar() {
		std::vector<int> aCars;
		for (auto& car : aDealerCars) {
			aCars.push_back(car.carId);
		}
		return aCars[rand()%aCars.size()];
	}
}