/* MenuMode.cpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#include "MenuMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for playing movement sounds:
#include "Sound.hpp"

//for loading:
#include "Load.hpp"

#include <random>


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


Load< Sound::Sample > sound_click(LoadTagDefault, []() -> Sound::Sample* {
	std::vector< float > data(size_t(48000 * 0.2f), 0.0f);
	for (uint32_t i = 0; i < data.size(); ++i) {
		float t = i / float(48000);
		//phase-modulated sine wave (creates some metal-like sound):
		data[i] = std::sin(3.1415926f * 2.0f * 440.0f * t + std::sin(3.1415926f * 2.0f * 450.0f * t));
		//quadratic falloff:
		data[i] *= 0.3f * std::pow(std::max(0.0f, (1.0f - t / 0.2f)), 2.0f);
	}
	return new Sound::Sample(data);
	});

Load< Sound::Sample > sound_clonk(LoadTagDefault, []() -> Sound::Sample* {
	std::vector< float > data(size_t(48000 * 0.2f), 0.0f);
	for (uint32_t i = 0; i < data.size(); ++i) {
		float t = i / float(48000);
		//phase-modulated sine wave (creates some metal-like sound):
		data[i] = std::sin(3.1415926f * 2.0f * 220.0f * t + std::sin(3.1415926f * 2.0f * 200.0f * t));
		//quadratic falloff:
		data[i] *= 0.3f * std::pow(std::max(0.0f, (1.0f - t / 0.2f)), 2.0f);
	}
	return new Sound::Sample(data);
	});


MenuMode::MenuMode(std::vector< Item > const& items_) : items(items_) {

	//select first item which can be selected:
	for (uint32_t i = 0; i < items.size(); ++i) {
		if (items[i].on_select) {
			selected = i;
			break;
		}
	}
}

MenuMode::~MenuMode() {
}

bool MenuMode::handle_event(SDL_Event const& evt, glm::uvec2 const& window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_UP) {
			//skip non-selectable items:
			for (uint32_t i = selected - 1; i < items.size(); --i) {
				if (items[i].on_select) {
					selected = i;
					Sound::play(*sound_click);
					break;
				}
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			//note: skips non-selectable items:
			for (uint32_t i = selected + 1; i < items.size(); ++i) {
				if (items[i].on_select) {
					selected = i;
					Sound::play(*sound_click);
					break;
				}
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_RETURN) {
			if (selected < items.size() && items[selected].on_select) {
				Sound::play(*sound_clonk);
				items[selected].on_select(items[selected]);
				return true;
			}
		}
	}
	if (background) {
		return background->handle_event(evt, window_size);
	}
	else {
		return false;
	}
}

void MenuMode::update(float elapsed) {

	if (background) {
		background->update(elapsed);
	}
}

void MenuMode::draw(glm::uvec2 const& drawable_size) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    // DRAW
    glClearColor(0.2f, 0.2f, 0.2f, 0.6f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw
    // start position
    float y_offset = -100.0f;
    float cursor_x = 0;
    float cursor_y = 0;
    double line = -1;
    for (size_t i = 0; i < textDrawer.characters.size(); ++i) {
        TextDrawer::Character c = textDrawer.characters[i];
        glm::mat4 to_clip = glm::mat4( //n.b. column major(!)
                1 * 2.0f / float(drawable_size.x), 0.0f, 0.0f, 0.0f,
                0.0f, 1 * 2.0f / float(drawable_size.y), 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                2.0f / float(drawable_size.x), 2.0f / float(drawable_size.y), 0.0f, 1.0f
        );
        glUseProgram(color_texture_program->program);
        glUniform3f(glGetUniformLocation(color_texture_program->program, "textColor"), c.line == selected?1: c.red, c.line == selected? 1:c.green, c.line == selected? 0:c.blue);
        glUniformMatrix4fv(color_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(to_clip));
        glBindVertexArray(VAO);

        if (c.line!=line){
            cursor_x = drawable_size.x/2.0*c.start_x;
            cursor_y = drawable_size.y/2.0*c.start_y+c.line*y_offset;
            line = c.line;
        }

        float xpos = cursor_x+c.x_offset+c.bearing_x;
        float ypos = cursor_y+c.y_offset-(c.height - c.bearing_y);
        int w = c.width;
        int h = c.height;

        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, c.texture);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        cursor_x+=c.x_advance;
        cursor_y+=c.y_advance;

    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

	GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
}


void MenuMode::layout_items(float gap) {
	float y = (float) view_max.y;
	for (auto& item : items) {
		glm::vec2 min(0.0f), max(0.0f);
		item.at.y = y - max.y;
		item.at.x = 0.5f * (view_max.x + view_min.x) - 0.5f * (max.x + min.x);
		y = y - (max.y - min.y) - gap;
	}
	float ofs = -0.5f * y;
	for (auto& item : items) {
		item.at.y += ofs;
	}
}

void MenuMode::update_items(std::vector<Item> const& items_) {
    items.clear();
    items = items_;
    for (uint32_t i = 0; i < items.size(); ++i) {
        if (items[i].on_select) {
            selected = i;
            break;
        }
    }
    draw_items();
}

void MenuMode::draw_items(){
    double line = 0;
    textDrawer.characters.clear();
    for (auto const& item : items) {
        std::size_t found = item.name.find('\n');
        if (found==std::string::npos){
            textDrawer.load_text(&item.name[0], -0.8f, 0.8f, 1.0f, 1.0f, 1.0f, line);
        } else{
            std::string s1 = item.name.substr(0,found);
            textDrawer.load_text(&s1[0], -0.8f, 0.8f, 1.0f, 1.0f, 1.0f, line);
            std::string s2 = item.name.substr(found+1);
            textDrawer.load_text(&s2[0], -0.8f, 0.8f, 1.0f, 1.0f, 1.0f, line+0.5);
        }
        line++;
    }
}