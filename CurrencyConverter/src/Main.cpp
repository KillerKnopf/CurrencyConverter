#include "Main.h"

int main(int argc, char* argv[]) {
	// Steps needed to be taken
	// API documentation available here: https://freecurrencyapi.com/docs/
	/*
		1. Check if api key exists
			If api key does not exist promt for it and store it in .env file as API_KEY
		2. Send GET request to https://api.freecurrencyapi.com/v1/status with api key in header
		3. Handle response (list of possible responses https://freecurrencyapi.com/docs/status-codes)
			3.1 Handle errors - print useful error message
			3.2 If correct move forward to main menu

		Main Menu
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
						Then check if datetime of filename are new enough
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
			4. Settings
				Set API_KEY
				Set how old the latest exchange rate can be

				Changes made here are stored in .env
	*/

	// ---------- Program Initialization ----------

	// Declare all variables, data structures and objects needed for the program

	// Map storing available currencies (key: currency->code, value full object)
	// Map storing the exchange rates (value: map of floats) of a base currency (key: currency->code)
	// Variable for api key
	// Variable for max age of latest data

	// Make sure the api key is available
	// It should be stored in the .env file in the same folder as the compiled executable binary (.exe file)
	// If .env file doesn't exist create a default .env file
	// If api key isn't in .env file ask the user for it
	// Finally load api key into process memory

	return 0;
}
