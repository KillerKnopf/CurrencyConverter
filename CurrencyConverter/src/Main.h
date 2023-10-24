#pragma once

#include <iostream>
#include <fstream>
#include <string>
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


using std::map;
using std::string;
using std::cout;

using json = nlohmann::json;

using CurrencyConverter::Currency;

bool check_api_status(string api_key);

// Some function were i tested how external libraries get used
void test_http_requests();
void test_json_parsing();