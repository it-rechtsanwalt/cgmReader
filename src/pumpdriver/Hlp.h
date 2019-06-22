/*============================================================================
 *  Name       : Hlp.h
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



#ifndef HLP_H_
#define HLP_H_

#include <iostream>
#include <exception>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include "UsbDevice.h"
#include "USBDriver.h"

//**  Crypto Stuff!
#include <hmac.h>
#include <osrng.h>
#include <secblock.h>
#include <hex.h>
//#include <sha.h>
#include <sha.h>
#include <filters.h>
#include <modes.h>

class Hlp {
public:
	Hlp();
	virtual ~Hlp();
	std::string toString(std::vector<unsigned char> writebuffer);
	std::string toHexString(std::vector<unsigned char> writebuffer);
	std::string wchartochar(wchar_t *wc);
	std::string string_to_hex(const std::string& input);
	std::string char_to_hex(unsigned char input[], int size);
	unsigned long long chartolonglong(unsigned char c7, unsigned char c6, unsigned char c5, unsigned char c4, unsigned char c3, unsigned char c2,
			unsigned char c1, unsigned char c0);
	unsigned long long chartolong(std::vector<unsigned char> in);
	unsigned long long chartolong(std::vector<unsigned char> in, int offset);
	short int charToShort(unsigned char c1, unsigned char c2);
	void longtocharBE(long l, unsigned char *c);
	void longtocharLE(long l, unsigned char *c);
	void longlongtocharLE(unsigned long long l, unsigned char * c);
	//unsigned long long chartolong(std::vector<unsigned char> in);
	unsigned char minimedMessageChecksum(std::vector<unsigned char> msg, unsigned char *payload, int payloadsize);
	int CRC16CCITT(std::vector<unsigned char> data, int offset, int initialValue, int polynomial, int bytesToCheck);
	std::vector<unsigned char> encrypt(UsbDevice * usbDevice, std::vector<unsigned char> data);
	std::vector<unsigned char> decrypt(UsbDevice * usbDevice, std::vector<unsigned char> data);
	std::string extractTime(UsbDevice * usbDevice, std::vector<unsigned char> data);
	bool checkAnswer(std::vector<unsigned char> answer, unsigned char expectedCode);
	unsigned int chartouint(std::vector<unsigned char> in, int offset);
	unsigned int read32BEtoInt(std::vector<unsigned char> data, int offset);
	short extractCommand(std::vector<unsigned char> msg);

	void copyVectorPart(std::vector<unsigned char> input, std::vector<unsigned char> *output, int startOffset, int endOffset);
	std::vector<unsigned char> copyVectorPart(std::vector<unsigned char> input, int startOffset, int endOffset);
	char* decodeDateTime(long rtc, long long offset);

private:

};
#endif /* HLP_H_ */
