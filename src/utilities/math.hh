#pragma once
#include "../sdk/misc/vector.hh"
#include "../sdk/misc/matrix.hh"

namespace utilities::math {
	void vector_angles(const sdk::vec3 & forward, sdk::vec3 & angles);
	sdk::vec3 calculate_angle(sdk::vec3 source, sdk::vec3 destination);
	float random_float(float min, float max);
	void angle_vectors(const sdk::vec3 & angles, sdk::vec3 * forward, sdk::vec3 * right = nullptr, sdk::vec3 * up = nullptr);
	float normalize_pitch( float pitch );
	float normalize_yaw( float yaw );
	sdk::vec3 normalize_angle(sdk::vec3 angle);

	void vector_transform(const float* in1, const sdk::matrix3x4_t& in2, float* out);

	void vector_transform(const sdk::vec3& in1, const sdk::matrix3x4_t& in2, sdk::vec3& out);

	inline float rotated_lower_body_yaw( float lby, float yaw )
	{
		float delta = normalize_yaw( yaw - lby );
		if ( fabs( delta ) < 25.f )
			return lby;

		if ( delta > 0.f )
			return yaw + 25.f;

		return yaw;
	}
}