#include "Main.h"

using namespace curlpp::options;


int main(int argc, char* argv[])
{
	// Function to get the window console to display unicode characters
	// In this case used to write currency symbols to the console
	SetConsoleOutputCP(CP_UTF8);

	// Steps needed to be taken
	// API documentation available here: https://freecurrencyapi.com/docs/

	// ---------- Program Initialization ----------
	// Instantiate AppState
	CurrencyConverter::AppState app_state = CurrencyConverter::AppState();

	// Command line parameter parsing
	if (argc < 2)
	{
		// No api key provided
		std::cerr << "\nNo API KEY provided!\n";
		std::cerr << "Usage: " << "CurrencyConverter.exe" << " <API_KEY>" << std::endl;
		return 1;
	}
	if (argc > 2)
	{
		// Too many arguments
		std::cerr << "\nToo many arguments provided!\n";
		std::cerr << "Usage: " << "CurrencyConverter.exe" << " <API_KEY>" << std::endl;
		return 1;
	}

	// Get api_key from command line arguments
	app_state.api_key = argv[1];
	app_state.currencies;

	try
	{
		// Checks availability of API endpoint status.
		// If status is available then the account data gets written to app_state->account.
		check_api_status(app_state);

		// Get available currencies here for the first time since they will be needed in any case
		get_currencies(app_state);

		// ---------- Program loop ----------
		
		bool program_should_close = false;
		std::string input = "";
		while (!program_should_close)
		{
			write_main_menu(app_state);
			// get input
			std::cout << "\nWhat do you want to do --> ";
			getline(std::cin, input);
			// handle input
			if (input == "A" || input == "a")
			{
				list_available_currencies(app_state);
			}
			if (input == "B" || input == "b")
			{
				write_detailed_currency_information(app_state);
			}
			if (input == "C" || input == "c")
			{
				exchange_money(app_state);
			}
			if (input == "H" || input == "h")
			{
				write_help_menu();
			}
			if (input == "X" || input == "x")
			{
				program_should_close = true;
			}

			// Pause loop by waiting for input
			std::cout << "\n\nTo continue please press enter" << std::endl;
			getline(std::cin, input);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n' << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "\n\n";
	return EXIT_SUCCESS;
}

// This function fetches all exchange rates for a given currency
// The currency is defined by it's currency code
// All exchange rates will be fetched to reduce the number of api calls
void get_exchange_rates(CurrencyConverter::AppState& app_state, CurrencyConverter::Currency& currency)
{
	// Class that handles memory initialization and deletion.
	curlpp::Cleanup cleaner;

	// Set target url of request to the status endpoint.
	curlpp::Easy request;
	request.setOpt(new Url("https://api.freecurrencyapi.com/v1/latest"));

	// Add headers. Here this is the api key and the currency.
	std::list<string> headers {};
	headers.push_back("apikey: " + app_state.api_key);
	headers.push_back("base_currency: " + currency.code);
	request.setOpt(new HttpHeader(headers));

	// Memory location to store the incoming response body
	std::stringstream response_body;
	// Set the response stream as output stream of the request
	request.setOpt(cURLpp::Options::WriteStream(&response_body));

	// Send request and get a result.
	request.perform();

	long response_code = curlpp::infos::ResponseCode::get(request);

	// Handling the possible response codes.
	// Error 403 (Not allowed), 422 (Validation Error) can't / shouldn't happen at this endpoint.
	// Error 404 may happen if the url got changed.
	switch (response_code)
	{
		// Happy case. Write received data to AppState and return.
		// Curly braces are needed so that a scope is properly created and memory can be properly initialized and deleted
		case 200:
			{
				// Example response body
				// {"meta":{"last_updated_at":"2022-01-01T23:59:59Z"},"data":{"AED":3.67306,"AFN":91.80254,"ALL":108.22904,"AMD":480.41659,"...":"150+ more currencies"}}

				// Parse response body
				auto parsed_body = json::parse(response_body);

				// Iterating over available currencies to get the key with which the parsed json gets accessed.
				for (auto element : app_state.currencies)
				{
					std::string code = element.first;
					float_t rate = parsed_body["data"][code];
					
					currency.exchange_rates[code] = rate;
				}

				// Update the used and reamining quotas since a successful query was made
				app_state.account.quotas.used += 1;
				app_state.account.quotas.remaining -= 1;
				return;
			}
			// Invalid api key. This should trigger a program termination.
		case 401:
			std::cerr << "\n\n\tInvalid API key! Please check your key." << "\n";
			throw new std::runtime_error("Invalid API key!");
		// Endpoint doesn't exist. This should trigger a program termination.
		case 404:
			std::cerr << "\n\n\tLatest exchange rate API endpoint doesn't exist anymore! Please check the api documentation for changes." << "\n";
			throw new std::runtime_error("Latest API endpoint doesn't exist!");
		// Used up all quotas
		case 429:
			std::cerr << "\n\n\tYou have reached your rate limit! Try again next month or upgrade your plan." << "\n";
			throw new std::runtime_error("Rate limit reached!");
		// Some kind of server error. This should trigger a retry or a program termination if enough retrys are reached.
		case 500:
			// No retry here since the status endpoint worked at program initialization.
			// So just terminate the program.
			std::cerr << "\n\n\tServer error! Terminating program. Please try again later." << "\n";
			throw new std::runtime_error("Latest API endpoint not reachable!");
		// Any other unexpected response code. This should trigger a program termination.
		default:
			std::cerr << "\n\n\tUnexpected error!" << "\n";
			throw new std::runtime_error("Unexpected error!");
	}
}

// This function lets the user exchange money from a chosen source currency into an chosen target currency
// It ask for the source currency, the target currency and the amount to exchange
// If the chosen source currencies has no cached exchange rate data then the data will be fetched from the API
void exchange_money(CurrencyConverter::AppState& app_state)
{
	std::string source_currency = "";
	std::string target_currency = "";
	std::string amount = "";
	float_t amount_f = 0.0;
	float_t converted_amount = 0.0;

	// Ask for source currency
	while (true)
	{
		// Rewrite main menu for cleaner output
		write_main_menu(app_state);
		std::cout << "\nWhat do you want to do --> C";
		std::cout << "\n---------- Money exchange ----------\n";
		std::cout << "Please enter the currency code of the source currency. Then the currency code of the target currency and finally the amount.\n";
		std::cout << "If you make an invalid input then your input will be ignored and this window will be refreshed.\n\n";
		std::cout << "Source currency -> ";

		getline(std::cin, source_currency);

		if (app_state.currencies.contains(source_currency))
		{
			break;
		}
	}

	// Check if exchange rate data exist for source currency
	// If not the fetch it
	if (app_state.currencies[source_currency].exchange_rates.empty())
	{
		get_exchange_rates(app_state, app_state.currencies[source_currency]);
	}

	// Ask for target currency
	while (true)
	{
		// Rewrite main menu and previous input for cleaner output
		write_main_menu(app_state);
		std::cout << "\nWhat do you want to do --> C";
		std::cout << "\n---------- Money exchange ----------\n";
		std::cout << "Please enter the currency code of the source currency. Then the currency code of the target currency and finally the amount.\n";
		std::cout << "If you make an invalid input then your input will be ignored and this window will be refreshed.\n\n";
		std::cout << "Source currency -> " << source_currency << '\n';
		std::cout << "Target currency -> ";

		getline(std::cin, target_currency);

		// Check if the target currency exist in currencies
		if (app_state.currencies[source_currency].exchange_rates.contains(target_currency))
		{
			break;
		}
	}

	// Ask for amount
	while (true)
	{
		// Rewrite main menu and previous input for cleaner output
		write_main_menu(app_state);
		std::cout << "\nWhat do you want to do --> C";
		std::cout << "\n---------- Money exchange ----------\n";
		std::cout << "Please enter the currency code of the source currency. Then the currency code of the target currency and finally the amount.\n";
		std::cout << "If you make an invalid input then your input will be ignored and this window will be refreshed.\n\n";
		std::cout << "Source currency -> " << source_currency << '\n';
		std::cout << "Target currency -> " << target_currency << '\n';
		std::cout << "Amount to be exchanged -> ";

		getline(std::cin, amount);

		// Check if a number was entered by casting it to double and checking the result
		// If the result is 0 then the input is invalid
		// A 0 input will be invlaid as well but that's fine because that calculation is pointless
		amount_f = atof(amount.c_str());
		if (amount_f)
		{
			break;
		}
	}

	// Do conversion and output result
	converted_amount = amount_f * app_state.currencies[source_currency].exchange_rates[target_currency];

	std::cout << amount_f;
	std::cout << std::setw(4) << app_state.currencies[source_currency].symbol;
	std::cout << " is " << converted_amount;
	std::cout << std::setw(4) << app_state.currencies[target_currency].symbol;
}

// This function lets the user choose a currency and displays detailed information about it
void write_detailed_currency_information(CurrencyConverter::AppState& app_state)
{
	std::string input = "";
	while (true)
	{
		std::cout << "\n---------- Detailed currency information ----------" << '\n';
		std::cout << "Please type in the code of the currency you wish to know more about -> ";
		getline(std::cin, input);

		if (app_state.currencies.contains(input))
		{
			app_state.currencies[input].print();
			break;
		}

		std::cout << "Unrecognized currency code. Please try again.";
	}
}

// This function write all currencies stored in app_state.currencies to the console
void list_available_currencies(CurrencyConverter::AppState& app_state)
{
	std::cout << "Available currencies:" << '\n';
	for (auto currency : app_state.currencies)
	{
		std::cout << "    " << currency.first << std::endl;
	}
}

// This function write the help menu to the console
void write_help_menu()
{
	// Clear console
	system("cls");
	// Explain option A
	std::cout << "List available currencies -> " << '\n';
	std::cout << "    This option shows you all currencies that are available on this API endpoint." << '\n';
	// Explain option B
	std::cout << "Show detailed information about a currency -> " << '\n';
	std::cout << "    This option shows you detailed information about a chosen currency." << '\n';
	// Explain option C
	std::cout << "Exchange money -> " << '\n';
	std::cout << "    This option lets you convert a chosen amount from your source currency into your target currency." << '\n';
	// Explain option H
	std::cout << "Show help -> " << '\n';
	std::cout << "    This option shows you this help menu." << '\n';
	// Explain option X
	std::cout << "Close the program -> " << '\n';
	std::cout << "    This option closes the program." << '\n';
}

// This function writes the main menu contents to the console
void write_main_menu(CurrencyConverter::AppState& app_state)
{
	// Clear console
	system("cls");
	// Display account data
	std::cout << "\n--------------------------------\n" << app_state.account.to_string() << "--------------------------------\n";
	// Display options to choose
	std::cout << "A -> List available currencies" << '\n';
	std::cout << "B -> Show detailed information about a currency" << '\n';
	std::cout << "C -> Exchange money" << '\n';
	std::cout << "H -> Show help" << '\n';
	std::cout << "X -> Close the program" << std::endl;
}

// This function loads the currency list into the app_state if needed.
void get_currencies(CurrencyConverter::AppState& app_state, bool forced)
{
	// If this function isn't doing a forced update and currencies are already loaded
	// then nothing needs to be done
	if (!forced && !app_state.currencies.empty())
	{
		return;
	}

	// Fetch new data
	// Class that handles memory initialization and deletion.
	curlpp::Cleanup cleaner;

	// Set target url of request to the status endpoint.
	curlpp::Easy request;
	request.setOpt(new Url("https://api.freecurrencyapi.com/v1/currencies"));

	// Add headers. Here only containing the api key.
	std::list<string> headers {};
	headers.push_back("apikey: " + app_state.api_key);
	request.setOpt(new HttpHeader(headers));

	// Memory location to store the incoming response body
	std::stringstream response_body;
	// Set the response stream as output stream of the request
	request.setOpt(cURLpp::Options::WriteStream(&response_body));

	// Send request and get a result.
	request.perform();

	long response_code = curlpp::infos::ResponseCode::get(request);

	// Handling the possible response codes.
	// Error 403 (Not allowed), 422 (Validation Error) can't / shouldn't happen at this endpoint.
	// Error 404 may happen if the url got changed.
	switch (response_code)
	{
		// Happy case. Write received data to AppState and return.
		// Curly braces are needed so that a scope is properly created and memory can be properly initialized and deleted
		case 200:
		{
			// Example response body
			// {"data":{"AED":{"symbol":"AED","name":"United Arab Emirates Dirham","symbol_native":"د.إ","decimal_digits":2,"rounding":0,"code":"AED","name_plural":"UAE dirhams"},"AFN":{"symbol":"Af","name":"Afghan Afghani","symbol_native":"؋","decimal_digits":0,"rounding":0,"code":"AFN","name_plural":"Afghan Afghanis"},"...":{}}}

			// Parse response body
			auto parsed_body = json::parse(response_body);

			// Construct new currencies from parsed_body
			for (auto& element : parsed_body["data"])
			{
				CurrencyConverter::Currency currency = CurrencyConverter::Currency(
					element["symbol"],
					element["name"],
					element["symbol_native"],
					element["decimal_digits"],
					element["rounding"],
					element["code"],
					element["name_plural"]
				);
				std::string code = (string) element["code"];
				app_state.currencies[code] = currency;
			}

			// Update the used and reamining quotas since a successful query was made
			app_state.account.quotas.used += 1;
			app_state.account.quotas.remaining -= 1;
			return;
		}
		// Invalid api key. This should trigger a program termination.
		case 401:
			std::cerr << "\n\n\tInvalid API key! Please check your key." << "\n";
			throw new std::runtime_error("Invalid API key!");
		// Endpoint doesn't exist. This should trigger a program termination.
		case 404:
			std::cerr << "\n\n\tCurrency API endpoint doesn't exist anymore! Please check the api documentation for changes." << "\n";
			throw new std::runtime_error("Currency API endpoint doesn't exist!");
		// Used up all quotas
		case 429:
			std::cerr << "\n\n\tYou have reached your rate limit! Try again next month or upgrade your plan." << "\n";
			throw new std::runtime_error("Rate limit reached!");
		// Some kind of server error. This should trigger a retry or a program termination if enough retrys are reached.
		case 500:
			// No retry here since the status endpoint worked at program initialization.
			// So just terminate the program.
			std::cerr << "\n\n\tServer error! Terminating program. Please try again later." << "\n";
			throw new std::runtime_error("Currency API endpoint not reachable!");
		// Any other unexpected response code. This should trigger a program termination.
		default:
			std::cerr << "\n\n\tUnexpected error!" << "\n";
			throw new std::runtime_error("Unexpected error!");
	}
}

void check_api_status(CurrencyConverter::AppState& app_state)
{
	// Class that handles memory initialization and deletion.
	curlpp::Cleanup cleaner;

	// Set target url of request to the status endpoint.
	curlpp::Easy request;
	request.setOpt(new Url("https://api.freecurrencyapi.com/v1/status"));

	// Add headers. Here only containing the api key.
	std::list<string> headers {};
	headers.push_back("apikey: " + app_state.api_key);

	request.setOpt(new HttpHeader(headers));

	// Memory location to store the incoming response body
	std::stringstream response_body;
	// Set the response stream as output stream of the request
	request.setOpt(cURLpp::Options::WriteStream(&response_body));

	// Counter necessary for retrying if the status endpoint fails with error 500
	// Retry currently after 30 seconds for the case if the problem is intermittent
	uint8_t retry_count {};
	uint8_t max_retry_count { 5 };

	while (true)
	{
		// Send request and get a result.
		// By default the result goes to standard output.
		request.perform();

		long response_code = curlpp::infos::ResponseCode::get(request);
		// std::cout << "\nDEBUG: response code --> " << response_code << "\n\n";

		// Handling the possible response codes.
		// Error 403 (Not allowed), 422 (Validation Error) and 429 (Rate Limit hit) can't / shouldn't happen at this endpoint.
		// Error 404 may happen if the url got changed.
		switch (response_code)
		{
			// Happy case. Write received data to AppState and return.
			case 200:
			{
				// Example response body for freecurrencyapi.com
				// {"account_id":239344465066725376,"quotas":{"month":{"total":5000,"used":0,"remaining":5000}}}
				// Example response body for currencyapi.com
				// {"account_id":239344465066725376,"quotas":{"month":{"total":5000,"used":0,"remaining":5000},"grace":{"total":0,"used":0,"remaining":0}}}

				// Parse response body
				auto parsed_body = json::parse(response_body);
				// Fill account info with response data
				for (auto& element : parsed_body)
				{
					// Extract data from json object to cast it readably
					uint32_t account_id = (uint32_t)parsed_body["account_id"];
					uint32_t total = (uint32_t)parsed_body["quotas"]["month"]["total"];
					uint32_t used = (uint32_t)parsed_body["quotas"]["month"]["used"];
					uint32_t remaining = (uint32_t)parsed_body["quotas"]["month"]["remaining"];

					// Create account object without grace data since this function is fetching from freecurrencyapi.com
					// which doesn't have grace options.
					CurrencyConverter::Account account = CurrencyConverter::Account(
						std::to_string(account_id),
						total,
						used,
						remaining,
						0, 0, 0
					);

					// Store account in th AppState so that it can be accessed outside
					app_state.account = account;
				}
				return;
			}
			// Invalid api key. This should trigger a program termination.
			case 401:
				std::cerr << "\n\n\tInvalid API key! Please check your key." << "\n";
				throw new std::runtime_error("Invalid API key!");
			// Endpoint doesn't exist. This should trigger a program termination.
			case 404:
				std::cerr << "\n\n\tStatus API endpoint doesn't exist anymore! Please check the api documentation for changes." << "\n";
				throw new std::runtime_error("Status API endpoint doesn't exist!");
			// Some kind of server error. This should trigger a retry or a program termination if enough retrys are reached.
			case 500:
				// Check if retry or termination.
				if (retry_count < max_retry_count)
				{
					// Retry
					std::cerr << "\n\n\tStatus API endpoint not reachable or other server error. Automatic retry in 30 seconds." << "\n";
					// Wait 30 seconds before restarting the loop.
					// Sleep takes milliseconds as argument so seconds time 1000.
					Sleep(30 * 1000);
					continue;
				} else
				{
					// Program termination
					std::cerr << "\n\n\tMaximum retries reached. Terminating program. Please try again later." << "\n";
					throw new std::runtime_error("Status API endpoint not reachable!");
				}
			// Any other unexpected response code. This should trigger a program termination.
			default:
				std::cerr << "\n\n\tUnexpected error!" << "\n";
				throw new std::runtime_error("Unexpected error!");
		}
	}
}

void test_http_requests()
{
	try
	{
		curlpp::initialize();

		curlpp::Easy myRequest;
		myRequest.setOpt<curlpp::options::Url>("https://jsonplaceholder.typicode.com/posts");

		// Send request and get a result.
		// By default the result goes to standard output.
		myRequest.perform();
	}

	catch (curlpp::RuntimeError& e)
	{
		std::cout << e.what() << std::endl;
	}

	catch (curlpp::LogicError& e)
	{
		std::cout << e.what() << std::endl;
	}

	curlpp::terminate();
}

void test_json_parsing() {
	// Test string as it can come from the api
	std::string string_to_parse = "{\"data\":{\"AUD\":1.5345401805,\"BGN\":1.8221902017,\"BRL\":4.8928708594,\"CAD\":1.3660502242,\"CHF\":0.8990201329,\"CNY\":7.2979409447,\"CZK\":22.701373061400003,\"DKK\":6.9510907368,\"EUR\":0.9317201208,\"GBP\":0.8080701125,\"HKD\":7.8221108037,\"HRK\":7.0425408739,\"HUF\":353.8908982314,\"IDR\":15704.153081381,\"ILS\":3.9255304843,\"INR\":83.0822989945,\"ISK\":139.0264570119,\"JPY\":149.4456975115,\"KRW\":1306.3790579203,\"MXN\":17.4655028338,\"MYR\":4.7255407304,\"NOK\":11.0195415238,\"NZD\":1.6677903169,\"PHP\":56.0604368321,\"PLN\":4.1444707599,\"RON\":4.630280471,\"RUB\":91.7027771534,\"SEK\":10.8775617582,\"SGD\":1.3535102301,\"THB\":35.5178959579,\"TRY\":28.3943738694,\"USD\":1,\"ZAR\":18.2485130574}}";
	// Test string parsed to json
	auto parsed = json::parse(string_to_parse);

	// Write string_to_parse to file
	//auto path = std::filesystem::current_path();
	//std::string outfile = path.generic_string() + "/output.txt";
	//std::ofstream out(outfile);
	//out << string_to_parse;
	//out.close();

	std::cout << "\n\n----- print string_to_parse\n\n";
	std::cout << std::setw(4) << string_to_parse;

	std::cout << "\n\n----- print parsed\n\n";
	std::cout << std::setw(4) << parsed;

	std::cout << "\n\n----- print parsed[\"data\"]\n\n";
	std::cout << std::setw(4) << parsed["data"];

	std::string code = "AUD";

	std::cout << "\n\n----- print parsed[\"data\"][\"AUD\"]\n\n";
	std::cout << std::setw(4) << parsed["data"][code];

	std::cout << "\n\n----- print test1 and test2\n\n";
	float_t test1 = parsed["data"][code];
	double_t test2 = parsed["data"][code];
	std::cout << test1 << '\n';
	std::cout << test2;

	std::cout << "\n\n----- print element in parsed\n\n";
	for (auto& element : parsed) {
		std::cout << element << '\n';
	}

	std::cout << "\n\n----- print element in parsed[\"data\"]\n\n";
	for (auto& element : parsed["data"])
	{

	}
	/*for (auto& element : parsed["data"]) {
		printf("% u.Country\n", i);
		std::cout << "   symbol -> " << element["symbol"] << '\n';
		std::cout << "   name -> " << element["name"] << '\n';
		std::cout << "   symbol_native -> " << element["symbol_native"] << '\n';
		std::cout << "   decimal_digits -> " << element["decimal_digits"] << '\n';
		std::cout << "   rounding -> " << element["rounding"] << '\n';
		std::cout << "   code -> " << element["code"] << '\n';
		std::cout << "   name_plural -> " << element["name_plural"] << '\n';

		i++;
	}*/

	// std::cout << "\n\n----- print constructed object\n\n";
	/*std::vector<CurrencyConverter::Currency*> currencies;
	i = 1;
	for (auto& element : parsed["data"]) {
		printf("% u.Country\n", i);
		std::cout << "code -> " << element["code"] << '\n';
		CurrencyConverter::Currency* currency = new CurrencyConverter::Currency(
			element["symbol"],
			element["name"],
			element["symbol_native"],
			element["decimal_digits"],
			element["rounding"],
			element["code"],
			element["name_plural"]
		);

		currencies.push_back(currency);

		i++;
	}
	std::cout << '\n';
	for (auto& element : currencies) {
		element->print();
		std::cout << '\n';
	}*/

	std::cout << "\n\n----- testing json finished\n\n";
}