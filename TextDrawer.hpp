#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <hb.h>
#include <hb-ft.h>
#include "ColorTextureProgram.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <functional>
#include <string>

struct TextDrawer {
    TextDrawer();
    virtual ~TextDrawer();
    FT_Library library;
    FT_Face face;
    hb_font_t* font;
    hb_buffer_t* buf;

    struct Character {
        const int texture;
        const int width;
        const int height;
        const double line;
        const float start_x;
        const float start_y;
        const float bearing_x;
        const float bearing_y;
        const float x_offset;
        const float y_offset;
        const float x_advance;
        const float y_advance;
        const float red;
        const float green;
        const float blue;
    };

    std::vector<Character> characters;
    void load_text(const char * texts, float x, float y, float red, float green, float blue, double line);
};