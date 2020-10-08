/* demo_menu.cpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#include "demo_menu.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include "PlayMode.hpp"


std::shared_ptr< MenuMode > demo_menu;

Load< void > load_demo_menu(LoadTagDefault, []() {
	std::vector< MenuMode::Item > items;
	items.emplace_back("You own a wolf, a sheep and a plant. You try to sell them off. You are at the left bank of this river and there's a shop. There's another shop at the other side of the river.\n"
                    "Once you moved all your stocks to the other side, you can ask that shop for their offers. Compare prices and sell them wisely. Oh, your animals might be hungry!");
	items.emplace_back("Continue");
	items.back().on_select = [](MenuMode::Item const&) {
	    Mode::switch_to_play();
	};

	demo_menu = std::make_shared< MenuMode >(items);
	demo_menu->draw_items();
	demo_menu->selected = 1;
	demo_menu->view_min = glm::vec2(0.0f, 0.0f);
	demo_menu->view_max = glm::vec2(320.0f, 200.0f);

	demo_menu->layout_items(2.0f);
	demo_menu->left_select_offset = glm::vec2(-5.0f - 3.0f, 0.0f);
	demo_menu->right_select_offset = glm::vec2(0.0f + 3.0f, 0.0f);
	demo_menu->select_bounce_amount = 5.0f;
});
