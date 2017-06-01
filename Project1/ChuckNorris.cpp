#include "Game.h"
#include <iostream>
#include <cctype>
#include <windows.h>
#include <sstream>
#include <exception>
#include <wchar.h>
#include "Util.h"

ChuckNorris::ChuckNorris() {
	init();
}

void ChuckNorris::init() {
	main_menu_string_ = Util::formatArraysToPrintable(main_menu_items_);
	settings_menu_string_ = Util::formatArraysToPrintable(settings_menu_items_);
	const auto & categories = getQuoteCategories();

	game_core_.setCategories(categories);
}

std::vector<std::string> ChuckNorris::getQuoteCategories() {
	const auto request_task = Util::makeJsonRequest(CATEGORY_URL);
	const auto & categories_json = request_task.get();

	const auto & category_arrays_json = categories_json.as_array();

	std::vector<std::string> categories;

	for (const auto & category : category_arrays_json) {
		const auto cate_str = utility::conversions::to_utf8string(category.as_string());
		categories.emplace_back(cate_str);
	}

	categories.emplace_back("Random");

	category_string_ = Util::formatArraysToPrintable(categories);
	
	return categories;
}

void ChuckNorris::printSettings() {
	auto user_choice = Util::getUserInputAsInteger(settings_menu_string_, { 1, settings_menu_items_.size() });

	switch (user_choice) {
	case 1: {
		setNumOfChoices();
		break;
	}
	case 2: {
		setCategory();
		break;
	}
	default: {
		break;
	}
	}
}

void ChuckNorris::printWelcome()
{
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = 30;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s(cfi.FaceName, L"Consolas"); // Choose your font
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

	std::cout << "CHUCK NORRIS JOKES";

	Sleep(3000);

	cfi.dwFontSize.Y = 20;
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void ChuckNorris::setNumOfChoices() {
	auto num_of_choices = Util::getUserInputAsInteger(num_of_choice_string_, { 2, MAX_NUM_OF_CHOICE });
	game_core_.setNumOfChoices(num_of_choices);
}

void ChuckNorris::setCategory() {
	const auto & categories = game_core_.getCategories();

	if (categories.empty()) {
		throw std::exception("CATEGORY EMPTY");
	}
	auto category_idx = Util::getUserInputAsInteger(category_string_, { 1, categories.size() });
	--category_idx;

	game_core_.setCategory(category_idx);
}


void ChuckNorris::start() {
	printWelcome();
	while (1) {
		try {
			auto main_menu_choice = Util::getUserInputAsInteger(main_menu_string_, { 1, main_menu_items_.size() });

			switch (main_menu_choice) {
			case 1: {
				game_core_.startGame();
				break;
			}
			case 2: {
				printSettings();
				break;
			}
			case 3: {
				system("cls");
				std::cout << game_core_.printGameStat() << std::endl;
				system("PAUSE");
				break;
			}
			case 4: {
				game_core_.reset();
				system("cls");
				std::cout << "DONE" << std::endl << std::endl;
				system("PAUSE");
				break;
			}
			case 5: {
				return;
			}
			default: {
				break;
			}
			}
		}
		catch (std::exception e) {
			system("cls");
			std::cout << "ERROR: " << e.what() << std::endl;
			system("PAUSE");
		}
	}
}