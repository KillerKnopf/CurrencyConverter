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

#include "Currency.h"


using std::map;
using std::string;
using std::cout;

using json = nlohmann::json;

using CurrencyConverter::Currency;


void test_json_parsing();