
#include "TextDrawer.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"


//for loading:
#include "data_path.hpp"

#include <random>



TextDrawer::TextDrawer(){
    auto error = FT_Init_FreeType(&library);
    if (error)
    {
        std::cout << "Libary not loaded." << std::endl;
    }

    error = FT_New_Face(library, &data_path("Roboto-Light.ttf")[0], 0, &face);
    if (error)
    {
        std::cout << "Failed to create face." << std::endl;
    }
    FT_Set_Char_Size(face, 0, 2000, 0, 0);
    font = hb_ft_font_create(face, NULL);
    buf = hb_buffer_create();
}

TextDrawer::~TextDrawer() {
    FT_Done_Face(face);
    hb_buffer_destroy(buf);
    hb_font_destroy(font);
}

void TextDrawer::load_text(const char * texts, float x, float y, float red, float green, float blue, double line){

    hb_buffer_reset(buf);
    hb_buffer_add_utf8(buf, texts, -1, 0, -1);

    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", -1));

    hb_shape(font, buf, NULL, 0);

    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

    for (size_t i = 0; i < glyph_count; ++i) {

        auto glyphid = glyph_info[i].codepoint;
        double x_offset = glyph_pos[i].x_offset / 64.0;
        double y_offset = glyph_pos[i].y_offset / 64.0;
        double x_advance = glyph_pos[i].x_advance / 64.0;
        double y_advance = glyph_pos[i].y_advance / 64.0;

        auto error = FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
        if (error)
        {
            std::cout << "Load glyph error" << std::endl;
        }

        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (error)
        {
            std::cout << "Render error" << std::endl;
        }
        int w = face->glyph->bitmap.width;
        int h = face->glyph->bitmap.rows;

        float l = (float) face->glyph->bitmap_left;
        float t = (float) face->glyph->bitmap_top;

        glUseProgram(color_texture_program->program);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned int texture;
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        glBindTexture(GL_TEXTURE_2D, 0);

        characters.push_back({(int)texture, w, h, line, x, y, l,t, (float)x_offset, (float)y_offset, (float)x_advance, (float)y_advance, red, green, blue});
    }
}