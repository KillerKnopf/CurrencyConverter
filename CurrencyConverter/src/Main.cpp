#include "Main.h"


using namespace curlpp::options;


int main(int argc, char* argv[])
{
	try
	{
		// Function to get the window console to display unicode characters
		// In this case used to write currency symbols to the console
		SetConsoleOutputCP(CP_UTF8);

		// Steps needed to be taken
		// API documentation available here: https://freecurrencyapi.com/docs/
		/*
			1. Parse command line arguments
				1.1 get api key
			2. Send GET request to https://api.freecurrencyapi.com/v1/status with api key in header
			3. Handle response (list of possible responses https://freecurrencyapi.com/docs/status-codes)
				2.1 Handle errors - print useful error message
				2.2 If correct move forward to menu

			4. Menu
			- Display api information (returned in success response above)
			- Display list of options to choose:
				1. List available currencies
					Check if list of currencies has already been fetched
						First check process memory
							If data exists and is new enough return
						Then check for file
							If file exist check if it isn't too old
								If data is new enough load into process memory
								Return
					If no valid data was found then fetch new data from https://api.freecurrencyapi.com/v1/currencies
						Store fetched data in process memory and file
						Return
					Display data from process memory
				2. Get exchange rates using latest rate
					>> Ask for base currency
					Check if base currency data has already been fetched and the stored data is new enough
						Check process memory
							Check if key in unordered map exists
							Check if data is new enough
							If valid data found return
						Check file (filename: $(BaseCurrency)_YYYY-MM-DDTHH:MM:SSZ)
							Check if file exists
							Then check if datetime of filename is new enough
							If valid data found load into process memory
							Return
						If no valid data was found in memory and file then fetch new data from https://api.freecurrencyapi.com/v1/latest
							Store fetched data in process memory and file using current timestamp
							Return
					>> Ask for all target currencies to be exchanged into
					>> Ask for the amount of base currency that should be exchanged
					Display table with row for each target currency
						Columns:
							Name of target currency
							Exchange rate to target currency (read from process memory)
							Exchanged amount (calculate by multiplying value to be exchanged with exchange rate)
				3. Get exchange rates from historical rates
					>> Ask for base currency
					>> Ask for date from which onwards data should be fetched (YYYY-MM-DD)
					Check if base currency data has already been fetched and the stored data is old enough
						Check file (filename: Historical\$(BaseCurrency)_YYYY-MM-DD)
							Check if file exists
							Then check if datetime of filename are old enough (file date before input date)
								If no valid data found then fetch new data from https://api.freecurrencyapi.com/v1/historical
								Store fetched data
					Tell user path to file (filename: Historical\$(BaseCurrency)_YYYY-MM-DD)
		*/

		// ---------- Program Initialization ----------

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
		string api_key = argv[1];
		cout << "\n\nargv[1] -> " << argv[1] << '\n';

		// Map was choosen over unordered_map because I want the currencies to be ordered alphabetically
		// and any map in this program will never have more than 32 entries.
		// That means the difference in time complexity (O(log n) vs O(1)) isn't a significant difference for this program.
		// Even if I switch to Currencyapi.com as a data source these maps won't have more than 172 entries.

		// Map storing available currencies (key: currency->code, Currency object)
		map<string, Currency*> currencies;


		check_api_status(api_key);




		// Testing stuff
		//test_json_parsing();
		// test_http_requests();
	}
	catch (std::exception e)
	{

	}
	std::cout << "\n\n";
	return 0;
}

