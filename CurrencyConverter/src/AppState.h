#pragma once
#include <string>

#include "Currency.h"
#include "Account.h"

namespace CurrencyConverter
{
	using std::string;

	class AppState {
	public:
		AppState();
		~AppState();

		// Used for API access, parse from command line arguments.
		string api_key;

		// Map was choosen over unordered_map because I want the currencies to be ordered alphabetically
		// and any map in this program will never have more than 32 entries.
		// That means the difference in time complexity (O(log n) vs O(1)) isn't a significant difference for this program.
		// Even if I switch to Currencyapi.com as a data source these maps won't have more than 172 entries.

		// Map storing available currencies (key: currency->code, Currency instance).
		std::map<string, Currency> currencies;

		// To store account data.
		Account account;
	};
}
