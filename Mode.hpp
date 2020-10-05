#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

struct Mode : std::enable_shared_from_this< Mode > {
	virtual ~Mode() { }

	//handle_event is called when new mouse or keyboard events are received:
	// (note that this might be many times per frame or never)
	//The function should return 'true' if it handled the event.
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) { return false; }

	//update is called at the start of a new frame, after events are handled:
	// 'elapsed' is time in seconds since the last call to 'update'
	virtual void update(float elapsed) { }

	//draw is called after update:
	virtual void draw(glm::uvec2 const &drawable_size) = 0;

	//Mode::current is the Mode to which events are dispatched.
	// use 'set_current' to change the current Mode (e.g., to switch to a menu)
	static std::shared_ptr< Mode > current;
    static std::shared_ptr< Mode > demo;
    static std::shared_ptr< Mode > play;
    static std::vector<bool> positions;
    static std::vector<bool> sell;
	static void set_current(std::shared_ptr< Mode > const &);
    static void set_demo(std::shared_ptr< Mode > const &);
    static void set_play(std::shared_ptr< Mode > const &);
    static void switch_to_demo();
    static void switch_to_play(std::vector<bool> change, std::vector<bool> s);
//    static void switch_pre_curr();
};

