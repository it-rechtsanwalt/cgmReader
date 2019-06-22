/*============================================================================
 *  Name       : UsbDevice.cpp
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



#include "UsbDevice.h"

UsbDevice::UsbDevice() {

}

UsbDevice::~UsbDevice() {
	// TODO Auto-generated destructor stub
}

void UsbDevice::setStickSerial(std::string serial) {

	stickSerial = serial.c_str();

}

void UsbDevice::setHMAC() {
	std::string hm;

	stickSerial.erase(0, stickSerial.find_first_not_of('0'));

	hm.append(stickSerial);
	hm.append(HMAC_PADDING);
	unsigned char sha[64];
	LOG_F(9, "String for HMAC: %s", hm.c_str());
	hmac = getSHA256(hm.c_str(), &sha[0]);
}

std::string UsbDevice::getSHA256(std::string data, unsigned char *c) {
	typedef unsigned char byte;
	byte const* pbData = (byte*) data.data();
	unsigned int nDataLen = data.length();

	byte abDigest[CryptoPP::SHA256::DIGESTSIZE];

	CryptoPP::SHA256().CalculateDigest(abDigest, pbData, nDataLen);

	std::string s;
	for (int i = CryptoPP::SHA256::DIGESTSIZE - 1; i >= 0; i--) {
		s.append(1, ((char) abDigest[i]));

	}

	return s;

}

/*
 * The following code snipped is taken from
 * "https://github.com/pazaan/600SeriesAndroidUploader" (adapted for C++ by Compuholic) - Thanks to pazaan for
 * his great work. Copyright (c) 2016 Lennart Goedhart (c) 2017
 * 600SeriesAndroidUploader Authors
 */

void UsbDevice::getLinkKey(std::vector<unsigned char> payload) {

	int pos = stickSerial.at(stickSerial.size() - 1) & 7;

	for (unsigned int i = 0; i < sizeof key; i++) {
		if ((payload[pos + 1] & 1) == 1) {
			key[i] = ~payload[pos];
		} else {
			key[i] = payload[pos];
		}
		if (((payload[pos + 1] >> 1) & 1) == 0) {
			pos += 3;

		} else {
			pos += 2;
		}
	}
}



int UsbDevice::getSignalStrengh() {
	return (((int) radiorssi & 0x00FF) * 100) / 0xA8;
}

void UsbDevice::incMinimedSequenceNumber() {
	minimedSequenceNumber++;
}
void UsbDevice::incBinarySequenceNumber() {
	binarySequenceNumber++;
}
void UsbDevice::incPumpSequenceNumber() {
	pumpSequenceNumber++;
}

