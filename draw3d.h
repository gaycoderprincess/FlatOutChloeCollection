NyaMat4x4 mProjectionMatrix;
void __thiscall GetProjectionMatrix(float* pThis, float* a2) {
	memcpy(&mProjectionMatrix, a2, sizeof(mProjectionMatrix));
	FO2MatrixInvert(pThis, a2);
}

float f3D2DYDivByDistance = 2;
NyaVec3 Get3DTo2D(NyaMat4x4 posMat) {
	if (pLoadingScreen || !pCameraManager) return {0,0,0};
	auto cam = pCameraManager->pCamera;
	if (!cam) return {0,0,0};
	auto mat = *cam->GetMatrix();
	mat = mat.Invert();
	auto proj = mProjectionMatrix;
	proj = proj.Invert();
	auto out = (proj * mat * posMat).p;
	out.x /= out.z * f3D2DYDivByDistance;
	out.y /= out.z * f3D2DYDivByDistance;
	out.y *= -1;
	out.x += 0.5;
	out.y += 0.5;
	return out;
}

void ApplyDraw3DPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4C118E, &GetProjectionMatrix);

	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43E9D4, &mallocHooked);
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43EBA7, &mallocHooked);
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43ECDC, &mallocHooked);
}