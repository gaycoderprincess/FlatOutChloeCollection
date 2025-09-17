namespace InstantAction {
	float fUpgradeLevel = 0.0;
	int nNumLaps = 3;

	struct tTrackEntry {
		int level;
		bool reversed;
	};

	std::vector<tTrackEntry> GetTracks(bool unplayedOnly, bool unwonOnly) {
		std::vector<tTrackEntry> aTracks;
		for (int i = 1; i < GetNumTracks()+1; i++) {
			if (unwonOnly && gCustomSave.tracksWon[i]) continue;
			if (!DoesTrackExist(i)) continue;
			if (!DoesTrackSupportAI(i)) continue;
			if (DoesTrackValueExist(i, "StuntMode")) continue;

			// skip derby tracks for the unplayed check
			if (unplayedOnly && !unwonOnly && (int)GetTrackValueNumber(i, "TrackType") == TRACKTYPE_DERBY) continue;

			bool addRegular = !unplayedOnly || !gCustomSave.bestLaps[i];
			bool addReversed = !unplayedOnly || !gCustomSave.bestLapsReversed[i];
			if (unplayedOnly && !unwonOnly && !gCustomSave.tracksWon[i]) {
				addRegular = true;
				addReversed = true;
			}

			if (addRegular) {
				aTracks.push_back({i, false});
			}
			if (DoesTrackSupportReversing(i) && addReversed) {
				aTracks.push_back({i, true});
			}
		}
		WriteLog(std::format("GetTracks({}, {})", unplayedOnly, unwonOnly));
		for (auto& track : aTracks) {
			WriteLog(std::format("{}{}", GetTrackName(track.level), track.reversed ? " REVERSED" : ""));
		}
		return aTracks;
	}

	tTrackEntry GetRandomLevel() {
		// prioritize entirely unplayed tracks, but if there's too few then do played but unwon tracks, then unplayed but won (reverse) tracks, afterwards all tracks
		std::vector<tTrackEntry> aTracks = GetTracks(true, true);
		if (aTracks.size() <= 3) aTracks = GetTracks(false, true);
		if (aTracks.size() <= 3) aTracks = GetTracks(true, false);
		if (aTracks.size() <= 3) aTracks = GetTracks(false, false);

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