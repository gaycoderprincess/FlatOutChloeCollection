NyaMat4x4 mProjectionMatrix;
void __thiscall GetProjectionMatrix(float* pThis, float* a2) {
	memcpy(&mProjectionMatrix, a2, sizeof(mProjectionMatrix));
	FO2MatrixInvert(pThis, a2);
}

bool IsBehindCamera(NyaVec3 pos)
{
	// Time for my own bit of dumb code -zw
	auto camMat = pCameraManager->pCamera->GetMatrix();
	auto camDir = *camMat * NyaVec4(0, 0, 1, 0);

	auto fromCamToPos = pos - camMat->p;
	fromCamToPos.Normalize();

	return fromCamToPos.Dot(NyaVec3(camDir.x, camDir.y, camDir.z)) < 0.0;
}


NyaVec3 Get3DTo2D(NyaVec3 pos) {
	if (pLoadingScreen || !pCameraManager) return {0,0,0};
	auto cam = pCameraManager->pCamera;
	if (!cam) return {0,0,0};
	auto mat = *cam->GetMatrix();
	mat = mat.Invert();
	auto proj = mProjectionMatrix;
	auto out = (proj * mat * NyaVec4(pos, 1));

	out.x /= out.w;
	out.y /= out.w;
	out.z /= out.w;

	out.x *= 0.5;
	out.y *= -0.5;

	out.x += 0.5;
	out.y += 0.5;
	return NyaVec3(out.x, out.y, out.z);
}

void ApplyDraw3DPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4C118E, &GetProjectionMatrix);

	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43E9D4, &mallocHooked);
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43EBA7, &mallocHooked);
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43ECDC, &mallocHooked);
}