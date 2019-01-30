#pragma once
#include "../../cheat/cheat.hh"
#include "../misc/colour.hh"

namespace sdk {
	class console_variable : public base_interface {
	public:
		char pad_0x0000[0x4];
		console_variable* pNext;
		int32_t bRegistered;
		char* pszName;
		char* pszHelpString;
		int32_t nFlags;
		char pad_0x0018[0x4];
		console_variable* pParent;
		char* pszDefaultValue;
		char* strString;
		int32_t StringLength;
		float fValue;
		int32_t nValue;
		int32_t bHasMin;
		float fMinVal;
		int32_t bHasMax;
		float fMaxVal;
		void* fnChangeCallback;

		float GetFloat(void) const {
			auto temp = *(int*)(&fValue);
			auto temp_result = (int)(temp ^ (DWORD)this);

			return *(float*)(&temp_result);
		}

		int GetInt(void) const {
			auto temp = *(int*)(&fValue);
			auto temp_result = (int)(temp ^ (DWORD)this);

			return *(int*)(&temp_result);
		}

		bool GetBool(void) const {
			return !!GetInt();
		}

		void SetValue(const char* value) {
			return this->get_vfunc<14, void>(this, value);
		}
	};//Size=0x0048

	class convar : public base_interface {
	public:
		console_variable* find_variable(const char* var_name) {
			return this->get_vfunc<14, console_variable*>(this, var_name);
		}

		void ConsoleColorPrintf(const sdk::colour& col, const char* pFormat) {
			this->get_vfunc<24, void>(this, col, pFormat);
		}
	};
}
