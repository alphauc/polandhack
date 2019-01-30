#pragma once

namespace object {
	// The container for the tab.
	class c_tab_container : public c_container {
	protected:
		size_t m_split;

	public:
		c_tab_container() {
			m_split = 0;

			set_visible(true);
			set_identifier(g_fnv_str("TAB_CONTAINER"));
		}

		void update() {
			if (!m_parent)
				set_visible(false);

			if (!is_visible())
				return;

			// Not initialised as c_form -> c_tab_control -> c_tab -> c_tab_container
			auto parent = m_parent->get_parent()->get_parent();

			if (!parent)
				return;

			m_pos.x = m_parent->get_parent()->get_pos().x + m_parent->get_parent()->get_size().x + 21;
			m_pos.y = m_parent->get_parent()->get_pos().y;

			m_size.x = parent->get_size().x - m_parent->get_parent()->get_size().x - 50 - 1;
			m_size.y = m_parent->get_parent()->get_size().y;
		}

		void draw() {
			if (!is_visible())
				return;

			// Draw background.
			render::filled_box(m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR(9, 9, 9));

			// Get the user information.
			char username[256 + 1];
			DWORD username_len = 256 + 1;
			GetUserName(username, &username_len);

			std::string str(__DATE__);

			sdk::vec2 name = render::get_text_size(str.append(" | ").append(username), render::fonts::menu, 0);

			// Draw the username and date.
			render::text(render::fonts::menu, m_pos.x + m_size.x - name.x - 2, m_pos.y + m_size.y - name.y - 2, MENU_COLOR(215, 215, 215), render::alignment::left, str);

			render::bordered_box(m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR(0, 0, 0));
			render::bordered_box(m_pos.x + 1, m_pos.y + 1, m_size.x - 2, m_size.y - 2, MENU_COLOR(45, 45, 45));

			// Draw any children that the tab container has.
			if (m_split > 0) {
				size_t split{};

				// NOTE: This is a ghetto solution, at best.
				// Fixing this properly would require the entire menu to be refactored.
				// ATM, I am happy with this code.
				auto container_one = std::make_shared< c_container >();
				auto container_two = std::make_shared< c_container >();

				container_one->m_pos = { m_pos.x + 15, m_pos.y + 15 };
				container_two->m_pos = { m_pos.x + m_size.x / 2 + 15, m_pos.y + 15 };

				for (auto &it : m_container) {
					if (m_split <= split++) {
						container_two->push(it);
					}
					else {
						container_one->push(it);
					}
				}

				// Draw the containers
				container_two->draw(); // draw the 2nd one first incase we have a colour picker open in the first one
				container_one->draw();
			}
			else {
				auto container_one = std::make_shared< c_container >();
				container_one->m_pos = { m_pos.x + 15, m_pos.y + 15 };

				for (auto &it : m_container) {
					container_one->push(it);
				}

				container_one->draw();
			}
		}

		void split() {
			m_split = m_container.size();
		}
	};
}