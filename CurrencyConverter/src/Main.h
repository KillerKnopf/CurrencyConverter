#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <windows.h>
#include <libloaderapi.h>

// Library for json parsing
#include <nlohmann/json.hpp>
// Library for making https requests
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Form.hpp>
#include <curlpp/Info.hpp>
#include <curlpp/Option.hpp>
#include <curlpp/Multi.hpp>
#include <curlpp/OptionBase.hpp>

#include "Currency.h"
#include "AppState.h"


using std::map;
using std::string;
using std::cout;

using json = nlohmann::json;

using CurrencyConverter::Currency;

void get_exchange_rates(CurrencyConverter::AppState& app_state, CurrencyConverter::Currency& currency);
void exchange_money(CurrencyConverter::AppState& app_state);
void write_detailed_currency_information(CurrencyConverter::AppState& app_state);
void list_available_currencies(CurrencyConverter::AppState& app_state);
void write_help_menu();
void write_main_menu(CurrencyConverter::AppState& app_state);
void get_currencies(CurrencyConverter::AppState& app_state, bool forced = false);
void check_api_status(CurrencyConverter::AppState& app_state);

// Some function that i used to learn how the external libraries get used
void test_http_requests();
void test_json_parsing();