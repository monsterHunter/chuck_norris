#pragma once
#include <unordered_set>
#include <deque>
#include "Util.h"

class GameCore {
	
	const std::string QUOTE_URL = "https://api.chucknorris.io/jokes/random";
	const int MAX_QUOTE_RETRY = 10;

	struct GameStat {
		int num_of_tries = 0;
		int num_of_wins = 0;
	} stats_;

	int num_of_choices_ = 4;
	int current_category_idx_ = 0;

	std::string cur_quote_url_ = QUOTE_URL;

	std::unordered_set<std::string> quote_ids_;
	std::deque<pplx::task<std::vector<std::string>>> quote_requests_;
	std::vector<std::string> categories_;

	bool registerQuoteId(const std::string & quote_id);
	void addQuote(std::vector<std::string> & quotes_container, const json::value & quote_json, const string_t & id_key, const string_t & quote_key);
	pplx::task<std::vector<std::string>> createQuoteRequest(int quote_num);
	pplx::task<std::vector<std::string>> getNextQuoteRequest();
	std::unordered_set<int> getAnswer(const std::vector<std::string> & quotes);
	void updateQuoteUrl();

public:

	GameCore();

	void startGame();
	void reset();

	void setNumOfChoices(int num_of_choices);
	void setCategories(const std::vector<std::string> & categories);
	void setCategory(int idx);

	std::vector<std::string> getCategories();

	std::string printGameStat();
};