void check_api_status(string api_key)
{
	curlpp::initialize();

	// Set target url of request to the status endpoint.
	curlpp::Easy request;
	request.setOpt(new Url("https://api.freecurrencyapi.com/v1/status"));

	// Add headers. Here only containing the api key.
	std::list<string> headers {};
	headers.push_back("apikey: " + api_key);

	request.setOpt(new HttpHeader(headers));

	uint8_t retry_count {};
	uint8_t max_retry_count { 5 };

	while (true)
	{
		// Send request and get a result.
		// By default the result goes to standard output.
		request.perform();

		long response_code = curlpp::infos::ResponseCode::get(request);
		std::cout << "\nresponse code --> " << response_code << "\n\n";

		// Handling the possible response codes.
		// Error 403 (Not allowed), 422 (Validation Error) and 429 (Rate Limit hit) can't / shouldn't happen at this endpoint.
		// Error 404 may happen if the url got changed.
		switch (response_code)
		{
			// Happy case. Parse data and return true.
			case 200:
				curlpp::terminate();
				return;
			// Invalid api key. This should trigger a program termination.
			case 401:
				std::cerr << "\n\n\tInvalid API key! Please check your key." << "\n";
				throw new std::runtime_error("Invalid API key!");
				break;
			// Endpoint doesn't exist. This should trigger a program termination.
			case 404:
				std::cerr << "\n\n\tStatus API endpoint doesn't exist anymore! Please check the api documentation for changes." << "\n";
				throw new std::runtime_error("Status API endpoint doesn't exist!");
				break;
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
				} else
				{
					// Program termination
					std::cerr << "\n\n\tMaximum retries reached. Terminating program. Please try again later" << "\n";
					throw new std::runtime_error("Status API endpoint not reachable!");
				}
				break;
			// Any other unexpected response code. This should trigger a program termination.
			default:
				std::cerr << "\n\n\tUnexpected error!" << "\n";
				throw new std::runtime_error("Unexpected error!");
				break;
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
	std::string string_to_parse = "{\"data\":{\"AED\":{\"symbol\":\"AED\",\"name\":\"United Arab Emirates Dirham\",\"symbol_native\":\"د.إ\",\"decimal_digits\":2,\"rounding\":0,\"code\":\"AED\",\"name_plural\":\"UAE dirhams\"},\"AFN\":{\"symbol\":\"Af\",\"name\":\"Afghan Afghani\",\"symbol_native\":\"؋\",\"decimal_digits\":0,\"rounding\":0,\"code\":\"AFN\",\"name_plural\":\"Afghan Afghanis\"}}}";
	// Test string parsed to json
	auto parsed = json::parse(string_to_parse);

	// Write string_to_parse to file
	auto path = std::filesystem::current_path();
	std::string outfile = path.generic_string() + "/output.txt";
	std::ofstream out(outfile);
	out << string_to_parse;
	out.close();

	std::cout << "\n\n----- print string_to_parse\n\n";
	std::cout << std::setw(4) << string_to_parse;

	std::cout << "\n\n----- print constructed object\n\n";
	std::cout << std::setw(4) << parsed;

	std::cout << "\n\n----- print constructed object\n\n";
	std::cout << std::setw(4) << parsed["data"];

	std::cout << "\n\n----- print constructed object\n\n";
	std::cout << std::setw(4) << parsed["data"]["AED"];

	std::cout << "\n\n----- print constructed object\n\n";
	for (auto& element : parsed) {
		std::cout << element << '\n';
	}

	std::cout << "\n\n----- print constructed object\n\n";
	int i = 1;
	for (auto& element : parsed["data"]) {
		printf("% u.Country\n", i);
		std::cout << "   symbol -> " << element["symbol"] << '\n';
		std::cout << "   name -> " << element["name"] << '\n';
		std::cout << "   symbol_native -> " << element["symbol_native"] << '\n';
		std::cout << "   decimal_digits -> " << element["decimal_digits"] << '\n';
		std::cout << "   rounding -> " << element["rounding"] << '\n';
		std::cout << "   code -> " << element["code"] << '\n';
		std::cout << "   name_plural -> " << element["name_plural"] << '\n';

		i++;
	}

	std::cout << "\n\n----- print constructed object\n\n";
	std::vector<CurrencyConverter::Currency*> currencies;
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
		element->debug_print();
		std::cout << '\n';
	}

	std::cout << "\n\n----- testing json finished\n\n";
}