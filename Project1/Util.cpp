#include <cctype>
#include <sstream>
#include "Util.h"

pplx::task<json::value> Util::makeJsonRequest(std::string url) {
	http_client client(uri(utility::conversions::to_string_t(url)));

	// Handle response headers arriving.
	pplx::task<json::value> request_task = client.request(methods::GET).then([&](http_response response)
	{
		//printf("Received response status code:%u\n", response.status_code());
		// Write response body into the file.
		return response.extract_json();
	});

	return request_task;
}


int Util::getUserInputAsInteger(const std::string& choice_string, std::pair<int, int> input_range) {
	std::string user_input;
	int user_choice;

	while (true) {
		system("cls");

		std::cout << choice_string;
		std::cin >> user_input;

		bool is_numeric = true;

		for (auto c : user_input) {
			if (!std::isdigit(c)) {
				is_numeric = false;
				break;
			}
		}

		if (!is_numeric) continue;

		user_choice = atoi(user_input.c_str());

		if (user_choice >= input_range.first && user_choice <= input_range.second) break;
	}

	return user_choice;
}

std::string Util::formatArraysToPrintable(const std::vector<std::string> & arr) {
	std::stringstream ss;
	for (int i = 0; i < arr.size(); ++i) {
		const auto & item = arr[i];
		ss << i + 1 << ". " << item << std::endl;
	}
	ss << std::endl;
	return ss.str();
}