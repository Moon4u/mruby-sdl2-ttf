#include "sdl2-ttf.h"
#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mruby/hash.h"
#include "mruby/string.h"
#include "sdl2_rwops.h"
#include "sdl2_surface.h"
#include "sdl2_pixels.h"



static struct RClass *class_Font    = NULL;


typedef struct mrb_sdl2_ttf_font_data_t {
  TTF_Font * font;
} mrb_sdl2_ttf_font_data_t;

static void
mrb_sdl2_ttf_font_data_free(mrb_state *mrb, void *p)
{
  mrb_sdl2_ttf_font_data_t *data =
    (mrb_sdl2_ttf_font_data_t*)p;
  if (NULL != data) {
    if (NULL != data->font) {
      mrb_free(mrb, data->font);
    }
    mrb_free(mrb, p);
  }
}


struct mrb_data_type const mrb_sdl2_ttf_font_data_type = {
  "Font", mrb_sdl2_ttf_font_data_free
};

TTF_Font *
mrb_sdl2_font_get_ptr(mrb_state *mrb, mrb_value value)
{
  mrb_sdl2_ttf_font_data_t *data;
  if (mrb_nil_p(value)) {
    return NULL;
  }

  data =
    (mrb_sdl2_ttf_font_data_t*)mrb_data_get_ptr(mrb, value, &mrb_sdl2_ttf_font_data_type);
  return data->font;
}

mrb_value
mrb_sdl2_font(mrb_state *mrb, TTF_Font *value)
{
  mrb_sdl2_ttf_font_data_t *data =
    (mrb_sdl2_ttf_font_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_ttf_font_data_t));
  if (NULL == data) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
  }
  data->font = value;
  return mrb_obj_value(Data_Wrap_Struct(mrb, class_Font, &mrb_sdl2_ttf_font_data_type, data));
}


static mrb_value
mrb_sdl2_ttf_font_initialize(mrb_state *mrb, mrb_value self)
{
  TTF_Font *font = NULL;
  mrb_sdl2_ttf_font_data_t *data =
    (mrb_sdl2_ttf_font_data_t*)DATA_PTR(self);

  if (NULL == data) {
    data = (mrb_sdl2_ttf_font_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_ttf_font_data_t));
    if (NULL == data) {
      mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
    }
    data->font = NULL;
  }

  if (2 == mrb->c->ci->argc) {
  	SDL_RWops *rwops;
  	mrb_value object;
  	mrb_int size;
  	mrb_get_args(mrb, "oi", &object, &size);

    if (mrb_obj_is_kind_of(mrb, object, mrb_class_get_under(mrb, mrb->object_class, "String"))) {
			font = TTF_OpenFont(RSTRING_PTR(object), size);
    } else if (mrb_obj_is_kind_of(mrb, object, mrb_class_get_under(mrb, mod_SDL2, "RWops"))) {
			rwops = mrb_sdl2_rwops_get_ptr(mrb, object);
			font = TTF_OpenFontRW(rwops, 0, size);
		}
  } else if (3 == mrb->c->ci->argc) {
  	SDL_RWops *rwops;
  	mrb_value object;
  	mrb_int size, index;
  	mrb_get_args(mrb, "oii", &object, &size, &index);
    if (mrb_obj_is_kind_of(mrb, object, mrb_class_get_under(mrb, mrb->object_class, "String"))) {
			font = TTF_OpenFontIndex(RSTRING_PTR(object), size, index);
    } else if (mrb_obj_is_kind_of(mrb, object, mrb_class_get_under(mrb, mod_SDL2, "RWops"))) {
			rwops = mrb_sdl2_rwops_get_ptr(mrb, object);
			font = TTF_OpenFontIndexRW(rwops, 0, size, index);
		}
  } else {
    mrb_free(mrb, data);
    mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments.");
  }
  if (NULL == font) {
    mrb_free(mrb, data);
    mruby_sdl2_raise_error(mrb);
  }

  data->font = font;

  DATA_PTR(self) = data;
  DATA_TYPE(self) = &mrb_sdl2_ttf_font_data_type;
  return self;
}

