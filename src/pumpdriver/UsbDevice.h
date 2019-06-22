/*============================================================================
 *  Name       : UsbDevice.h
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



#ifndef USBDEVICE_H_
#define USBDEVICE_H_

#include <wchar.h>

#include <vector>
#include "../utils/loguru.hpp"
#include <iostream>
#include <string.h>
#include "PumpStatus.h"


#include <hex.h>
#include <sha.h>

class UsbDevice {
public:
	UsbDevice();
	virtual ~UsbDevice();
	std::string stickSerial;
	std::string hmac;
	void setStickSerial(std::string);
	void setHMAC();
	std::string getHMAC();
	void incMinimedSequenceNumber();
	void incBinarySequenceNumber();
	void incPumpSequenceNumber();
	void getLinkKey(std::vector<unsigned char> payload);
	int getSignalStrengh();
	void getIV(unsigned char * iv);



	long binarySequenceNumber = 1;
	long minimedSequenceNumber = 1;
	unsigned char pumpSequenceNumber = 0;
	unsigned long long pumpMac = 0;
	unsigned long long linkMac = 0;
	unsigned char key[16];
	unsigned char radiorssi = 0;
	unsigned int pumpTimeRTC;
	int pumpTimeOFFSET;

	unsigned char radioChannel = 0x00;
//	PumpStatus pumpStatus;

//	void setPumpStatus(PumpStatus status);
//	PumpStatus * getPumpStatus();

private:

	std::string getSHA256(std::string data, unsigned char *ch);
	std::string HMAC_PADDING = "A4BD6CED9A42602564F413123";

};

#endif /* USBDEVICE_H_ */
