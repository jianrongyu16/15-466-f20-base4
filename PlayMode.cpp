#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("area.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("area.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});


PlayMode::PlayMode() : scene(*hexapod_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "Boat") boat = &transform;
		else if (transform.name == "Wolf") {
		    wolf = &transform;
            res_wolf = &transform;
		}
        else if (transform.name == "Sheep") {
            sheep = &transform;
            res_sheep = &transform;
        }
        else if (transform.name == "Plant") {
            plant = &transform;
            res_plant = &transform;
        }
	}
	sell.push_back(false);
    sell.push_back(false);
    sell.push_back(false);


	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
            space.pressed = true;
            return true;
        } else if (evt.key.keysym.sym == SDLK_BACKSPACE) {
            backspace.pressed = true;
            return true;
        }
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
    if (boat->position.x>0) {
        if (sheep&&plant&&sheep->position.x<0 && plant->position.x<0) {
            plant->position.z = -10;
            plant=nullptr;
        }
        if (sheep&&wolf&&wolf->position.x<0 && sheep->position.x<0) {
            sheep->position.z = -10;
            sheep=nullptr;
        }
    } else {
        if (sheep&&plant&&sheep->position.x>0 && plant->position.x>0) {
            plant->position.z = -10;
            plant=nullptr;
        }
        if (sheep&&wolf&&wolf->position.x>0 && sheep->position.x>0) {
            sheep->position.z = -10;
            sheep=nullptr;
        }
    }

    if (!wolf &&!sheep&&!plant) {
        std::vector< MenuMode::Item > items;
        end_scene(items, money>=200);
        demo_menu->update_items(items);
        Mode::switch_to_demo();
        return;
    }

    if (check_all_passed()&&backspace.pressed) {
        backspace.pressed = false;
        std::vector< MenuMode::Item > items;
        if (boat->position.x<0) {
            sell_animals_phase0(items);
        } else {
            sell_animals_phase1(items);
        }
        demo_menu->update_items(items);
        Mode::switch_to_demo();
    }

    if (space.pressed) {
        space.pressed = false;
        std::vector< MenuMode::Item > items;
        move_animals(items);
        demo_menu->update_items(items);
        Mode::switch_to_demo();
    }

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
    glDisable(GL_BLEND);
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));
        constexpr float H = 0.09f;
        std::string s0 = "plant has been eaten by sheep";
        std::string s1 = "sheep has been eaten by wolf";
        std::string s2 = "plant has been sold";
        std::string s3 = "sheep has been sold";
        std::string s4 = "wolf has been sold";
        std::string s5 = "Money: "+std::to_string(money);
        std::string s6 = "Press SPACE to continue to moving options";
        std::string s7 = "Press BACKSPACE to continue to selling options";
        std::string s8 = "You can do better than this!";
        std::string s9 = "You nailed it!";
        lines.draw_text(s5,
           glm::vec3(1.4f, 0.9f, 0.0),
           glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
           glm::u8vec4(0xff, 0xff, 0xff, 0x00));
        lines.draw_text(s6,
                        glm::vec3(-0.7f, 0.9f, 0.0),
                        glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                        glm::u8vec4(0xff, 0xff, 0xff, 0x00));
        if (check_all_passed()) lines.draw_text(s7,
                        glm::vec3(-0.7f, 0.8f, 0.0),
                        glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                        glm::u8vec4(0xff, 0xff, 0xff, 0x00));
        if (dead>0) lines.draw_text(money==200?s9:s8,
                                    glm::vec3(-0.7f, 0.4f, 0.0),
                                    glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                                    glm::u8vec4(0xff, 0xff, 0xff, 0x00));
        if (!plant)lines.draw_text(sell[2]?s2:s0,
                                   glm::vec3(-0.4f, 0.7f, 0.0),
                                   glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                                   glm::u8vec4(0xff, 0xff, 0xff, 0x00));
        if (!sheep)lines.draw_text(sell[1]?s3:s1,
                                    glm::vec3(-0.4f, 0.6f, 0.0),
                                    glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                                    glm::u8vec4(0xff, 0xff, 0xff, 0x00));
        if (!wolf)lines.draw_text(s4,
                                   glm::vec3(-0.4f, 0.5f, 0.0),
                                   glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                                   glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
	GL_ERRORS();
}

bool PlayMode::check_all_passed() {
    if (boat->position.x<0) {
        if (wolf&&wolf->position.x>0) return false;
        if (sheep&&sheep->position.x>0) return false;
        if (plant&&plant->position.x>0) return false;
        return true;
    }
    if (wolf&&wolf->position.x<0) return false;
    if (sheep&&sheep->position.x<0) return false;
    if (plant&&plant->position.x<0) return false;
    return true;
}

