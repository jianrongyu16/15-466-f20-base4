#include "Mode.hpp"
#include <iostream>


std::shared_ptr< Mode > Mode::current;
std::shared_ptr< Mode > Mode::demo;
std::shared_ptr< Mode > Mode::play;
std::vector<bool> Mode::positions;
std::vector<bool> Mode::sell;

void Mode::set_current(std::shared_ptr< Mode > const &new_current) {
    std::cout<<"thefuck"<<std::endl;
	current = new_current;
	//NOTE: may wish to, e.g., trigger resize events on new current mode.
}

//void Mode::switch_pre_curr(int command) {
//    std::shared_ptr< Mode > temp = current;
//    current = previous;
//    previous = temp;
//    this.command = command;
////NOTE: may wish to, e.g., trigger resize events on new current mode.
//}

void Mode::set_demo(std::shared_ptr< Mode > const &d) {
    demo = d;
    current = demo;
    //NOTE: may wish to, e.g., trigger resize events on new current mode.
}

void Mode::switch_to_demo() {
    current = demo;
    //NOTE: may wish to, e.g., trigger resize events on new current mode.
}

void Mode::set_play(std::shared_ptr< Mode > const &p) {
    play = p;
    current = play;
    //NOTE: may wish to, e.g., trigger resize events on new current mode.
}

void Mode::switch_to_play(std::vector<bool> change, std::vector<bool> s) {
//    std::cout<<"thefuck"<<std::endl;
    for (uint32_t i = 0; i<4; i++) {
        if (change[i]) positions[i] = !positions[i];
    }
    sell = s;
    std::cout<<s[0]<<s[1]<<s[2]<<std::endl;
    std::cout<<positions[3]<<std::endl;
    current = play;
    //NOTE: may wish to, e.g., trigger resize events on new current mode.
}