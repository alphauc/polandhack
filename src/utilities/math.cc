#include "math.hh"
#include <assert.h>

#define degrees_2_radians( x ) ( ( float )( x ) * ( float )( ( float )( 3.14159265358979323846f ) / 180.0f ) )

static void sine_cosine( float radians, float * sine, float * cosine ){
	*sine = ( float )sin( radians );
	*cosine = ( float )cos( radians );
}

void utilities::math::vector_angles( const sdk::vec3& forward, sdk::vec3 &angles ) {
	float tmp, yaw, pitch;

	if ( forward.y == 0 && forward.x == 0 ) {
		yaw = 0;
		if ( forward.x > 0 )
			pitch = 270;
		else
			pitch = 90;
	}
	else {
		yaw = ( atan2( forward.y, forward.x ) * 180 / 3.14159265358979323846f ); // lol, whats m_pi
		if ( yaw < 0 )
			yaw += 360;

		tmp = sqrt( forward.x * forward.x + forward.y * forward.y );
		pitch = ( atan2( -forward.x, tmp ) * 180 / 3.14159265358979323846f );
		if ( pitch < 0 )
			pitch += 360;
	}

	angles.x = pitch;
	angles.y = yaw;
	angles.x = 0;
}

sdk::vec3 utilities::math::calculate_angle( sdk::vec3 source, sdk::vec3 destination ) {
	sdk::vec3 ret;
	vector_angles( destination - source, ret );
	return ret;
}

float utilities::math::random_float( float min, float max ) {
	assert( max > min );
	float random = ( ( float )rand( ) ) / ( float )RAND_MAX;
	float range = max - min;
	return ( random*range ) + min;
}

__forceinline float DotProduct(const sdk::vec3& a, const sdk::vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

void utilities::math::vector_transform(const float* in1, const sdk::matrix3x4_t& in2, float* out) {
	out[0] = DotProduct((sdk::vec3)in1, (sdk::vec3)in2[0]) + in2[0][3];
	out[1] = DotProduct((sdk::vec3)in1, (sdk::vec3)in2[1]) + in2[1][3];
	out[2] = DotProduct((sdk::vec3)in1, (sdk::vec3)in2[2]) + in2[2][3];
}

void utilities::math::vector_transform(const sdk::vec3& in1, const sdk::matrix3x4_t& in2, sdk::vec3& out) {
	vector_transform(&in1.x, in2, &out.x);
}


sdk::vec3 utilities::math::normalize_angle(sdk::vec3 angle) {
	angle.x = normalize_pitch(angle.x);
	angle.y = normalize_yaw(angle.y);

	if ((angle.z > 50) || (angle.z < 50))
		angle.z = 0;

	return angle;
}

void utilities::math::angle_vectors( const sdk::vec3 &angles, sdk::vec3 *forward, sdk::vec3 *right, sdk::vec3 *up ) {
	float sr, sp, sy, cr, cp, cy;
	sine_cosine( degrees_2_radians( angles.y ), &sy, &cy );
	sine_cosine( degrees_2_radians( angles.x ), &sp, &cp );
	sine_cosine( degrees_2_radians( angles.z ), &sr, &cr );

	if ( forward ) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if ( right ) {
		right->x = ( -1 * sr*sp*cy + -1 * cr*-sy );
		right->y = ( -1 * sr*sp*sy + -1 * cr*cy );
		right->z = -1 * sr*cp;
	}

	if ( up ) {
		up->x = ( cr*sp*cy + -sr * -sy );
		up->y = ( cr*sp*sy + -sr * cy );
		up->z = cr * cp;
	}
}

float utilities::math::normalize_pitch( float pitch ) {
	while ( pitch > 89.f )
		pitch -= 180.f;
	while ( pitch < -89.f )
		pitch += 180.f;

	return pitch;
}

float utilities::math::normalize_yaw( float yaw ) {
	if ( yaw > 180 )
		yaw -= ( round( yaw / 360 ) * 360.f );
	else if ( yaw < -180 )
		yaw += ( round( yaw / 360 ) * -360.f );

	return yaw;
}
