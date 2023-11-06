# CurrencyConverter

## Description

A cli tool for converting currencies.
Fetches currency data and exchange rates from freecurrencyapi.com.

Because i haven't used cmake or similar build tools:  
This project runs on WINDOWS and VISUAL STUDIO only!

## Usage

1. Create a free account at https://freecurrencyapi.com/
2. Grab the provided api key
3. Git clone the project
4. Open the solution file
5. Install 'nlohmann.json' through nuget package manager
6. Install curl and curlpp through vcpkg
7. Make sure that Visual Studio is using vcpkg so that it can include curlpp
	1. Under porjec properties > vcpkg set stuff to yes
8. Build the project through Visual Studio
9. Open a console in the ouput directory (bin/.. in solution directory)
10. Start the program using: .\CurrencyConverter\ <your API key>
	1. To pass the API key into the program when launching it from Visual Studio add the API key to 'Project Properties' > "Debugging" > 'Command Arguments'