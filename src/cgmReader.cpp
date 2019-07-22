/*============================================================================
 *  Name       : cgmReader.cpp
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

#include <iostream>
#include "utils/loguru.hpp"
#include "pumpdriver/PumpReader.h"
#include "jsonServer/JsonServer.h"
#include "jsonServer/statusData.h"
#include <future>

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "MainParameters.h"
#include <mutex>

using namespace std;

//////////////// STATICS ////////////////////
const int VERSION_MAJOR = 0;
const int VERSION_MINOR = 2;
int disp = 1;
int logLevel = loguru::Verbosity_INFO;
unsigned short vid = 0x1a79;
unsigned short pid = 0x6210;

////////////////////////////////////////////////

MainParameters mParams;
PumpStatus ps { };
PumpReader pumpReader;
UsbDevice readData;
HistoryData hD;
StatusData statusData;
JsonServer jsonServer;

std::string readStatus; 		// our info output string
time_t now = time(NULL); // actual time

int readStick(void);
int jsonThread(void);
std::mutex readStick_mutex;

int read_status = 0;

/**
 * ProgramStatus:
 * 0: idle - no stick
 * 10: idle
 * 15: read in progress
 * 30: Stick not found
 * 31: stick in unknown state
 * 32: pump not found
 * 33: read error
 * 40: all ok, data read.
 *
 */
int programStatus = 0;
int noStickFoundCounter = 0;

int processArgs(int argc, char *argv[]);
bool timeToCheck();

time_t pollForStick = time(NULL);
time_t pollForPump = time(NULL);

int main(int argc, char *argv[]) {
	// process arguments
	programStatus = processArgs(argc, argv);
	// Start the logging facility
	loguru::init(argc, argv);
	loguru::g_stderr_verbosity = logLevel;

	// Say Hello
	LOG_F(INFO, "640g Reader. Version: %d.%d", VERSION_MAJOR, VERSION_MINOR);
	LOG_F(INFO, "Press <ctrl>-c for exit");

	//create "the" reader Thread
	std::thread readerThread(readStick);
	readStatus = mParams.statustexts[0];

	//create "the" reader Thread
	std::thread readerJsonThread(jsonThread);

	/*Main loop (non-blocking):
	 * 1. if status is < 10: stick not initialized
	 * 2. if status is 10: idle, checking if sleep time is over
	 * 3. if status is 20: read in progress.
	 * 4. if status is 9: stick not found... sleep
	 * 5. if status is 8: pump not found... sleep
	 * 6. if status is 7: read ok, waiting for next read... sleep
	 */
	while (1) {
		if (programStatus < 10) {  // idle
			// check if it is time to read values
			if (timeToCheck()) {
				readStatus = mParams.statustexts[1];
				LOG_F(INFO, mParams.statustexts[1]);
				programStatus = 10;
			}
		}
		switch (programStatus) {
		case 7: {	// read ok. output status text with remaining time
			char buf[sizeof(mParams.statustexts[11]) * 2 + 20]; // FIXME: THIS IS QUICK AN DIRTY!
			sprintf(buf, mParams.statustexts[11], pollForPump - time(NULL));
			readStatus = buf;

			break;
		}
		case 8: {	// no pump found, output status text with remaining time
			char buf[sizeof(mParams.statustexts[6]) * 2 + 20]; // FIXME: THIS IS QUICK AN DIRTY!
			sprintf(buf, mParams.statustexts[6], pollForPump - time(NULL));
			readStatus = buf;
			break;
		}
		case 9: { 	// no stick found, output status text with remaining time
			char buf[sizeof(mParams.statustexts[2]) * 2 + 20]; // FIXME: THIS IS QUICK AN DIRTY!
			sprintf(buf, mParams.statustexts[2], pollForPump - time(NULL));
			readStatus = buf;
			break;
		}
		case 30: { // stick not found (returned from reader thread) reset timer and reset text
			LOG_F(ERROR, "Error Stick not found. Sleeping for %d seconds...",
					mParams.pollStickTime);
			noStickFoundCounter++;
			if (noStickFoundCounter > mParams.noStickResetCount) {
				noStickFoundCounter = 0;
				programStatus = 31;
				break;
			}
			programStatus = 9;  // stick not found status
			pollForPump = time(NULL) + mParams.pollStickTime;
			break;
		}
		case 31: { // Stick in unknown state
			LOG_F(INFO, "Powercycling - just to be sure...");
			// FIXME: this is quick and dirty. for the raspberry pi (ALL BUT ZERO and ZERO/W) we do a powercycling for all usb ports.
			// FIXME: do a powercycling just for the port where the reader is connected...

			std::system("./uhubctl -a 2");
			sleep(15);
			programStatus = 9;

			break;
		}
		case 32: {	// pump not found... trying again later...
			LOG_F(INFO, "Pump not near. Sleeping for %d seconds...",
					mParams.pollPumpTime);
			pollForPump = time(NULL) + mParams.pollPumpTime;
			programStatus = 8;		// Pump not found

			break;
		}

		case 33: { // read error... try again?
			// FIXME retry management
			programStatus = 9;  // stick not found status
			pollForPump = time(NULL) + mParams.pollStickTime;
			break;
		}

		case 40: {  // everything fine!
			LOG_F(INFO, "Read ok. Sleeping for %d seconds...",
					mParams.pollPumpTime);
			programStatus = 7;
//			if (disp == 1) {

			// TODO process events
//				int pos = 0;
//				std::vector<int> vals;
//				if (hD.events.size()) {
//					for (unsigned int i = 0; i < hD.events.size(); i++) {
//						time_t showperiod = time(0) - (24 * 3600); // we want 24h
//						if (showperiod - hD.events.at(i).eventTime < 0) {
//							vals.push_back(hD.events.at(i).sg);
//							pos++;
//					}
//					}
//					ps.valCount = vals.size();
//					ps.bglVals = vals.data();
//
//				}
//
//				fill_values(readData.getPumpStatus(), readStatus.c_str());
//			}
			pollForPump = time(NULL) + mParams.pollPumpTime;

			break;
		}

		default:
			break;
		}
		// we only show recent values. so: if last successful is more than 10 minutes ago: reset the provided data:

		usleep(10 * 1000); /*Just to let the system breath*/

	}
	// FIXME: cleanup

	// wait for readerthread to finish...
	readerThread.join();
	if (mParams.JsonServerEnable) {
		readerJsonThread.join();
	}
	return 0;

}

