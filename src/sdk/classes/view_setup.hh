#pragma once
#include "../misc/vector.hh"

namespace sdk {

	enum class blur_mode
	{
		disable = 1,
		game = 2,
		sfm = 3
	};

	class view_setup {
	public:
		int x;
		int	x_old;
		int y;
		int y_old;
		int width;
		int width_old;
		int height;
		int height_old;

		bool ortho;
		float orthol_left;
		float ortho_top;
		float ortho_right;
		float ortho_bottom;

	private:
		char pad1[0x7C];

	public:
		float fov;
		float fov_viewmodel;
		vec2 origin;
		vec3 angles;

		float _near;
		float _far;
		float near_viewmodel;
		float far_viewmodel;

		float aspect_ratio;
		float near_blur_depth;
		float near_focus_depth;
		float far_focus_depth;
		float far_blur_depth;
		float near_blur_radius;
		float far_blur_radius;
		int do_f_quality;
		blur_mode motion_blur_mode;

		float shutter_time;
		vec2 shutter_open_pos;
		vec3 shutter_open_ang;
		vec2 shutter_close_pos;
		vec3 shutter_close_ang;

		float off_center_top;
		float off_center_bottom;
		float off_center_left;
		float off_center_right;

		bool off_Center : 1;
		bool render_to_subrect_of_larger_screen : 1;
		bool bloom_and_tone_mapping : 1;
		bool depth_of_field : 1;
		bool hdr_target : 1;
		bool draw_world_normal : 1;
		bool cull_front_faces : 1;
		bool cache_full_scene_set : 1;
		bool render_flashlight_depth_translucents : 1;
	private:
		char pad2[0x40];
	};

}