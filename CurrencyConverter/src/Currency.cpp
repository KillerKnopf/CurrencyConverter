#include "Currency.h"

namespace CurrencyConverter
{
	Currency::Currency()
	{
		this->symbol = "";
		this->name = "";
		this->symbol_native = "";
		this->decimal_digits = 0;
		this->rounding = 0;
		this->code = "";
		this->name_plural = "";
	}
	Currency::Currency(string symbol, string name, string symbol_native, uint8_t decimal_digits, uint8_t rounding, string code, string name_plural) {
		this->symbol = symbol;
		this->name = name;
		this->symbol_native = symbol_native;
		this->decimal_digits = decimal_digits;
		this->rounding = rounding;
		this->code = code;
		this->name_plural = name_plural;
	}

	Currency::~Currency() {

	}

	void Currency::print() {
		printf("Country: %s\n", this->code.c_str());
		printf("    symbol: %s\n    name: %s\n    symbol_native: %s\n    decimal_digits: %u\n    rounding: %u\n    code: %s\n    name_plural: %s\n",
			   this->symbol.c_str(),
			   this->name.c_str(),
			   this->symbol_native.c_str(),
			   this->decimal_digits,
			   this->rounding,
			   this->code.c_str(),
			   this->name_plural.c_str());
		std::cout << "    Exchange rates:" << std::endl;
		for (auto entry : this->exchange_rates)
		{
			std::cout << std::setw(4) << "    -> " << entry.first << ": " << entry.second << std::endl;
		}
	}
}
