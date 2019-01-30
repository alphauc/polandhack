#pragma once

#include "vector.hh"

namespace sdk {
	struct matrix3x4_t {
	public:

		matrix3x4_t() {
		}

		matrix3x4_t(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

		void Pad();
		void Pad2();

		void Init
		(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

		void Init(void*);

		inline float* operator[](int i) {
			return m[i];
		}

		inline const float* operator[](int i) const {
			return m[i];
		}

		inline float* Base() {
			return &m[0][0];
		}

		inline const float* Base() const {
			return &m[0][0];
		}

		inline vec3 operator*(const vec3& vVec) const {
			vec3 vRet;
			vRet.x = m[0][0] * vVec.x + m[0][1] * vVec.y + m[0][2] * vVec.z + m[0][3];
			vRet.y = m[1][0] * vVec.x + m[1][1] * vVec.y + m[1][2] * vVec.z + m[1][3];
			vRet.z = m[2][0] * vVec.x + m[2][1] * vVec.y + m[2][2] * vVec.z + m[2][3];

			return vRet;
		}

		matrix3x4_t& operator=(const matrix3x4_t& mOther) {
			m[0][0] = mOther.m[0][0];
			m[0][1] = mOther.m[0][1];
			m[0][2] = mOther.m[0][2];
			m[0][3] = mOther.m[0][3];

			m[1][0] = mOther.m[1][0];
			m[1][1] = mOther.m[1][1];
			m[1][2] = mOther.m[1][2];
			m[1][3] = mOther.m[1][3];

			m[2][0] = mOther.m[2][0];
			m[2][1] = mOther.m[2][1];
			m[2][2] = mOther.m[2][2];
			m[2][3] = mOther.m[2][3];

			m[3][0] = mOther.m[3][0];
			m[3][1] = mOther.m[3][1];
			m[3][2] = mOther.m[3][2];
			m[3][3] = mOther.m[3][3];

			return *this;
		}

		bool operator==(const matrix3x4_t& src) const {
			return !memcmp(src.m, m, sizeof(m));
		}

		void MatrixMul(const matrix3x4_t& vm, matrix3x4_t& out) const {
			out.Init(
				m[0][0] * vm.m[0][0] + m[0][1] * vm.m[1][0] + m[0][2] * vm.m[2][0] + m[0][3] * vm.m[3][0],
				m[0][0] * vm.m[0][1] + m[0][1] * vm.m[1][1] + m[0][2] * vm.m[2][1] + m[0][3] * vm.m[3][1],
				m[0][0] * vm.m[0][2] + m[0][1] * vm.m[1][2] + m[0][2] * vm.m[2][2] + m[0][3] * vm.m[3][2],
				m[0][0] * vm.m[0][3] + m[0][1] * vm.m[1][3] + m[0][2] * vm.m[2][3] + m[0][3] * vm.m[3][3],

				m[1][0] * vm.m[0][0] + m[1][1] * vm.m[1][0] + m[1][2] * vm.m[2][0] + m[1][3] * vm.m[3][0],
				m[1][0] * vm.m[0][1] + m[1][1] * vm.m[1][1] + m[1][2] * vm.m[2][1] + m[1][3] * vm.m[3][1],
				m[1][0] * vm.m[0][2] + m[1][1] * vm.m[1][2] + m[1][2] * vm.m[2][2] + m[1][3] * vm.m[3][2],
				m[1][0] * vm.m[0][3] + m[1][1] * vm.m[1][3] + m[1][2] * vm.m[2][3] + m[1][3] * vm.m[3][3],

				m[2][0] * vm.m[0][0] + m[2][1] * vm.m[1][0] + m[2][2] * vm.m[2][0] + m[2][3] * vm.m[3][0],
				m[2][0] * vm.m[0][1] + m[2][1] * vm.m[1][1] + m[2][2] * vm.m[2][1] + m[2][3] * vm.m[3][1],
				m[2][0] * vm.m[0][2] + m[2][1] * vm.m[1][2] + m[2][2] * vm.m[2][2] + m[2][3] * vm.m[3][2],
				m[2][0] * vm.m[0][3] + m[2][1] * vm.m[1][3] + m[2][2] * vm.m[2][3] + m[2][3] * vm.m[3][3],

				m[3][0] * vm.m[0][0] + m[3][1] * vm.m[1][0] + m[3][2] * vm.m[2][0] + m[3][3] * vm.m[3][0],
				m[3][0] * vm.m[0][1] + m[3][1] * vm.m[1][1] + m[3][2] * vm.m[2][1] + m[3][3] * vm.m[3][1],
				m[3][0] * vm.m[0][2] + m[3][1] * vm.m[1][2] + m[3][2] * vm.m[2][2] + m[3][3] * vm.m[3][2],
				m[3][0] * vm.m[0][3] + m[3][1] * vm.m[1][3] + m[3][2] * vm.m[2][3] + m[3][3] * vm.m[3][3]
			);
		}

		matrix3x4_t operator*(const matrix3x4_t& vm) const {
			matrix3x4_t ret;
			MatrixMul(vm, ret);
			return ret;
		}

		inline matrix3x4_t operator+(const matrix3x4_t& other) const {
			matrix3x4_t ret;
			for (int i = 0; i < 16; i++) {
				((float*)ret.m)[i] = ((float*)m)[i] + ((float*)other.m)[i];
			}
			return ret;
		}

	public:
		float m[4][4];
	};

	typedef float VMatrix[3][4];
}