int processArgs(int argc, char *argv[]) {
	// TODO: process arguments!
	return 0;
}

bool timeToCheck() {

	if (pollForPump - time(NULL) > 0) {
		return false;
	};
	return true;
}

// the read process
/*
 * return values:
 * 30: Stick not found
 * 31: stick in unknown state
 * 32: pump not found
 * 33: read error
 * 40: all ok, data read.
 *
 */
int reading(void) {
	readStick_mutex.lock();
	int result = pumpReader.readingInitializeStick(mParams);
	if (result == -1) {
		// stick not found
		readStick_mutex.unlock();
		return 30;
	}
	readStick_mutex.unlock();
	if (result == -99) {
		// stick in unknown state;
		readStick_mutex.lock();
		readStatus = mParams.statustexts[4];
		readStick_mutex.unlock();
		return 31;
	}
	readStick_mutex.lock();
	readStatus = mParams.statustexts[5];
	readStick_mutex.unlock();
	result = pumpReader.readingFindPump();
	if (result == -2) {
		// pump not found
		readStick_mutex.unlock();
		return 32;
	}
	if (result == -3) {
		// read error!
		readStick_mutex.unlock();
		return 33;
	}

	readStick_mutex.lock();
	readStatus = mParams.statustexts[7];
	result = pumpReader.readingStartDownload(&ps, &hD);
	LOG_F(8, "HD EVENT SIZE %d", hD.events.size());
	readStick_mutex.unlock();
	if (result == -4) {
		// read error retry?
		readStick_mutex.lock();
		readStatus = mParams.statustexts[8];
		readStick_mutex.unlock();
		return 33;
	}
	readStick_mutex.lock();
	readStatus = mParams.statustexts[10];
	readStick_mutex.unlock();
	result = pumpReader.readingCloseConnection();
	readStick_mutex.lock();
	readStatus = mParams.statustexts[0];
	readStick_mutex.unlock();
	return 40;
}

int readStick(void) {

	while (1) {
		sleep(1);
		if (programStatus == 10) {
			programStatus = 15;
			int result = reading();
			readStick_mutex.lock();
			programStatus = result;
			statusData.refresh(ps, result);
			readStick_mutex.unlock();
		}

	}
}

int jsonThread(void) {
	if (!mParams.JsonServerEnable) {
		return 0;
	} else {
		LOG_F(INFO, "Starting Server at port %d", mParams.JsonServerPort);
		statusData.initData();
		jsonServer.startServer(mParams.JsonServerKey, mParams.JsonServerPort,
				&statusData);
		return 0;
	}

}

