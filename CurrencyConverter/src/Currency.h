#pragma once
#include <string>
#include <map>
#include <iostream>
#include <windows.h>

namespace CurrencyConverter
{
	using std::string;

	class Currency {
	public:
		Currency(string symbol, string name, string symbol_native, uint8_t decimal_digits, uint8_t rounding, string code, string name_plural);
		~Currency();

		void debug_print();

		string symbol;
		string name;
		string symbol_native;
		uint8_t decimal_digits;
		uint8_t rounding;
		string code;
		string name_plural;

		std::map<string, float> exhange_rates;
	};
}