#pragma once

namespace object {
	class c_checkbox : public c_object {
	protected:
		bool  m_clicked = false;
		bool *m_value;

	public:
		c_checkbox(const char *name, bool *value = nullptr) :
			m_value(value) {
			set_name(name);
			set_visible(true);

			m_size = { 7, 7 };

			set_identifier(g_fnv_str("CHECKBOX"));
		}

		void update() {
			if (!m_parent)
				set_visible(false);

			if (!is_visible())
				return;

			// Update the position.
			m_pos.x = m_parent->get_pos().x + 5;
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

			// See if the checkbox was ticked.
			if (g_input.key_down(VK_LBUTTON)) {
				if (g_input.is_in_box(m_pos, { 100, m_size.y }) && !m_clicked) {
					m_clicked = true;
					*m_value ^= 1;
				}
			}
			else {
				m_clicked = false;
			}


		}

		void draw() {
			if (!is_visible())
				return;

			if (*m_value) {
				render::gradient(m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOUR(g_config.menu.m_accent),
					MENU_COLOR(g_config.menu.m_accent.r * 0.40f, g_config.menu.m_accent.g * 0.40f, g_config.menu.m_accent.b * 0.40f), true);
			}
			else {
				render::gradient(m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR(72, 72, 72), MENU_COLOR(45, 45, 45), true);
			}
			
			render::bordered_box(m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR(0, 0, 0));

			render::text(render::fonts::menu, m_pos.x + 20, m_pos.y - 3, MENU_COLOR(215, 215, 215), render::alignment::left, m_name);
		}
	};
}