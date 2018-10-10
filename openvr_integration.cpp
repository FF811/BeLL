#include "openvr_integration.h"


COpenVR::COpenVR() {
	hmd = nullptr;
	width = 0;
	height = 0;
}

COpenVR::~COpenVR() {
	disable();
}


void COpenVR::set_mat4(glm::mat4& m, const vr::HmdMatrix34_t &vm) const
{
	m[0][0] = vm.m[0][0];
	m[0][1] = vm.m[1][0];
	m[0][2] = vm.m[2][0];
	m[0][3] = 0.0f;

	m[1][0] = vm.m[0][1];
	m[1][1] = vm.m[1][1];
	m[1][2] = vm.m[2][1];
	m[1][3] = 0.0f;

	m[2][0] = vm.m[0][2];
	m[2][1] = vm.m[1][2];
	m[2][2] = vm.m[2][2];
	m[2][3] = 0.0f;

	m[3][0] = vm.m[0][3];
	m[3][1] = vm.m[1][3];
	m[3][2] = vm.m[2][3];
	m[3][3] = 1.0f;
}

void COpenVR::set_mat4(glm::mat4& m, const vr::HmdMatrix44_t &vm) const
{
	m[0][0] = vm.m[0][0];
	m[0][1] = vm.m[1][0];
	m[0][2] = vm.m[2][0];
	m[0][3] = vm.m[3][0];

	m[1][0] = vm.m[0][1];
	m[1][1] = vm.m[1][1];
	m[1][2] = vm.m[2][1];
	m[1][3] = vm.m[3][1];

	m[2][0] = vm.m[0][2];
	m[2][1] = vm.m[1][2];
	m[2][2] = vm.m[2][2];
	m[2][3] = vm.m[3][2];

	m[3][0] = vm.m[0][3];
	m[3][1] = vm.m[1][3];
	m[3][2] = vm.m[2][3];
	m[3][3] = vm.m[3][3];
}


bool COpenVR::enable()
{

	if (isEnabled()) {
		return true;
	}

	vr::EVRInitError eError = vr::VRInitError_None;
	hmd = vr::VR_Init(&eError, vr::VRApplication_Scene);
	if (eError != vr::VRInitError_None) {
		hmd = NULL;
		return false;
	}

	if (!vr::VRCompositor())
	{

		return false;
	}

	uint32_t w, h;
	hmd->GetRecommendedRenderTargetSize(&w, &h);
	width = (int)w;
	height = (int)h;

	return true;

}

void COpenVR::disable()
{
	if (isEnabled()) {

		vr::VR_Shutdown();
		hmd = NULL;
	}
}


void COpenVR::handle_event(vr::VREvent_t& ev)
{
	switch (ev.eventType)
	{
	case vr::VREvent_TrackedDeviceActivated:
	{
		//SetupRenderModelForTrackedDevice(event.trackedDeviceIndex);
		//dprintf("Device %u attached. Setting up render model.\n", event.trackedDeviceIndex);
	}
	break;
	case vr::VREvent_TrackedDeviceDeactivated:
	{
		//dprintf("Device %u detached.\n", event.trackedDeviceIndex);
	}
	break;
	case vr::VREvent_TrackedDeviceUpdated:
	{
		//dprintf("Device %u updated.\n", event.trackedDeviceIndex);
	}
	break;
	}
}


void COpenVR::begin_cycle()
{
	if (!isEnabled()) {
		return;
	}


	// Process SteamVR events
	vr::VREvent_t event;
	while (hmd->PollNextEvent(&event, sizeof(event)))
	{
		handle_event(event);
	}

	// Process SteamVR controller state
	for (vr::TrackedDeviceIndex_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		hmd->GetControllerState(i, &state[i], sizeof(state[i]));
	}

	/* wait for next poses */
	vr::VRCompositor()->WaitGetPoses(pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	for (unsigned int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
	{
		if (pose[nDevice].bPoseIsValid)
		{
			set_mat4(pose_mat[nDevice],pose[nDevice].mDeviceToAbsoluteTracking);
			/*
			printf("%d XXXX\n", nDevice);
			pose_mat[nDevice].print("dev");
			printf("\n");
			*/
		}
	}

}

void COpenVR::composite(unsigned int tex_left, unsigned int tex_right)
{
	if (!isEnabled()) {
		return;
	}

	vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)tex_left, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)tex_right, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

}

glm::mat4 COpenVR::get_projection(int eye, float clip_near, float clip_far) const
{
	glm::mat4 m;

	if (!isEnabled()) {
		return glm::mat4(1.0f);
	}

	if (eye == 1) {
		set_mat4(m,hmd->GetProjectionMatrix(vr::EVREye::Eye_Right, clip_near, clip_far));
	}
	else {
		set_mat4(m, hmd->GetProjectionMatrix(vr::EVREye::Eye_Left, clip_near, clip_far));
	}


	return m;

}

void COpenVR::get_projection_raw(int eye, float *values) const
{
	glm::mat4 m;

	if (!isEnabled()) {
		values[0] = -1.0f;
		values[1] = 1.0f;
		values[2] = -1.0f;
		values[3] = 1.0f;
		return;
	}


}


glm::mat4 COpenVR::get_view(int eye) const
{
	glm::mat4 m;

	if (isEnabled()) {

		glm::mat4 pos;
		if (eye == 1) {
			set_mat4(pos,hmd->GetEyeToHeadTransform(vr::EVREye::Eye_Right));
		}
		else {
			set_mat4(pos,hmd->GetEyeToHeadTransform(vr::EVREye::Eye_Left));
		}
		m = glm::inverse(pos) * glm::inverse(pose_mat[vr::k_unTrackedDeviceIndex_Hmd]);

	} else {
		m=glm::mat4(1.0f);
	}
	return m;
}

const glm::mat4& COpenVR::get_tracking_pose(unsigned int idx) const
{

	if (idx >= vr::k_unMaxTrackedDeviceCount) {
		idx = 0;
	}
	return pose_mat[idx];


}

unsigned long COpenVR::get_controller_buttons(unsigned int idx) const
{

	if (idx >= vr::k_unMaxTrackedDeviceCount) {
		idx = 0;
	}

	return (state[idx].ulButtonPressed) ? 1 : 0;



}

