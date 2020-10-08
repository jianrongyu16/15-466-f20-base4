#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "demo_menu.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>


struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space, backspace;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	Scene::Transform *boat = nullptr;
    Scene::Transform *wolf = nullptr;
	Scene::Transform *sheep = nullptr;
	Scene::Transform *plant = nullptr;

    Scene::Transform *res_wolf = nullptr;
    Scene::Transform *res_sheep = nullptr;
    Scene::Transform *res_plant = nullptr;

	bool plant_sold = false;
    bool sheep_sold = false;
    int money = 0;
    float dead = 0.0f;

    std::vector<bool> sell;
	
	//camera:
	Scene::Camera *camera = nullptr;

    bool check_all_passed();
    void move_animals(std::vector< MenuMode::Item > &items);
    void sell_animals_phase0(std::vector< MenuMode::Item > &items);
    void sell_animals_phase1(std::vector< MenuMode::Item > &items);
    void end_scene(std::vector< MenuMode::Item > &items, bool good);

};
//
//extern std::shared_ptr< PlayMode > playmode;
