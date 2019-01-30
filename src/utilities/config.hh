#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <string>

#include "../sdk/misc/colour.hh"
#include "thirdparty/json.hh"

namespace config {
	enum class type { int8, int16, int32, int64, float_, string_, colour, array_ };

	class item {
	private:
		int8_t int8;
		int16_t int16;
		int32_t int32;
		int64_t int64;
		float float_;
		std::string string_;
		sdk::colour colour;
		std::vector<item *> array_;

		std::string name;
		type item_type;

	public:
		item(const std::string &name, int8_t default_value = 0) : name(name), int8(default_value), item_type(type::int8) { };
		item(const std::string &name, int16_t default_value = 0) : name(name), int16(default_value), item_type(type::int16) { };
		item(const std::string &name, int32_t default_value = 0) : name(name), int32(default_value), item_type(type::int32) { };
		item(const std::string &name, int64_t default_value = 0) : name(name), int64(default_value), item_type(type::int64) { };
		item(const std::string &name, float default_value = 0) : name(name), float_(default_value), item_type(type::float_) { };
		item(const std::string &name, const std::string &default_value = 0) : name(name), string_(default_value), item_type(type::string_) { };
		item(const std::string &name, const sdk::colour &default_value = 0) : name(name), colour(default_value), item_type(type::colour) { };
		item(const std::string &name, const std::vector<item *> &default_value = {}) : name(name), array_(default_value), item_type(type::array_) { };

		int8_t &get_int8() { return this->int8; }
		int16_t &get_int16() { return this->int16; }
		int32_t &get_int32() { return this->int32; }
		int64_t &get_int64() { return this->int64; }
		float &get_float() { return this->float_; }
		std::string &get_string() { return this->string_; }
		sdk::colour &get_colour() { return this->colour; }
		std::vector<item *> &get_array() { return this->array_; }

		std::string get_name() { return this->name; }
		type get_type() { return this->item_type; }

	};

	extern std::vector<item *> config_items;
	extern nlohmann::json config_json;

	void initialize();
	void shutdown();

	item *get(const std::string &name);
	item *create(const std::string &name, const type &item_type);

}