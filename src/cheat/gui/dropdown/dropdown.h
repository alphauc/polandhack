#pragma once

// note to reader:
// i stopped giving a fuck like halfway through this lol

static int animation = 5;
static int animation_c = 5;
namespace object {
	class c_dropdown : public c_object {
	protected:
		std::vector< std::string > m_options;
		int *m_value;

		bool m_opened = false;
		bool m_clicked = false;
		bool m_held = false;

	public:
		c_dropdown(const char *name, int *value, std::vector< std::string > options) {
			m_options.clear();
			m_options = options;
			m_options.shrink_to_fit();

			m_value = value;

			set_name(name);
			set_visible(true);

			m_size = { 154, 34 };

			set_identifier(g_fnv_str("DROPDOWN"));
		}

		void update() {
			if (!m_parent)
				set_visible(false);

			if (!is_visible())
				return;

			// Update the position.
			m_pos.x = m_parent->get_pos().x + 25;
			m_pos.y = m_parent->get_pos().y + 5;

			for (auto &it : m_parent->container()) {
				if (it == shared_from_this())
					break;

				if (it->is_visible())
					m_pos.y += it->get_size().y + object_spacing;
			}

			// Don't affect the object until it's unblocked.
			if (m_blocked)
				return;

			// Restore blocked state if the dropdown is closed.
			if (!m_opened) {
				set_blocked_r(false);
			}

			// See if the dropdown was clicked.
			if (m_held && !g_input.key_down(VK_LBUTTON)) {
				m_clicked = true;
			}
			else {
				m_clicked = false;
			}

			if (m_clicked && g_input.is_in_box({ m_pos.x, m_pos.y + 15 }, { m_size.x, 20 })) {
				m_opened = true;
			}

			if (m_opened) {
				set_blocked_r(true);

				for (size_t n{}; n < m_options.size(); n++) {
					long new_y = (long)m_pos.y + 40 + 20 * n;
					sdk::vec2 pos = { m_pos.x, new_y };

					if (m_clicked) {
						long thing = (long)m_size.y + 5 + (20 * m_options.size());
						if (g_input.is_in_box(pos, sdk::vec2{ m_size.x, 20 })) {
							*m_value = n;
							m_opened = false;
						}
						else if (!g_input.is_in_box(m_pos, sdk::vec2{ m_size.x, thing })) {
							m_opened = false;
							animation_c = 1;
						}
					}
				}
			}

			if (m_opened) {
				animation_c++;
			}
			if (animation_c > 20)
				animation_c = 20;

			std::clamp<int>(animation_c, 0, 20);

			m_held = g_input.key_down(VK_LBUTTON);
		}

		void draw() {
			if (!is_visible())
				return;

			// The dropdown name.
			render::text(render::fonts::menu, m_pos.x, m_pos.y - 2, MENU_COLOR(215, 215, 215), render::alignment::left, m_name);


			// The dropdown itself.
			render::gradient(m_pos.x, m_pos.y + 15, m_size.x, m_size.y - 15, MENU_COLOR(35, 35, 35), MENU_COLOR(35, 35, 35), true);
			render::bordered_box(m_pos.x, m_pos.y + 15, m_size.x, m_size.y - 15, MENU_COLOR(0, 0, 0));

			// The little triangle next to the dropdown.
			auto arrow = [](int x, int y, bool turn) {
				if (!turn) {
					for (auto i = 5; i >= 3; --i) {
						auto offset = 5 - i;

						render::line(x + offset, y + offset, x + offset + std::clamp(i - offset, 0, 5), y + offset, MENU_COLOR(152, 152, 152));
					}
				}
				else {
					render::line(x + 2, y, x + 3, y, MENU_COLOR(152, 152, 152));
					render::line(x + 1, y + 1, x + 4, y + 1, MENU_COLOR(152, 152, 152));
					render::line(x, y + 2, x + 5, y + 2, MENU_COLOR(152, 152, 152));
				}
			};

			arrow(m_pos.x + m_size.x - 10, (m_pos.y + m_size.y / 2) + 7, m_opened);

			if (m_options.size()) {
				auto *text = m_options.at(*m_value).c_str();
				render::text(render::fonts::menu, m_pos.x + 10, m_pos.y + 19, MENU_COLOR(153, 153, 153), render::alignment::left, text, false);
			}

			// Draw dropdown
			if (m_opened) {
				render::filled_box(m_pos.x, m_pos.y + 35, m_size.x, animation_c * m_options.size(), MENU_COLOR(40, 40, 40));
				render::bordered_box(m_pos.x, m_pos.y + 35, m_size.x, animation_c * m_options.size(), MENU_COLOR(0, 0, 0));

				for (size_t n{}; n < m_options.size(); n++) {
					auto *text = m_options.at(n).c_str();
					bool selected = n == (size_t)*m_value;

					render::text(render::fonts::menu, m_pos.x + 10,
						m_pos.y + 40 + 20 * n, selected ? g_config.menu.m_accent : MENU_COLOR(153, 153, 153), render::alignment::left, text);
				}
			}
		}
	};

	struct multi_item_t {
		std::string m_name;
		bool *m_value;
	};

