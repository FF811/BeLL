#ifndef OPENVR_INTEGRATION
#define OPENVR_INTEGRATION

#include "glm/glm.hpp"
#include <openvr.h>




class COpenVR {
	private:

		vr::IVRSystem *hmd;
		vr::TrackedDevicePose_t pose[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 pose_mat[vr::k_unMaxTrackedDeviceCount];
		vr::VRControllerState_t state[vr::k_unMaxTrackedDeviceCount];

		void handle_event(vr::VREvent_t& ev);
		void set_mat4(glm::mat4& m, const vr::HmdMatrix34_t &vm) const;
		void set_mat4(glm::mat4& m, const vr::HmdMatrix44_t &vm) const;


		int width;
		int height;


	public:
		COpenVR();
		~COpenVR();
		bool isEnabled() const { return (hmd != NULL); }
		int getWidth() const { return width; }
		int getHeight() const { return height; }

		bool enable();
		void disable();
		glm::mat4 get_projection(int eye, float clip_near, float clip_far) const;
		void get_projection_raw(int eye, float *values) const;
		glm::mat4 get_view(int eye) const ;
		const glm::mat4& get_tracking_pose(unsigned int idx) const;
		unsigned long get_controller_buttons(unsigned int idx) const;
		void begin_cycle();
		void composite(unsigned int tex_left, unsigned int tex_right);
};
#endif
/* !OPENVR_INTEGRARTION */
