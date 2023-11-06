#pragma once
#include <string>
#include <map>
#include <iostream>
#include <windows.h>
#include <iomanip>

namespace CurrencyConverter
{
	using std::string;

	class Currency {
	public:
		Currency();
		Currency(string symbol, string name, string symbol_native, uint8_t decimal_digits, uint8_t rounding, string code, string name_plural);
		~Currency();

		void print();

		string symbol;
		string name;
		string symbol_native;
		uint8_t decimal_digits;
		uint8_t rounding;
		string code;
		string name_plural;

		std::map<string, float> exchange_rates;
	};
}