	class c_multi_dropdown : public c_object {
	protected:
		std::vector< multi_item_t > m_options;

		bool m_opened = false;
		bool m_clicked = false;
		bool m_held = false;

	public:
		c_multi_dropdown(const char *name, std::vector< multi_item_t > options) {
			m_options.clear();
			m_options = options;
			m_options.shrink_to_fit();

			set_name(name);
			set_visible(true);

			m_size = { 154, 34 };

			set_identifier(g_fnv_str("MULTIDROPDOWN"));
		}

		void update() {
			if (!m_parent)
				set_visible(false);

			if (!is_visible())
				return;

			// Update the position.
			m_pos.x = m_parent->get_pos().x + 25;
			m_pos.y = m_parent->get_pos().y + 5;

			for (auto &it : m_parent->container()) {
				if (it == shared_from_this())
					break;

				if (it->is_visible())
					m_pos.y += it->get_size().y + object_spacing;
			}

			// Don't affect the object until it's unblocked.
			if (m_blocked)
				return;

			// Restore blocked state if the dropdown is closed.
			if (!m_opened) {
				set_blocked_r(false);
			}

			// See if the dropdown was clicked.
			if (m_held && !g_input.key_down(VK_LBUTTON)) {
				m_clicked = true;
			}
			else {
				m_clicked = false;
			}

			if (m_clicked && g_input.is_in_box({ m_pos.x, m_pos.y + 15 }, { m_size.x, 20 })) {
				m_opened = true;
			}

			if (m_opened) {
				set_blocked_r(true);

				for (size_t n{}; n < m_options.size(); n++) {
					long new_y = (long)m_pos.y + 35 + 20 * n;
					sdk::vec2 pos = { m_pos.x, new_y };

					if (m_clicked) {
						long thing = (long)m_size.y + 5 + (20 * m_options.size());
						if (g_input.is_in_box(pos, sdk::vec2{ m_size.x, 20 })) {
							*m_options.at(n).m_value ^= 1;
						}
						else if (!g_input.is_in_box(m_pos, sdk::vec2{ m_size.x, thing })) {
							m_opened = false;
							animation = 1;
						}
					}
				} 
			}

			if (m_opened) {
				animation ++;
			}
			if (animation > 20)
				animation = 20;

			std::clamp<int>(animation, 0, 20);

			m_held = g_input.key_down(VK_LBUTTON);
		}

		void draw() {
			if (!is_visible())
				return;

			// The dropdown name.
			render::text(render::fonts::menu, m_pos.x, m_pos.y - 2, MENU_COLOR(215, 215, 215), render::alignment::left, m_name);

			// The dropdown itself.
			render::gradient(m_pos.x, m_pos.y + 15, m_size.x, m_size.y - 15, MENU_COLOR(35, 35, 35), MENU_COLOR(35, 35, 35), true);
			render::bordered_box(m_pos.x, m_pos.y + 15, m_size.x, m_size.y - 15, MENU_COLOR(0, 0, 0));

			// The little triangle next to the dropdown.
			auto arrow = [](int x, int y, bool turn) {
				if (!turn) {
					for (auto i = 5; i >= 3; --i) {
						auto offset = 5 - i;

						render::line(x + offset, y + offset, x + offset + std::clamp(i - offset, 0, 5), y + offset, MENU_COLOR(152, 152, 152));
					}
				}
				else {
					render::line(x + 2, y, x + 3, y, MENU_COLOR(152, 152, 152));
					render::line(x + 1, y + 1, x + 4, y + 1, MENU_COLOR(152, 152, 152));
					render::line(x, y + 2, x + 5, y + 2, MENU_COLOR(152, 152, 152));
				}
			};

			arrow(m_pos.x + m_size.x - 10, (m_pos.y + m_size.y / 2) + 7, m_opened);

			if (m_options.size()) {
				std::string buffer;

				for (auto &it : m_options) {
					if (*it.m_value) {
						if (buffer.length() > 0)
							buffer += ", ";

						buffer += it.m_name;
					}
				}

				// aaaaAAAAAAAAAA BAD
				if (buffer.length() > 23) {
					buffer.resize(23);
					buffer += "...";
				}

				if (!buffer.length())
					buffer += "none";

				render::text(render::fonts::menu, m_pos.x + 10, m_pos.y + 19,
					m_opened ? g_config.menu.m_accent : MENU_COLOR(153, 153, 153), render::alignment::left, buffer.c_str(), false);
			}

			// Draw dropdown
			if (m_opened) {

				render::filled_box(m_pos.x, m_pos.y + 35, m_size.x, animation * m_options.size(), MENU_COLOR(40, 40, 40));
				render::bordered_box(m_pos.x, m_pos.y + 35, m_size.x, animation * m_options.size(), MENU_COLOR(0, 0, 0));

				for (size_t n{}; n < m_options.size(); n++) {
					auto text = m_options.at(n).m_name.c_str();
					bool selected = *m_options.at(n).m_value;

					render::text(render::fonts::menu, m_pos.x + 10,
						m_pos.y + 40 + 20 * n, selected ? g_config.menu.m_accent : MENU_COLOR(153, 153, 153), render::alignment::left, text);

				}
			}
		}
	};
}