#include <fstream>

#include "config.hh"
#include "console.hh"

std::vector<config::item *> config::config_items;
nlohmann::json config::config_json;

void config::initialize() {
	std::ifstream config_file("config.json");

	try {
		config_file >> config_json;

		static std::function<item *(const nlohmann::json &)> parse_item = [&](const nlohmann::json &node) -> item * {
			auto result = create(node.at("name").get<std::string>(), (type) std::stoi(node.at("type").get<std::string>()));

			switch (result->get_type()) {
			case type::int8:	result->get_int8() = node.at("value").get<int8_t>(); break;
			case type::int16:	result->get_int16() = node.at("value").get<int16_t>(); break;
			case type::int32:	result->get_int32() = node.at("value").get<int32_t>(); break;
			case type::int64:	result->get_int64() = node.at("value").get<int64_t>(); break;
			case type::float_:	result->get_float() = node.at("value").get<float>(); break;
			case type::string_: result->get_string() = node.at("value").get<std::string>(); break;
			case type::colour:	{
					sdk::colour value(sdk::colour::white);

					value.r = node.at("value").at("r").get<int32_t>();
					value.g = node.at("value").at("g").get<int32_t>();
					value.b = node.at("value").at("b").get<int32_t>();

					if (node.at("value").find("a") != node.end())
						value.a = node.at("value").at("a").get<int32_t>();

					result->get_colour() = value;
					break;
				}
			case type::array_:	{
					std::vector<item *> value {};

					for (const auto &sub_node : node) {
						value.emplace_back(parse_item(sub_node));
					}

					result->get_array() = value;
					break;
				}
			}

			return result;
		};

		for (const auto &node : config_json) {
			parse_item(node);
		}
	}
	catch (const std::exception &ex) {
		console::log("error parsing the config file: %s", ex.what());
	}

	config_file.close();
}

void config::shutdown() {
	std::ofstream config_file("config.json");

	static std::function<void(item *, nlohmann::json &)> serialize_item = [&](item *config_item, nlohmann::json &node) {
		node["name"] = config_item->get_name();
		node["type"] = std::to_string((int32_t)config_item->get_type());

		switch (config_item->get_type()) {
		case type::int8:	node["value"] = config_item->get_int8(); break;
		case type::int16:	node["value"] = config_item->get_int16(); break;
		case type::int32:	node["value"] = config_item->get_int32(); break;
		case type::int64:	node["value"] = config_item->get_int64(); break;
		case type::float_:	node["value"] = config_item->get_float(); break;
		case type::string_:	node["value"] = config_item->get_string(); break;
		case type::colour:	node["value"]["r"] = config_item->get_colour().r;
							node["value"]["g"] = config_item->get_colour().g;
							node["value"]["b"] = config_item->get_colour().b;
							node["value"]["a"] = config_item->get_colour().a;
							break;
		case type::array_: {
				for (size_t i = 0; i < config_item->get_array().size(); i++) {
					serialize_item(config_item->get_array().at(i), node["value"][i]);
				}

				break;
			}
		}
	};

	for (item *config_item : config_items) {
		auto &node = config_json[config_item->get_name()];
		serialize_item(config_item, node);
	}

	config_file << config_json;
	config_file.close();
}

config::item *config::get(const std::string &name) {
	auto result = std::find_if(config_items.begin(), config_items.end(), [&](config::item *item) {
		return item->get_name() == name;
	});

	if (result == config_items.end())
		return nullptr;

	return *result;
}

config::item *config::create(const std::string &name, const config::type &item_type) {
	item *result = get(name);

	if (result != nullptr)
		return result;

	switch (item_type) {
		case type::int8:	result = new item(name, int8_t(0)); break;
		case type::int16:	result = new item(name, int16_t(0)); break;
		case type::int32:	result = new item(name, int32_t(0)); break;
		case type::int64:	result = new item(name, int64_t(0)); break;
		case type::float_:	result = new item(name, float(0)); break;
		case type::string_: result = new item(name, ""); break;
		case type::colour:	result = new item(name, sdk::colour::white); break;
		case type::array_:	result = new item(name, std::vector<item *> {}); break;
	}

	config_items.emplace_back(result);
	return result;
}