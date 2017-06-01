#pragma once

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

class Util {
public:
	static pplx::task<json::value> makeJsonRequest(std::string url);
	static int getUserInputAsInteger(const std::string& choice_string, std::pair<int, int> input_range);
	static std::string formatArraysToPrintable(const std::vector<std::string> & arr);
};