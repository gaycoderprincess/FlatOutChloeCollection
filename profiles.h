tCustomSaveStructure gTempCustomSave;

bool LoadProfileForStats(int id) {
	gTempCustomSave.Load(id-1);
	return true;
}
bool IsProfileValid(int id) {
	if (!LoadProfileForStats(id)) return false;
	return true;
}
int GetProfileCupsCompleted(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.nCupsPassed;
}
int GetProfileCupsMax(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.nCupsMax;
}
int GetProfileCarsUnlocked(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.nCarsUnlocked;
}
int GetProfileProgress(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.nGameCompletion;
}