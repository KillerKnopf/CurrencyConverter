#pragma once
#include <string>

using std::string;

namespace CurrencyConverter
{
	// Quota is the number of requests can be made each month
	class Quotas {
	public:
		Quotas();
		Quotas(uint32_t total, uint32_t used, uint32_t remaining);
		~Quotas();

		string to_string();

		uint32_t total;
		uint32_t used;
		uint32_t remaining;
	};

	// Grace are the available requests each month above the Quota limit
	// Not existing on freecurrencyapi.com, but in here because I may add the ability to fetch from currencyapi.com
	class Grace {
	public:
		Grace();
		Grace(uint32_t total, uint32_t used, uint32_t remaining);
		~Grace();

		string to_string();

		uint32_t total;
		uint32_t used;
		uint32_t remaining;
	};

	class Account {
	public:
		Account();
		Account(string account_id, uint32_t quotas_total, uint32_t quotas_used, uint32_t quotas_remaining, uint32_t grace_total, uint32_t grace_used, uint32_t grace_remaining);
		~Account();

		string to_string();

		string account_id;
		// Storing data about api request counts
		Quotas quotas;
		// Storing data about overdrafted api request counts
		Grace grace;
	};
}
