#include "interface_manager.hh"
#include "console.hh"
#include "renderer.hh"

sdk::d3d_font *create_font(const std::string &font_name, size_t size, size_t weight, bool antialiased = false) {
	sdk::d3d_font *font_ptr;

	D3DXCreateFontA(
		interfaces::d3d_device, size, 0, weight, 0, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		antialiased ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
		DEFAULT_PITCH, font_name.c_str(), &font_ptr
	);

	return font_ptr;
}

sdk::d3d_font *render::fonts::menu = nullptr;
sdk::d3d_font *render::fonts::menu_large = nullptr;
sdk::d3d_font *render::fonts::menu_small = nullptr;
sdk::vgui::font render::fonts::visuals_general;
sdk::vgui::font render::fonts::visuals_flags;
sdk::vgui::font render::fonts::misc_specs;

sdk::vec2 render::screen_size = sdk::vec2(1280, 720);
sdk::d3d_state_block *render::state_block = nullptr;

void render::initialize() {
	fonts::menu = create_font("Tahoma", 12, 400);
	fonts::menu_large = create_font("Tahoma", 12, 700);
	fonts::menu_small = create_font("Small Fonts", 12, 0);
}

void render::shutdown() {
	fonts::menu->Release();
	fonts::menu_large->Release();
	fonts::menu_small->Release();
}

void render::invalidate() {
	shutdown();
	interfaces::d3d_device = nullptr;
}

void render::restore(sdk::d3d_device_9 *d3d_device) {
	D3DVIEWPORT9 d3d_viewport;

	interfaces::d3d_device = d3d_device;
	interfaces::d3d_device->GetViewport(&d3d_viewport);

	initialize();
}


void render::begin() {
	D3DVIEWPORT9 d3d_viewport;
	interfaces::d3d_device->GetViewport(&d3d_viewport);

	screen_size = sdk::vec2(
		static_cast<float>(d3d_viewport.Width),
		static_cast<float>(d3d_viewport.Height)
	);

	interfaces::d3d_device->CreateStateBlock(D3DSBT_ALL, &state_block);
	state_block->Capture();

	interfaces::d3d_device->SetVertexShader(nullptr);
	interfaces::d3d_device->SetPixelShader(nullptr);
	interfaces::d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	interfaces::d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	interfaces::d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	interfaces::d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	interfaces::d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	interfaces::d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	interfaces::d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	interfaces::d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	interfaces::d3d_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	interfaces::d3d_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	interfaces::d3d_device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	interfaces::d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	interfaces::d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	interfaces::d3d_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	interfaces::d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	interfaces::d3d_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	interfaces::d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	interfaces::d3d_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	interfaces::d3d_device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	interfaces::d3d_device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}

void render::end() {
	state_block->Apply();
	state_block->Release();
}

sdk::vec2 render::get_text_size(const std::string &text, sdk::d3d_font *font, bool width) {
	RECT rect = { 0, 0, 0, 0 };

	if (!font)
		return 0;

	font->DrawTextA(nullptr, text.c_str(), text.length(), &rect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));

	return sdk::vec2(rect.right - rect.left, rect.bottom - rect.top);
}

void render::line(const sdk::vec2 &start, const sdk::vec2 &end, const sdk::colour &colour) {
	sdk::vertices vertices({
		sdk::vertex({ (float)start.x, (float)start.y, 0.0f, 1.0f, D3DCOLOR_ARGB(colour.a, colour.r, colour.g, colour.b) }),
		sdk::vertex({ (float)end.x, (float)end.y, 0.0f, 1.0f, D3DCOLOR_ARGB(colour.a, colour.r, colour.g, colour.b) })
		});

	interfaces::d3d_device->SetTexture(0, nullptr);
	interfaces::d3d_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertices.data(), sizeof(sdk::vertex));
}

void render::line(float x, float y, float w, float h, const sdk::colour &colour) {
	line(sdk::vec2(x, y), sdk::vec2(w, h), colour);
}

void render::filled_box(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour) {
	sdk::vertices vertices({
		sdk::vertex({ (float)pos.x, (float)pos.y, 0.0f, 1.0f, D3DCOLOR_ARGB(colour.a, colour.r, colour.g, colour.b) }),
		sdk::vertex({ (float)pos.x + size.x, (float)pos.y, 0.0f, 1.0f, D3DCOLOR_ARGB(colour.a, colour.r, colour.g, colour.b) }),
		sdk::vertex({ (float)pos.x, (float)pos.y + size.y, 0.0f, 1.0f, D3DCOLOR_ARGB(colour.a, colour.r, colour.g, colour.b) }),
		sdk::vertex({ (float)pos.x + size.x, (float)pos.y + size.y, 0.0f, 1.0f, D3DCOLOR_ARGB(colour.a, colour.r, colour.g, colour.b) })
		});

	interfaces::d3d_device->SetTexture(0, nullptr);
	interfaces::d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices.data(), sizeof(sdk::vertex));
}

void render::filled_box(float x, float y, float w, float h, const sdk::colour &colour) {
	filled_box(sdk::vec2(x, y), sdk::vec2(w, h), colour);
}

void render::filled_box_outlined(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour, const sdk::colour &outline_colour) {
	bordered_box(pos - sdk::vec2(1.0f, 1.0f), size + sdk::vec2(2.0f, 2.0f), outline_colour);
	filled_box(pos, size, colour);
}

void render::filled_box_outlined(float x, float y, float w, float h, const sdk::colour &colour, const sdk::colour &outline_colour) {
	filled_box_outlined(sdk::vec2(x, y), sdk::vec2(w, h), colour, outline_colour);
}

