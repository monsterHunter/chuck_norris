#pragma once

#include <vector>
#include <string>
#include <cstdlib>
#include "GameCore.h"

class ChuckNorris {
	const int MAX_NUM_OF_CHOICE = 10;
	const std::string CATEGORY_URL = "https://api.chucknorris.io/jokes/categories";

	
	const std::vector<std::string> main_menu_items_ = { "START", "SETTINGS", "STATS", "RESET", "EXIT" };
	const std::vector<std::string> settings_menu_items_ = { "NUMBER OF CHOICE", "CATEGORIES" };

	std::string main_menu_string_;
	std::string settings_menu_string_;
	std::string num_of_choice_string_ = "Number of Choices: ";
	std::string category_string_;

	GameCore game_core_;

	void init();

	void printSettings();
	void printWelcome();
	void setNumOfChoices();
	void setCategory();
	std::vector<std::string> getQuoteCategories();

public:

	ChuckNorris();
	void start();

};