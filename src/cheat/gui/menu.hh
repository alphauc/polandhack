#pragma once

// Make sure that the input helper is accessible to all controls first.
#include "../../utilities/input.hh"

#include "../../utilities/renderer.hh"
#include "../../sdk/misc/colour.hh"

#include "../cfg.hh"
#include "../../security/fnv.hh"

// Small helpers for drawing.
#define MENU_COLOR( r, g, b ) sdk::colour( r, g, b, g_config.menu.m_alpha )
#define MENU_COLOUR( x ) sdk::colour( x.r, x.g, x.b, g_config.menu.m_alpha )
#define homo( x, x2 ) sdk::colour( x.r, x.g, x.b, g_config.menu.m_alpha - x2 )

// All objects should be included before the menu.
#include "object/object.h"

#include "form/form.h"

#include "tab-control/tab_control.h"
#include "tab-container/tab_container.h"
#include "tab/tab.h"

#include "checkbox/checkbox.h"
#include "dropdown/dropdown.h"
#include "button/button.hh"
#include "slider/slider.h"
#include "colour-picker/colour-picker.h"

namespace menu {
	class c_ui {
	protected:
		object::c_obj_vector m_container;
		bool m_initialized;
		void update_ui();

	public:
		void initialize();
		void on_frame();
	}; extern c_ui ui;
}