void PlayMode::move_animals(std::vector< MenuMode::Item > &items){
    items.emplace_back("Move one of them to the other side");
    if (wolf&&(boat->position.x<0)==(wolf->position.x<0)){
        items.emplace_back("Wolf");
        items.back().on_select = [this](MenuMode::Item const&) {
            wolf->position.x = -wolf->position.x;
            boat->position.x = -boat->position.x;
            Mode::switch_to_play();
        };
    }
    if (sheep&&(boat->position.x<0)==(sheep->position.x<0)) {
        items.emplace_back("Sheep");
        items.back().on_select = [this](MenuMode::Item const &) {
            sheep->position.x = -sheep->position.x;
            boat->position.x = -boat->position.x;
            Mode::switch_to_play();
        };
    }
    if (plant&&(boat->position.x<0)==(plant->position.x<0)) {
        items.emplace_back("Plant");
        items.back().on_select = [this](MenuMode::Item const &) {
            plant->position.x = -plant->position.x;
            boat->position.x = -boat->position.x;
            Mode::switch_to_play();
        };
    }
    items.emplace_back("Only the boat");
    items.back().on_select = [this](MenuMode::Item const&) {
        boat->position.x = -boat->position.x;
        Mode::switch_to_play();
    };
    items.emplace_back("None");
    items.back().on_select = [](MenuMode::Item const&) {
        Mode::switch_to_play();
    };
}

void PlayMode::sell_animals_phase0(std::vector< MenuMode::Item > &items){
    items.emplace_back("The shop here offer you the following options. "
                       "You can always come back.");
    if (wolf) {
        items.emplace_back("Sell Wolf for 100");
        items.back().on_select = [this](MenuMode::Item const &) {
            sell[0] = true;
            wolf->position.z = -10;
            wolf=nullptr;
            money+=100;
            Mode::switch_to_play();
        };
    }
    if (sheep) {
        items.emplace_back("Sell Sheep for 50");
        items.back().on_select = [this](MenuMode::Item const &) {
            sell[1] = true;
            sheep->position.z = -10;
            sheep=nullptr;
            money+=50;
            Mode::switch_to_play();
        };
    }
    if (plant) {
        items.emplace_back("Sell plant for 10");
        items.back().on_select = [this](MenuMode::Item const &) {
            sell[2] = true;
            plant->position.z = -10;
            plant=nullptr;
            money+=10;
            Mode::switch_to_play();
        };
    }
    items.emplace_back("Sell Nothing");
    items.back().on_select = [](MenuMode::Item const&) {
        Mode::switch_to_play();
    };

}

void PlayMode::sell_animals_phase1(std::vector< MenuMode::Item > &items){
    items.emplace_back("The shop here offer you the following options. "
                       "You can always come back.");
    if (wolf){
        items.emplace_back("Sell Wolf for 130");
        items.back().on_select = [this](MenuMode::Item const&) {
            sell[0] = true;
            wolf->position.z = -10;
            wolf=nullptr;
            money+=130;
            Mode::switch_to_play();
        };
    }
    if (sheep) {
        items.emplace_back("Sell Sheep for 30");
        items.back().on_select = [this](MenuMode::Item const &) {
            sell[1] = true;
            sheep->position.z = -10;
            sheep=nullptr;
            money+=30;
            Mode::switch_to_play();
        };
    }
    if (plant) {
        items.emplace_back("Sell plant for 20");
        items.back().on_select = [this](MenuMode::Item const &) {
            sell[2] = true;
            plant->position.z = -10;
            plant=nullptr;
            money+=20;
            Mode::switch_to_play();
        };
    }
    items.emplace_back("Sell Nothing");
    items.back().on_select = [](MenuMode::Item const&) {
        Mode::switch_to_play();
    };
}

void PlayMode::end_scene(std::vector< MenuMode::Item > &items, bool good){
    if (good)items.emplace_back("You nailed it!");
    else items.emplace_back("This doesn't even cover the cost! You can do better than this!");
    items.emplace_back("Continue");
    items.back().on_select = [this](MenuMode::Item const&) {
        plant_sold = false;
        sheep_sold = false;
        wolf = res_wolf;
        sheep = res_sheep;
        plant = res_plant;
        wolf->position.x = -std::abs(wolf->position.x);
        sheep->position.x = -std::abs(sheep->position.x);
        plant->position.x = -std::abs(plant->position.x);
        wolf->position.z = 0.1;
        sheep->position.z = 0.01;
        plant->position.z = 1;
        boat->position.x = -std::abs(boat->position.x);
        money = 0;
        sell.clear();
        sell.push_back(false);
        sell.push_back(false);
        sell.push_back(false);
        Mode::switch_to_play();
    };
}
