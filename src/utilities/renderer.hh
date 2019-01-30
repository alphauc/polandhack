#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "../sdk/misc/colour.hh"
#include "../sdk/misc/definitions.hh"
#include "../sdk/misc/vector.hh"
#include "../sdk/misc/vgui.hh"

namespace render::fonts {

	extern sdk::d3d_font *menu;
	extern sdk::d3d_font *menu_large;
	extern sdk::d3d_font *menu_small;
	extern sdk::vgui::font visuals_general;
	extern sdk::vgui::font visuals_flags;
	extern sdk::vgui::font misc_specs;

}

namespace render {

	enum class alignment { left, center, right };

	extern sdk::vec2 screen_size;
	extern sdk::d3d_state_block *state_block;

	void initialize();
	void shutdown();

	void invalidate();
	void restore(sdk::d3d_device_9 *d3d_device);

	void begin();
	void end();

	sdk::vec2 get_text_size(const std::string &text, sdk::d3d_font *font, bool width);

	void line(const sdk::vec2 &start, const sdk::vec2 &end, const sdk::colour &colour);
	void line(float x, float y, float w, float h, const sdk::colour &colour);

	void filled_box(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour);
	void filled_box_outlined(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour, const sdk::colour &outline_colour);

	void filled_box(float x, float y, float w, float h, const sdk::colour &colour);
	void filled_box_outlined(float x, float y, float w, float h, const sdk::colour &colour, const sdk::colour &outline_colour);

	void bordered_box(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour);
	void bordered_box_outlined(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour, const sdk::colour &outline_colour);

	void bordered_box(float x, float y, float w, float h, const sdk::colour &colour);
	void bordered_box_outlined(float x, float y, float w, float h, const sdk::colour &colour, const sdk::colour &outline_colour);

	void gradient(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour1, const sdk::colour &colour2, bool vertical = false);
	void gradient_outlined(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour1, const sdk::colour &colour2, const sdk::colour &outline_colour, bool vertical = false);

	void gradient(float x, float y, float w, float h, const sdk::colour &colour1, const sdk::colour &colour2, bool vertical = false);
	void gradient_outlined(float x, float y, float w, float h, const sdk::colour &colour1, const sdk::colour &colour2, const sdk::colour &outline_colour, bool vertical = false);

	void text(const sdk::vec2 &pos, const std::string &text, sdk::d3d_font *font, const sdk::colour &colour = sdk::colour::white, const alignment &align = alignment::left);
	void text_outlined(const sdk::vec2 &pos, const std::string &text, sdk::d3d_font *font, const sdk::colour &colour = sdk::colour::white, const sdk::colour &outline_colour = sdk::colour::black, const alignment &align = alignment::left);

	void text(sdk::d3d_font *font, float x, float y, const sdk::colour &colour, const alignment &align, const std::string &text, bool shadow = true);
	void text_outlined(sdk::d3d_font *font, float x, float y, const sdk::colour &colour, const alignment &align, const std::string &text);

}
