/*============================================================================
 *  Name       : MainParameters.h
 *  Project    : cgmReader
 *  Author     : compuholic - itra.at
 *  created    : Jun 22, 2019
 *
 * Copyright 2019 - compuholic - itra.at
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice, the following disclaimer and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *  Description : TBD
 * ============================================================================
 */



#ifndef MAINPARAMETERS_H_
#define MAINPARAMETERS_H_


#include <string>
#include "utils/loguru.hpp"


/*
 * Class for calling parameters
 */
class MainParameters {
public:
	MainParameters();
	virtual ~MainParameters();

	// our loggin level
	int logLevel = loguru::Verbosity_INFO;



	long pollStickTime = 30;  // Seconds  - polling for the device plugged in
	long pollPumpTime = 90;  // Seconds  - polling for the pump


	/*
	 * Server Settings
	 */
	bool JsonServerEnable = true; // start json server
	int  JsonServerPort = 8888; // json server port
	std::string JsonServerKey = "1234"; // Json server key

	/*
	 * try powercycling if stick fails? 0 = disabled, 1 = enabled
	 */
	int powerCycling = 1;
	int noStickResetCount = 2;

	/*
	 * vid and pid of the stick
	 */
	unsigned short vid = 0x1a79;
	unsigned short pid = 0x6210;






	// static texts for status
	const char *statustexts[13] = { "Ready"							// 0
			, "Searching for Stick..."								// 1
			, "No stick found... Sleeping for %d seconds..." 	 	// 2
			, "Stick found. Initializing..."						// 3
			, "Stick in unknown state. Resetting..."  				// 4
			, "Stick initialized. Searching for pump..."			// 5
			, "Pump not found. Sleeping for %d seconds..."			// 6
			, "Pump negotiation successful. Downloading Data..."    // 7
			, "Error downloading data. Retrying"					// 8
			, "Error downloading. Aborting"							// 9
			, "Download complete. Closing connection..."		 	// 10
			, "Read ok. Update in %d seconds..."					// 11
			, "Powercycling - just to be sure. Please wait..."		// 12
			};

};

#endif /* MAINPARAMETERS_H_ */
