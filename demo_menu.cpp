/* demo_menu.cpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#include "demo_menu.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include "PlayMode.hpp"


std::shared_ptr< MenuMode > demo_menu;

Load< void > load_demo_menu(LoadTagDefault, []() {
	std::vector< MenuMode::Item > items;
	items.emplace_back("You owns a wolf, a sheep and a plant. You try to sell them off. You are at this left bank of the river and the shop here offer you the following options. "
                    "There's another shop at the other side of the river so you can also choose to compare your prices. You can always come back.");
	items.emplace_back("Sell Wolf for 100");
	items.back().on_select = [](MenuMode::Item const&) {
	    std::vector< bool > c(3, false);
        std::vector< bool > s(3, false);
        s[0] = true;
	    Mode::switch_to_play(c,s);
	};
	items.emplace_back("Sell Sheep for 50");
	items.back().on_select = [](MenuMode::Item const&) {
        std::vector< bool > c(3, false);
        std::vector< bool > s(3, false);
        s[1] = true;
	    Mode::switch_to_play(c,s);
	};

    items.emplace_back("Sell plant for 10");
    items.back().on_select = [](MenuMode::Item const&) {
        std::vector< bool > c(3, false);
        std::vector< bool > s(3, false);
        s[2] = true;
        Mode::switch_to_play(c,s);
    };
    items.emplace_back("Sell Nothing");
    items.back().on_select = [](MenuMode::Item const&) {
        std::vector< bool > c(3, false);
        std::vector< bool > s(3, false);
        Mode::switch_to_play(c,s);
    };


	demo_menu = std::make_shared< MenuMode >(items);
	demo_menu->selected = 1;
	demo_menu->view_min = glm::vec2(0.0f, 0.0f);
	demo_menu->view_max = glm::vec2(320.0f, 200.0f);

	demo_menu->layout_items(2.0f);
	demo_menu->left_select_offset = glm::vec2(-5.0f - 3.0f, 0.0f);
	demo_menu->right_select_offset = glm::vec2(0.0f + 3.0f, 0.0f);
	demo_menu->select_bounce_amount = 5.0f;
});
