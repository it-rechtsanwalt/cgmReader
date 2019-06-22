/*============================================================================
 *  Name       : PumpReader.h
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


#ifndef PUMPREADER_H_
#define PUMPREADER_H_

#include "../utils/loguru.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>
#include "UsbDevice.h"
#include "USBDriver.h"
#include "PumpDriver.h"
#include <string>
#include <future>
#include <usb.h>
#include <libusb-1.0/libusb.h>
#include "../MainParameters.h"

class PumpReader {
public:
	PumpReader();
	virtual ~PumpReader();

	void powerCircle(void);

	// The steps

	int readingInitializeStick(MainParameters & mParams);
	int readingFindPump();
	int readingStartDownload(PumpStatus * ps, HistoryData * hD);
	int readingCloseConnection();
	PumpStatus pumpStatus;


private:
	PumpDriver pumpDriver;
	UsbDevice usbDevice;


	libusb_hotplug_callback_handle handle;
	int initialize(PumpDriver * pumpDriver);


};



#endif /* PUMPREADER_H_ */
