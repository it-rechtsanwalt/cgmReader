/*============================================================================
 *  Name       : PumpReader.cpp
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



#include "PumpReader.h"


PumpReader::PumpReader() {

	// TODO Auto-generated constructor stub

}

PumpReader::~PumpReader() {

	// TODO Auto-generated destructor stub
}




void PumpReader::powerCircle(void) {

	/*FIXME check: is the stick stuck? if yes:
	 * not yet working. an i have no idea why....
	 * 1. get correct Stick id
	 * 2. execute:
	 * echo 0 > /sys/bus/usb/devices/<STICK-ID>/authorized
	 * sleep 100
	 * echo 1 > /sys/bus/usb/devices/<STICK-ID>/authorized
	 */
	LOG_F(INFO, "Power circle (NOT YET IMPLEMENTED)");

}

int PumpReader::readingInitializeStick(MainParameters & mParams) {
	pumpDriver.usbDevice = &usbDevice;
	int res = pumpDriver.usbDriver.initialize(mParams.vid, mParams.pid, usbDevice);
	if (res != 0) {	// stick not found

		pumpDriver.closeDriver();
		return -1;
	}
	LOG_F(INFO, "Stick found");
	res = pumpDriver.getStickData();
	if (res == -1) {
		LOG_F(ERROR, "Stick found but in unknown state.");
		return -99;
	}



	return 0;
}

int PumpReader::readingFindPump() {
	pumpDriver.init();
	pumpDriver.openPumpConnection();
	pumpDriver.enterPassThroughMode();
	pumpDriver.readInfo();
	int status =  pumpDriver.channelNegotiation();
	if (status == 0) {

		pumpDriver.exitPassthroughMode();
		pumpDriver.exitControlMode();
		pumpDriver.closeDriver();
		return -2;
	}
	pumpDriver.enteringEHSM();
	pumpDriver.getPumpTime();
	return 0;

}


int PumpReader::readingStartDownload(PumpStatus * ps, HistoryData * hD) {
return pumpDriver.downloadData(ps);
	/*FIXME if we want to get the history data: you need this :
	if (pumpDriver.downloadData(ps)) {
		LOG_F(ERROR, "Download error. retrying...");
		return -4;
	}
return pumpDriver.downloadHistory(ps, hD);
*/
}


int PumpReader::readingCloseConnection() {
	//FIXME: check state and close correct
	pumpDriver.closeEHSM();
	pumpDriver.closeConnection();
	pumpDriver.exitPassthroughMode();
	pumpDriver.exitControlMode();
	pumpDriver.closeDriver();

	return 0;
}


