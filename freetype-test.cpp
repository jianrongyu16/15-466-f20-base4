#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>
#include "ColorTextureProgram.hpp"
#include <glm/gtc/type_ptr.hpp>
//#include "Load.hpp"

unsigned int VAO, VBO;
static Load< void > setup_buffers(LoadTagDefault, []() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
});
/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    const int texture_id; // ID handle of the glyph texture
    const int width;
    const int height;
    const double offset_x;
    const double offset_y;
    const double advance_x;
    const double advance_y;
    Character(int texture_id, int width, int height, double offset_x, double offset_y, double advance_x, double advance_y) :
            texture_id(texture_id), width(width), height(height), offset_x(offset_x), offset_y(offset_y), advance_x(advance_x), advance_y(advance_y)
    {}
};
//This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
//You probably shouldn't be looking here to learn to use either library.

int main(int argc, char **argv) {
//	FT_Library library;
//    FT_Face face;
//	FT_Init_FreeType( &library );
//
//	hb_buffer_t *buf = hb_buffer_create();
//    hb_buffer_add_utf8(buf, "Apple", -1, 0, -1);
//    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
//    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
//    hb_buffer_set_language(buf, hb_language_from_string("en", -1));
//
//    printf("123\n");
//    FT_New_Face(library, "/Users/JianrongYu/Desktop/15-666/15-466-f20-base4/dist/Roboto-Light.ttf", 0, &face);
//    printf("123\n");
//    FT_Set_Char_Size(face, 0, 1000, 0, 0);
//    printf("123\n");
//    hb_font_t *font = hb_ft_font_create(face, NULL);
//    printf("123\n");
//    hb_shape(font, buf, NULL, 0);
//    printf("123\n");
//    unsigned int glyph_count;
//    hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
//    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
//    printf("123\n");
//    double cursor_x = 0.0, cursor_y = 0.0;
//    for (int i = 0; i < glyph_count; ++i) {
//        auto glyphid = glyph_info[i].codepoint;
//        auto x_offset = glyph_pos[i].x_offset / 64.0;
//        auto y_offset = glyph_pos[i].y_offset / 64.0;
//        auto x_advance = glyph_pos[i].x_advance / 64.0;
//        auto y_advance = glyph_pos[i].y_advance / 64.0;
//        //   draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
//        printf("%d %f %f\n", glyphid, cursor_x + x_offset, cursor_y + y_offset);
//        cursor_x += x_advance;
//        cursor_y += y_advance;
//    }
//	hb_buffer_destroy(buf);

    glm::uvec2 drawable_size = glm::uvec2(2560, 1440);
    std::cout<<drawable_size.x<<" "<<drawable_size.y<<std::endl;
    FT_Library library;
    FT_Face face;
    auto error = FT_Init_FreeType(&library);
    if (error)
    {
        std::cout << "Libary not loaded." << std::endl;
    }
    std::cout<<drawable_size.x<<" "<<drawable_size.y<<std::endl;
    // https://harfbuzz.github.io/ch03s03.html
    // Create a buffer and put your text in it.
    hb_buffer_t* buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, "ABCDEFG", -1, 0, -1);

    // Set the script, language and direction of the buffer.
    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", -1));

    // Create a face and a font, using FreeType for now. // just do once?
    error = FT_New_Face(library, "dist/Roboto-Light.ttf", 0, &face); // considering loading font file to memory
    if (error)
    {
        std::cout << "Failed to create face." << std::endl;
    }
    std::cout<<drawable_size.x<<" "<<drawable_size.y<<std::endl;
    FT_Set_Char_Size(face, 0, 1000, 0, 0);
    hb_font_t* font = hb_ft_font_create(face, NULL);

    // Shape
    hb_shape(font, buf, NULL, 0);

    // Get the glyph and position information.
    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

    // handle named slot that points to the face object's glyph slot
//	FT_GlyphSlot  slot = face->glyph;

    static bool loaded = false;
    static std::vector<Character> characters;
    std::cout<<drawable_size.x<<" "<<drawable_size.y<<std::endl;
    if (!loaded)
    {
        std::cout<<"which"<<std::endl;
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        std::cout<<"glyph_count"<<std::endl;
        // load textures
        for (size_t i = 0; i < glyph_count; ++i) {

            // glyph constants
            auto glyphid = glyph_info[i].codepoint; // the id specifies the specific letter
            double x_offset = glyph_pos[i].x_offset / 64.0;
            double y_offset = glyph_pos[i].y_offset / 64.0;
            double x_advance = glyph_pos[i].x_advance / 64.0;
            double y_advance = glyph_pos[i].y_advance / 64.0;

            // load glyph image
            error = FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
            if (error)
            {
                std::cout << "Load glyph error" << std::endl;
            }

            // convert to bitmap
            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            if (error)
            {
                std::cout << "Render error" << std::endl;
            }
            int w = face->glyph->bitmap.width;
            int h = face->glyph->bitmap.rows;

            // heavily following: https://learnopengl.com/In-Practice/Text-Rendering
            glUseProgram(color_texture_program->program);

            // 1 color
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // store into texture
            unsigned int texture;
            glGenTextures(1, &texture);

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            // create character and push back
            characters.push_back({ (int)texture,w,h, x_offset, y_offset,x_advance,y_advance });

        }
        // configure VAO/VBO for texture quads
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        loaded = true;
    }
    std::cout<<drawable_size.x<<" "<<drawable_size.y<<std::endl;
    // DRAW
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw
    // start position
    double cursor_x = 0;
    double cursor_y = 0;
    for (size_t i = 0; i < glyph_count; ++i) {

        Character c = characters[i];
        // heavily following: https://learnopengl.com/In-Practice/Text-Rendering
        glm::mat4 to_clip = glm::mat4( //n.b. column major(!)
                1 * 2.0f / float(drawable_size.x), 0.0f, 0.0f, 0.0f,
                0.0f, 1 * 2.0f / float(drawable_size.y), 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                2.0f / float(drawable_size.x), 2.0f / float(drawable_size.y), 0.0f, 1.0f
        );
        glUseProgram(color_texture_program->program);
        glUniform3f(glGetUniformLocation(color_texture_program->program, "textColor"), 1.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(color_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(to_clip));
        glBindVertexArray(VAO);

        float xpos = (float)cursor_x + (float)c.offset_x;
        float ypos = (float)cursor_y + (float)c.offset_y;
        int w = c.width;
        int h = c.height;

        // update VBO for each character
        // two triangles!!!
        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // render glyph texture over quad
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, c.texture_id);

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad (run opengl pipeline)
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // update start
        cursor_x += c.advance_x;
        cursor_y += c.advance_y;
    }

    // clean up face
    FT_Done_Face(face);

    // close out
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    //// Tidy up.
    hb_buffer_destroy(buf);
    hb_font_destroy(font);
}