void render::bordered_box(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour) {
	line(pos, pos + sdk::vec2(size.x, 0.0f), colour);
	line(pos + sdk::vec2(size.x, 0.0f), pos + sdk::vec2(size.x, size.y), colour);
	line(pos + sdk::vec2(size.x, size.y), pos + sdk::vec2(0.0f, size.y), colour);
	line(pos + sdk::vec2(0.0f, size.y), pos, colour);
}

void render::bordered_box(float x, float y, float w, float h, const sdk::colour &colour) {
	bordered_box(sdk::vec2(x, y), sdk::vec2(w, h), colour);
}

void render::bordered_box_outlined(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour, const sdk::colour &outline_colour) {
	bordered_box(pos - sdk::vec2(1.0f, 1.0f), size + sdk::vec2(2.0f, 2.0f), outline_colour);
	bordered_box(pos + sdk::vec2(1.0f, 1.0f), size - sdk::vec2(2.0f, 2.0f), outline_colour);
	bordered_box(pos, size, colour);
}

void render::bordered_box_outlined(float x, float y, float w, float h, const sdk::colour &colour, const sdk::colour &outline_colour) {
	bordered_box_outlined(sdk::vec2(x, y), sdk::vec2(w, h), colour, outline_colour);
}

void render::gradient(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour1, const sdk::colour &colour2, bool vertical) {
	const auto clr = D3DCOLOR_ARGB(colour1.a, colour1.r, colour1.g, colour1.b);
	const auto clr2 = D3DCOLOR_ARGB(colour2.a, colour2.r, colour2.g, colour2.b);
	D3DCOLOR dwcol1, dwcol2, dwcol3, dwcol4;
	int posx1 = pos.x, posy1 = pos.y;	

	int posx2 = pos.x + size.x, posy2 = pos.y + size.y;
	if (vertical) {
		dwcol1 = clr;
		dwcol2 = clr;
		dwcol3 = clr2;
		dwcol4 = clr2;
	}
	else {
		dwcol1 = clr;
		dwcol2 = clr2;
		dwcol3 = clr;
		dwcol4 = clr2;
	}

	sdk::vertex vert[4] =
	{
		{ float(posx1), float(posy1), 0.0f, 1.0f, dwcol1 },
		{ float(posx2), float(posy1), 0.0f, 1.0f, dwcol2 },
		{ float(posx1), float(posy2), 0.0f, 1.0f, dwcol3 },
		{ float(posx2), float(posy2), 0.0f, 1.0f, dwcol4 }
	};

	interfaces::d3d_device->SetTexture(0, nullptr);
	interfaces::d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	interfaces::d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(sdk::vertex));
}

void render::gradient(float x, float y, float w, float h, const sdk::colour &colour1, const sdk::colour &colour2, bool vertical) {
	gradient(sdk::vec2(x, y), sdk::vec2(w, h), colour1, colour2, vertical);
}

void render::gradient_outlined(const sdk::vec2 &pos, const sdk::vec2 &size, const sdk::colour &colour1, const sdk::colour &colour2, const sdk::colour &outline_colour, bool vertical) {
	bordered_box(pos - sdk::vec2(1.0f, 1.0f), size + sdk::vec2(2.0f, 2.0f), outline_colour);
	gradient(pos, size, colour1, colour2, vertical);
}

void render::gradient_outlined(float x, float y, float w, float h, const sdk::colour &colour1, const sdk::colour &colour2, const sdk::colour &outline_colour, bool vertical) {
	gradient_outlined(sdk::vec2(x, y), sdk::vec2(w, h), colour1, colour2, outline_colour, vertical);
}

void render::text(const sdk::vec2 &pos, const std::string &text, sdk::d3d_font *font, const sdk::colour &colour, const render::alignment &align) {
	if (!font)
		return;

	RECT rect;
	SetRect(&rect, (float)pos.x, (float)pos.y, (float)pos.x, (float)pos.y);

	font->DrawTextA(nullptr, text.c_str(), -1, &rect, (int)align | DT_NOCLIP, D3DCOLOR_RGBA(colour.r, colour.g, colour.b, colour.a));
}

void render::text(sdk::d3d_font *font, float x, float y, const sdk::colour &colour, const alignment &align, const std::string &text, bool shadow) {
	if (shadow) {
		render::text(sdk::vec2(x + 1, y + 1), text, font, sdk::colour(0, 0, 0, colour.a - 10), align);
		render::text(sdk::vec2(x, y), text, font, colour, align);
	}
	else {
		render::text(sdk::vec2(x, y), text, font, colour, align);
	}
}

void render::text_outlined(const sdk::vec2 &pos, const std::string &text, sdk::d3d_font *font, const sdk::colour &colour, const sdk::colour &outline_colour, const render::alignment &align) {
	if (!font)
		return;

	render::text(pos - sdk::vec2(1.0f, 0.0f), text, font, outline_colour, align);
	render::text(pos + sdk::vec2(1.0f, 0.0f), text, font, outline_colour, align);
	render::text(pos - sdk::vec2(0.0f, 1.0f), text, font, outline_colour, align);
	render::text(pos + sdk::vec2(0.0f, 1.0f), text, font, outline_colour, align);

	render::text(pos, text, font, colour, align);
}

void render::text_outlined(sdk::d3d_font *font, float x, float y, const sdk::colour &colour, const alignment &align, const std::string &text) {
	text_outlined(sdk::vec2(x, y), text, font, colour, sdk::colour::black, align);
}
