
#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>


//This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
//You probably shouldn't be looking here to learn to use either library.

int main(int argc, char **argv) {
	FT_Library library;
    FT_Face face;
	FT_Init_FreeType( &library );

	hb_buffer_t *buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, "Apple", -1, 0, -1);
    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", -1));

    printf("123\n");
    FT_New_Face(library, "/Users/JianrongYu/Desktop/15-666/15-466-f20-base4/dist/Roboto-Light.ttf", 0, &face);
    printf("123\n");
    FT_Set_Char_Size(face, 0, 1000, 0, 0);
    printf("123\n");
    hb_font_t *font = hb_ft_font_create(face, NULL);
    printf("123\n");
    hb_shape(font, buf, NULL, 0);
    printf("123\n");
    unsigned int glyph_count;
    hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
    printf("123\n");
    double cursor_x = 0.0, cursor_y = 0.0;
    for (int i = 0; i < glyph_count; ++i) {
        auto glyphid = glyph_info[i].codepoint;
        auto x_offset = glyph_pos[i].x_offset / 64.0;
        auto y_offset = glyph_pos[i].y_offset / 64.0;
        auto x_advance = glyph_pos[i].x_advance / 64.0;
        auto y_advance = glyph_pos[i].y_advance / 64.0;
        //   draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
        printf("%d %f %f\n", glyphid, cursor_x + x_offset, cursor_y + y_offset);
        cursor_x += x_advance;
        cursor_y += y_advance;
    }
	hb_buffer_destroy(buf);
}
