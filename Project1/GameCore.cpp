#include <exception>
#include "GameCore.h"
#include <ctype.h>

GameCore::GameCore() {
	quote_requests_.push_back(createQuoteRequest(num_of_choices_));
}

void GameCore::setNumOfChoices(int num_of_choices) {
	if (num_of_choices != num_of_choices_) { // if number of choice is changed, clear all pending requests
		quote_requests_.clear();
		quote_requests_.push_back(createQuoteRequest(num_of_choices));
	}
	num_of_choices_ = num_of_choices;
}

bool GameCore::registerQuoteId(const std::string & quote_id) {
	if (quote_ids_.find(quote_id) != quote_ids_.end()) return false;
	quote_ids_.insert(quote_id);
	return true;
}

void GameCore::addQuote(std::vector<std::string> & quotes_container, const json::value & quote_json, const string_t & id_key, const string_t & quote_key) {
	const auto & quote_id = utility::conversions::to_utf8string(quote_json.at(id_key).as_string());

	if (registerQuoteId(quote_id)) {
		const auto & quote = utility::conversions::to_utf8string(quote_json.at(quote_key).as_string());
		quotes_container.emplace_back(quote);
	}
}

void GameCore::updateQuoteUrl() {
	if (current_category_idx_ < 0 || current_category_idx_ >= categories_.size()) {
		current_category_idx_ = 0;
	}

	const auto & category = categories_[current_category_idx_];

	if (category == "Random") {
		cur_quote_url_ = QUOTE_URL;
	}
	else {
		cur_quote_url_ = QUOTE_URL + "?category=" + category;
	}
}


void GameCore::setCategories(const std::vector<std::string> & categories) {
	if (categories.empty()) {
		throw std::exception("CATEGORY EMPTY");
	}
	categories_ = categories;

	updateQuoteUrl();

	quote_requests_.clear();
	quote_requests_.push_back(createQuoteRequest(num_of_choices_));
}


void GameCore::setCategory(int idx) {
	bool need_clear_tasks = !(idx == current_category_idx_);
	current_category_idx_ = idx;
	updateQuoteUrl();
	if (need_clear_tasks) {
		quote_requests_.clear();
		quote_requests_.push_back(createQuoteRequest(num_of_choices_));
	}
}


pplx::task<std::vector<std::string>> GameCore::createQuoteRequest(int quote_num) {
	std::vector<pplx::task<json::value>> request_tasks;

	for (int i = 0; i < quote_num; ++i) {
		request_tasks.emplace_back(Util::makeJsonRequest(cur_quote_url_));
	}

	auto quote_key = utility::conversions::to_string_t("value");
	auto id_key = utility::conversions::to_string_t("id");

	return pplx::when_all(std::begin(request_tasks), std::end(request_tasks)).then([=](const std::vector<json::value> & returned_jsons) {

		std::vector<std::string> random_quotes;

		for (const auto & quote_json : returned_jsons) {
			addQuote(random_quotes, quote_json, id_key, quote_key);
		}

		int retry_counter = 0;

		// in case there are duplicates
		while (random_quotes.size() < quote_num) {
			++retry_counter;

			if (retry_counter > MAX_QUOTE_RETRY) {
				break;
			}

			// query individual quote
			auto query_task = Util::makeJsonRequest(cur_quote_url_);
			query_task.wait();
			// 
			const auto & quote_json = query_task.get();

			addQuote(random_quotes, quote_json, id_key, quote_key);
		}

		// return empty if exceeds max retry
		if (retry_counter > MAX_QUOTE_RETRY) {
			return pplx::task_from_result(std::vector<std::string>());
		}

		return pplx::task_from_result(random_quotes);
	});
}


std::unordered_set<int> GameCore::getAnswer(const std::vector<std::string>& quotes) {

	std::unordered_set<int> answers;

	int most_caps = 0;

	for (int i = 0; i < quotes.size(); ++i) {
		const auto & quote = quotes[i];
		
		int caps_count = 0;

		for (const auto c : quote) {
			caps_count += isupper(c);
		}

		if (caps_count > most_caps) {
			answers.clear();
			answers.insert(i + 1);
			most_caps = caps_count;
		}
		else if (caps_count == most_caps) {
			answers.insert(i + 1);
		}
	}

	return answers;
}


void GameCore::startGame() {
	const auto query_task = getNextQuoteRequest();

	const auto & quotes = query_task.get();
	
	if (quotes.empty()) {
		throw std::exception("NO MORE QUOTES. PLEASE RESET or CHANGE CATEGORIES");
	}

	const auto & formatted_quotes = Util::formatArraysToPrintable(quotes);

	auto user_answer = Util::getUserInputAsInteger(formatted_quotes, { 1, quotes.size() });

	const auto & answer = getAnswer(quotes);


	system("cls");

	stats_.num_of_tries++;

	if (answer.find(user_answer) != answer.end()){
		stats_.num_of_wins++;
		std::cout << "NICE !";
	}
	else {
		std::cout << "BOO ...";
	}
	std::cout << std::endl << std::endl;

	if (stats_.num_of_tries % 10 == 0) {
		std::cout << printGameStat() << std::endl;
	}

	system("pause");
}


pplx::task<std::vector<std::string>> GameCore::getNextQuoteRequest() {
	if (quote_requests_.empty()) {
		quote_requests_.push_back(createQuoteRequest(num_of_choices_));
	}
	auto task = quote_requests_.front();
	quote_requests_.pop_front();
	quote_requests_.push_back(createQuoteRequest(num_of_choices_));
	return task;
}

void GameCore::reset() {
	quote_requests_.clear();
	quote_ids_.clear();
	quote_requests_.push_back(createQuoteRequest(num_of_choices_));

	stats_ = GameStat();
}

std::string GameCore::printGameStat() {
	std::stringstream ss;

	double percetage = stats_.num_of_tries == 0 ? 100 : (double)(stats_.num_of_wins) * 100 / stats_.num_of_tries;

	ss.precision(2);

	ss << "GAMES: " << stats_.num_of_tries << "\nWINS: " << stats_.num_of_wins << "\n%: " << std::fixed << percetage << std::endl;
	return ss.str();
}

std::vector<std::string> GameCore::getCategories() {
	return categories_;
}