static mrb_value
mrb_sdl2_ttf_font_free(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_ttf_font_data_t *data =
    (mrb_sdl2_ttf_font_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_ttf_font_data_type);
  if (NULL != data->font) {
    TTF_CloseFont(data->font);
    data->font = NULL;
  }
  return self;
}



static mrb_value
mrb_sdl2_ttf_init(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_Init());
}

static mrb_value
mrb_sdl2_ttf_font_get_style(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_GetFontStyle(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_set_style(mrb_state *mrb, mrb_value self)
{
	mrb_int style;
	mrb_get_args(mrb, "i", &style);
	TTF_SetFontStyle(mrb_sdl2_font_get_ptr(mrb, self), style);
	return mrb_nil_value();
}

static mrb_value
mrb_sdl2_ttf_font_get_outline(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_GetFontOutline(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_set_outline(mrb_state *mrb, mrb_value self)
{
	mrb_int outline;
	mrb_get_args(mrb, "i", &outline);
	TTF_SetFontOutline(mrb_sdl2_font_get_ptr(mrb, self), outline);
	return mrb_nil_value();
}

static mrb_value
mrb_sdl2_ttf_font_get_hinting(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_GetFontHinting(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_set_hinting(mrb_state *mrb, mrb_value self)
{
	mrb_int hinting;
	mrb_get_args(mrb, "i", &hinting);
	TTF_SetFontHinting(mrb_sdl2_font_get_ptr(mrb, self), hinting);
	return mrb_nil_value();
}

static mrb_value
mrb_sdl2_ttf_font_get_height(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_FontHeight(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_get_ascent(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_FontAscent(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_get_descent(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_FontDescent(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_get_lineskip(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_FontLineSkip(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_get_kerning(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_GetFontKerning(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_set_kerning(mrb_state *mrb, mrb_value self)
{
	mrb_bool allowed;
	mrb_get_args(mrb, "b", &allowed);
	TTF_SetFontKerning(mrb_sdl2_font_get_ptr(mrb, self), allowed ? 1 : 0);
	return mrb_nil_value();
}

static mrb_value
mrb_sdl2_ttf_font_get_faces(mrb_state *mrb, mrb_value self)
{
	return mrb_fixnum_value(TTF_FontFaces(mrb_sdl2_font_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_ttf_font_face_fixed_width(mrb_state *mrb, mrb_value self)
{
	return (TTF_FontFaceIsFixedWidth(mrb_sdl2_font_get_ptr(mrb, self)) == 0) ? mrb_false_value() : mrb_true_value();
}

static mrb_value
mrb_sdl2_ttf_font_get_face_family_name(mrb_state *mrb, mrb_value self)
{
	char *r = TTF_FontFaceFamilyName(mrb_sdl2_font_get_ptr(mrb, self));
	if (r)
		return mrb_str_new_cstr(mrb, r);
	return mrb_nil_value();
}

static mrb_value
mrb_sdl2_ttf_font_get_face_style_name(mrb_state *mrb, mrb_value self)
{
	char *r = TTF_FontFaceStyleName(mrb_sdl2_font_get_ptr(mrb, self));
	if (r)
		return mrb_str_new_cstr(mrb, r);
	return mrb_nil_value();
}

static mrb_value
mrb_sdl2_ttf_font_get_glyph_index(mrb_state *mrb, mrb_value self)
{
	mrb_value character;
	Uint16 ch;
	mrb_get_args(mrb, "b", &character);
  if (mrb_obj_is_kind_of(mrb, character, mrb_class_get_under(mrb, mrb->object_class, "String"))) {
		ch = *RSTRING_PTR(character);
  } else if (mrb_obj_is_kind_of(mrb, character, mrb_class_get_under(mrb, mrb->object_class, "Integer"))) {
  	ch = (Uint16) character.value.i;
  }
	return mrb_fixnum_value(TTF_GlyphIsProvided(mrb_sdl2_font_get_ptr(mrb, self), ch));
}

static mrb_value
mrb_sdl2_ttf_font_get_glyph_metric(mrb_state *mrb, mrb_value self)
{
	int minx, maxx, miny, maxy, advance;
	mrb_value character;
	Uint16 ch;
	mrb_value result;
	mrb_get_args(mrb, "b", &character);
  if (mrb_obj_is_kind_of(mrb, character, mrb_class_get_under(mrb, mrb->object_class, "String"))) {
		ch = *RSTRING_PTR(character);
  } else if (mrb_obj_is_kind_of(mrb, character, mrb_class_get_under(mrb, mrb->object_class, "Integer"))) {
  	ch = (Uint16) character.value.i;
  }
  result = mrb_hash_new_capa(mrb, 5);
  if (TTF_GlyphMetrics(mrb_sdl2_font_get_ptr(mrb, self), ch, &minx, &maxx, &miny, &maxy, &advance) == -1) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	mrb_hash_set(mrb, result, mrb_str_new_cstr(mrb, "minx"), mrb_fixnum_value(minx));
	mrb_hash_set(mrb, result, mrb_str_new_cstr(mrb, "maxx"), mrb_fixnum_value(maxx));
	mrb_hash_set(mrb, result, mrb_str_new_cstr(mrb, "miny"), mrb_fixnum_value(miny));
	mrb_hash_set(mrb, result, mrb_str_new_cstr(mrb, "maxy"), mrb_fixnum_value(maxy));
	mrb_hash_set(mrb, result, mrb_str_new_cstr(mrb, "advance"), mrb_fixnum_value(advance));

	return result;
}

static mrb_value
mrb_sdl2_ttf_font_get_size_text(mrb_state *mrb, mrb_value self)
{
	mrb_value text;
	int w, h;
	mrb_value result;
	mrb_get_args(mrb, "S", &text);

  if (TTF_SizeText(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), &w, &h) == -1) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
  result = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, result, mrb_fixnum_value(w));
  mrb_ary_push(mrb, result, mrb_fixnum_value(h));
  return result;
}

static mrb_value
mrb_sdl2_ttf_font_get_size_UTF8(mrb_state *mrb, mrb_value self)
{
	mrb_value text;
	int w, h;
	mrb_value result;
	mrb_get_args(mrb, "S", &text);

  if (TTF_SizeUTF8(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), &w, &h) == -1) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
  result = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, result, mrb_fixnum_value(w));
  mrb_ary_push(mrb, result, mrb_fixnum_value(h));
  return result;
}

static mrb_value
mrb_sdl2_ttf_font_get_size_UNICODE(mrb_state *mrb, mrb_value self)
{
	mrb_value text;
	int w, h;
	mrb_value result;
	mrb_get_args(mrb, "S", &text);

  if (TTF_SizeUNICODE(mrb_sdl2_font_get_ptr(mrb, self), (Uint16 *) RSTRING_PTR(text), &w, &h) == -1) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
  result = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, result, mrb_fixnum_value(w));
  mrb_ary_push(mrb, result, mrb_fixnum_value(h));
  return result;
}

static mrb_value
mrb_sdl2_ttf_font_render_text(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiii", &text, &r, &g, &b, &a);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderText_Solid(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), color);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_UTF8(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiii", &text, &r, &g, &b, &a);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderUTF8_Solid(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), color);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_UNICODE(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiii", &text, &r, &g, &b, &a);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderUNICODE_Solid(mrb_sdl2_font_get_ptr(mrb, self), (Uint16 *)RSTRING_PTR(text), color);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_glyph(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiii", &text, &r, &g, &b, &a);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderGlyph_Solid(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text)[0], color);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_text_shaded(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int fgr, fgg, fgb, fga, bgr, bgg, bgb, bga;
	SDL_Surface * c;
	SDL_Color fgcolor;
	SDL_Color bgcolor;
	mrb_get_args(mrb, "Siiiiiiii", &text, &fgr, &fgg, &fgb, &fga, &bgr, &bgg, &bgb, &bga);
	fgcolor.r = fgr;
	fgcolor.g = fgg;
	fgcolor.b = fgb;
	fgcolor.a = fga;
	bgcolor.r = bgr;
	bgcolor.g = bgg;
	bgcolor.b = bgb;
	bgcolor.a = bga;
	c = TTF_RenderText_Shaded(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), fgcolor, bgcolor);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_UTF8_shaded(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int fgr, fgg, fgb, fga, bgr, bgg, bgb, bga;
	SDL_Surface * c;
	SDL_Color fgcolor;
	SDL_Color bgcolor;
	mrb_get_args(mrb, "Siiiiiiii", &text, &fgr, &fgg, &fgb, &fga, &bgr, &bgg, &bgb, &bga);
	fgcolor.r = fgr;
	fgcolor.g = fgg;
	fgcolor.b = fgb;
	fgcolor.a = fga;
	bgcolor.r = bgr;
	bgcolor.g = bgg;
	bgcolor.b = bgb;
	bgcolor.a = bga;
	c = TTF_RenderUTF8_Shaded(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), fgcolor, bgcolor);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_UNICODE_shaded(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int fgr, fgg, fgb, fga, bgr, bgg, bgb, bga;
	SDL_Surface * c;
	SDL_Color fgcolor;
	SDL_Color bgcolor;
	mrb_get_args(mrb, "Siiiiiiii", &text, &fgr, &fgg, &fgb, &fga, &bgr, &bgg, &bgb, &bga);
	fgcolor.r = fgr;
	fgcolor.g = fgg;
	fgcolor.b = fgb;
	fgcolor.a = fga;
	bgcolor.r = bgr;
	bgcolor.g = bgg;
	bgcolor.b = bgb;
	bgcolor.a = bga;
	c = TTF_RenderUNICODE_Shaded(mrb_sdl2_font_get_ptr(mrb, self), (Uint16 *)RSTRING_PTR(text), fgcolor, bgcolor);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_glyph_shaded(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int fgr, fgg, fgb, fga, bgr, bgg, bgb, bga;
	SDL_Surface * c;
	SDL_Color fgcolor;
	SDL_Color bgcolor;
	mrb_get_args(mrb, "Siiiiiiii", &text, &fgr, &fgg, &fgb, &fga, &bgr, &bgg, &bgb, &bga);
	fgcolor.r = fgr;
	fgcolor.g = fgg;
	fgcolor.b = fgb;
	fgcolor.a = fga;
	bgcolor.r = bgr;
	bgcolor.g = bgg;
	bgcolor.b = bgb;
	bgcolor.a = bga;
	c = TTF_RenderGlyph_Shaded(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text)[0], fgcolor, bgcolor);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_text_blended(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiii", &text, &r, &g, &b, &a);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderText_Blended(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), color);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_UTF8_blended(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiii", &text, &r, &g, &b, &a);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderUTF8_Blended(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), color);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_UNICODE_blended(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiii", &text, &r, &g, &b, &a);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderUNICODE_Blended(mrb_sdl2_font_get_ptr(mrb, self), (Uint16 *)RSTRING_PTR(text), color);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_glyph_blended(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiii", &text, &r, &g, &b, &a);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderGlyph_Blended(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text)[0], color);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_text_blended_wrapped(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a, wrapLength;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiiii", &text, &r, &g, &b, &a, &wrapLength);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderText_Blended_Wrapped(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), color, wrapLength);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_UTF8_blended_wrapped(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a, wrapLength;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiiii", &text, &r, &g, &b, &a, &wrapLength);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderUTF8_Blended_Wrapped(mrb_sdl2_font_get_ptr(mrb, self), RSTRING_PTR(text), color, wrapLength);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_font_render_UNICODE_blended_wrapped(mrb_state *mrb, mrb_value self)
{
	mrb_value text; 
	mrb_int r, g, b, a, wrapLength;
	SDL_Surface * c;
	SDL_Color color;
	mrb_get_args(mrb, "Siiiii", &text, &r, &g, &b, &a, &wrapLength);
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = TTF_RenderUNICODE_Blended_Wrapped(mrb_sdl2_font_get_ptr(mrb, self), (Uint16 *)RSTRING_PTR(text), color, wrapLength);
	if (c == NULL) {
    mruby_sdl2_raise_error(mrb);
  	return mrb_false_value();
  }
	return mrb_sdl2_video_surface(mrb, c, 0);
}

static mrb_value
mrb_sdl2_ttf_quit(mrb_state *mrb, mrb_value self)
{
	TTF_Quit();
	return mrb_nil_value();
}

static mrb_value
mrb_sdl2_ttf_was_init(mrb_state *mrb, mrb_value self)
{
	return (TTF_WasInit() == 0) ? mrb_false_value() : mrb_true_value();
}

static mrb_value
mrb_sdl2_ttf_font_get_kerning_size(mrb_state *mrb, mrb_value self)
{
	mrb_int prev_index, index;
	mrb_get_args(mrb, "ii", &prev_index, &index);
	return mrb_fixnum_value(TTF_GetFontKerningSize(mrb_sdl2_font_get_ptr(mrb, self), prev_index, index));	
}
void
mrb_mruby_sdl2_ttf_gem_init(mrb_state *mrb)
{
  int arena_size;
	struct RClass *mod_Ttf = mrb_define_module_under(mrb, mod_SDL2, "TTF");
  class_Font = mrb_define_class_under(mrb, mod_Ttf, "Font", mrb->object_class);

  MRB_SET_INSTANCE_TT(class_Font, MRB_TT_DATA);

  mrb_define_module_function(mrb, mod_Ttf, "init",                  mrb_sdl2_ttf_init,                MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Ttf, "quit",                  mrb_sdl2_ttf_quit,                MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Ttf, "is_init?",              mrb_sdl2_ttf_was_init,                MRB_ARGS_NONE());

  mrb_define_method(mrb, class_Font, "initialize",                     mrb_sdl2_ttf_font_initialize,   MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, class_Font, "free",                           mrb_sdl2_ttf_font_free,                           MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "destroy",                        mrb_sdl2_ttf_font_free,                           MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "get_style",                      mrb_sdl2_ttf_font_get_style,                      MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "set_style",                      mrb_sdl2_ttf_font_set_style,                      MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "get_outline",                    mrb_sdl2_ttf_font_get_outline,                    MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "set_outline",                    mrb_sdl2_ttf_font_set_outline,                    MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "get_hinting",                    mrb_sdl2_ttf_font_get_hinting,                    MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "set_hinting",                    mrb_sdl2_ttf_font_set_hinting,                    MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "get_height",                     mrb_sdl2_ttf_font_get_height,                     MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "get_ascent",                     mrb_sdl2_ttf_font_get_ascent,                     MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "get_descent",                    mrb_sdl2_ttf_font_get_descent,                    MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "get_lineskip",                   mrb_sdl2_ttf_font_get_lineskip,                   MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "get_kerning",                    mrb_sdl2_ttf_font_get_kerning,                    MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "set_kerning",                    mrb_sdl2_ttf_font_set_kerning,                    MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "get_faces",                      mrb_sdl2_ttf_font_get_faces,                      MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "face_is_fixed_width?",           mrb_sdl2_ttf_font_face_fixed_width,               MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "get_face_family_name",           mrb_sdl2_ttf_font_get_face_family_name,           MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "get_face_style_name",            mrb_sdl2_ttf_font_get_face_style_name,            MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Font, "get_glyph_index",                mrb_sdl2_ttf_font_get_glyph_index,                MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "get_glyph_metric",               mrb_sdl2_ttf_font_get_glyph_metric,               MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "size_text",                      mrb_sdl2_ttf_font_get_size_text,                  MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "size_UTF8",                      mrb_sdl2_ttf_font_get_size_UTF8,                  MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "size_UNICODE",                   mrb_sdl2_ttf_font_get_size_UNICODE,               MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Font, "render_text",                    mrb_sdl2_ttf_font_render_text,                    MRB_ARGS_REQ(5));
  mrb_define_method(mrb, class_Font, "render_UTF8",                    mrb_sdl2_ttf_font_render_UTF8,                    MRB_ARGS_REQ(5));
  mrb_define_method(mrb, class_Font, "render_UNICODE",                 mrb_sdl2_ttf_font_render_UNICODE,                 MRB_ARGS_REQ(5));
  mrb_define_method(mrb, class_Font, "render_glyph",                   mrb_sdl2_ttf_font_render_glyph,                   MRB_ARGS_REQ(5));
  mrb_define_method(mrb, class_Font, "render_text_shaded",             mrb_sdl2_ttf_font_render_text_shaded,             MRB_ARGS_REQ(9));
  mrb_define_method(mrb, class_Font, "render_UTF8_shaded",             mrb_sdl2_ttf_font_render_UTF8_shaded,             MRB_ARGS_REQ(9));
  mrb_define_method(mrb, class_Font, "render_UNICODE_shaded",          mrb_sdl2_ttf_font_render_UNICODE_shaded,          MRB_ARGS_REQ(9));
  mrb_define_method(mrb, class_Font, "render_glyph_shaded",            mrb_sdl2_ttf_font_render_glyph_shaded,            MRB_ARGS_REQ(9));
  mrb_define_method(mrb, class_Font, "render_text_blended",            mrb_sdl2_ttf_font_render_text_blended,            MRB_ARGS_REQ(5));
  mrb_define_method(mrb, class_Font, "render_UTF8_blended",            mrb_sdl2_ttf_font_render_UTF8_blended,            MRB_ARGS_REQ(5));
  mrb_define_method(mrb, class_Font, "render_UNICODE_blended",         mrb_sdl2_ttf_font_render_UNICODE_blended,         MRB_ARGS_REQ(5));
  mrb_define_method(mrb, class_Font, "render_glyph_blended",           mrb_sdl2_ttf_font_render_glyph_blended,           MRB_ARGS_REQ(5));
  mrb_define_method(mrb, class_Font, "render_text_blended_wrapped",    mrb_sdl2_ttf_font_render_text_blended_wrapped,    MRB_ARGS_REQ(6));
  mrb_define_method(mrb, class_Font, "render_UTF8_blended_wrapped",    mrb_sdl2_ttf_font_render_UTF8_blended_wrapped,    MRB_ARGS_REQ(6));
  mrb_define_method(mrb, class_Font, "render_UNICODE_blended_wrapped", mrb_sdl2_ttf_font_render_UNICODE_blended_wrapped, MRB_ARGS_REQ(6));
  mrb_define_method(mrb, class_Font, "get_kerning_size",               mrb_sdl2_ttf_font_get_kerning_size,               MRB_ARGS_REQ(2));

  arena_size = mrb_gc_arena_save(mrb);
  mrb_define_const(mrb, mod_Ttf, "UNICODE_BOM_NATIVE",  mrb_fixnum_value(UNICODE_BOM_NATIVE));
  mrb_define_const(mrb, mod_Ttf, "UNICODE_BOM_SWAPPED", mrb_fixnum_value(UNICODE_BOM_SWAPPED));
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mrb_define_const(mrb, mod_Ttf, "TTF_STYLE_NORMAL",        mrb_fixnum_value(TTF_STYLE_NORMAL));
  mrb_define_const(mrb, mod_Ttf, "TTF_STYLE_BOLD",          mrb_fixnum_value(TTF_STYLE_BOLD));
  mrb_define_const(mrb, mod_Ttf, "TTF_STYLE_ITALIC",        mrb_fixnum_value(TTF_STYLE_ITALIC));
  mrb_define_const(mrb, mod_Ttf, "TTF_STYLE_UNDERLINE",     mrb_fixnum_value(TTF_STYLE_UNDERLINE));
  mrb_define_const(mrb, mod_Ttf, "TTF_STYLE_STRIKETHROUGH", mrb_fixnum_value(TTF_STYLE_STRIKETHROUGH));
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mrb_define_const(mrb, mod_Ttf, "TTF_HINTING_NORMAL",  mrb_fixnum_value(TTF_HINTING_NORMAL));
  mrb_define_const(mrb, mod_Ttf, "TTF_HINTING_LIGHT",   mrb_fixnum_value(TTF_HINTING_LIGHT));
  mrb_define_const(mrb, mod_Ttf, "TTF_HINTING_MONO",    mrb_fixnum_value(TTF_HINTING_MONO));
  mrb_define_const(mrb, mod_Ttf, "TTF_HINTING_NONE",    mrb_fixnum_value(TTF_HINTING_NONE));
  mrb_gc_arena_restore(mrb, arena_size);
}

void
mrb_mruby_sdl2_ttf_gem_final(mrb_state *mrb)
{

}
