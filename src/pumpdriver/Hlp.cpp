/*============================================================================
 *  Name       : Hlp.cpp
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



#include "Hlp.h"
#include <ctime>
#include <chrono>

MinimedCommands minimedCommands;

template<size_t multiple> size_t round_up(const size_t len) {
	if (len % multiple == 0)
		return len;
	else
		return ((len / multiple) + 1) * multiple;
}

Hlp::Hlp() {
	// TODO Auto-generated constructor stub

}

Hlp::~Hlp() {
	// TODO Auto-generated destructor stub
}

/**
 * @brief converts a vector to a hex string
 *
 */

std::string Hlp::toHexString(std::vector<unsigned char> writebuffer) {

	std::ostringstream os;
	for (int i = 0; i < (int) writebuffer.size(); ++i) {
		os << std::setfill('0') << std::setw(2) << std::hex << (int) writebuffer[i];
//		os << " ";
	}

	return os.str();
}

int Hlp::CRC16CCITT(std::vector<unsigned char> data, int offset, int initialValue, int polynomial, int bytesToCheck) {
	// From http://introcs.cs.princeton.edu/java/61data/CRC16CCITT.java
	int crc = initialValue;

	for (int c = 0; c < bytesToCheck; c++) {
		unsigned char b = data[c + offset];
		for (int i = 0; i < 8; i++) {
			bool bit = ((b >> (7 - i) & 1) == 1);
			bool c15 = ((crc >> 15 & 1) == 1);
			crc <<= 1;
			if (c15 ^ bit)
				crc ^= polynomial;
		}
	}

	crc &= 0xffff;
	return crc;
}

/**
 * @desc crypts the message
 */
std::vector<unsigned char> Hlp::encrypt(UsbDevice * usbDevice, std::vector<unsigned char> data) {
	std::vector<unsigned char> enc;

	size_t messageLen = data.size();
	unsigned char ivC[16];
	memcpy(ivC, usbDevice->key, 16);
	byte * iv = &ivC[0];
	iv[0] = usbDevice->radioChannel;
	byte * key = (byte *) usbDevice->key;
	byte encText[messageLen];
	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(key, 16, iv);
	cfbEncryption.ProcessData(encText, data.data(), messageLen);

	return std::vector<unsigned char>(encText, encText + messageLen);

}

/**
 * @desc decrypts the message
 */
std::vector<unsigned char> Hlp::decrypt(UsbDevice * usbDevice, std::vector<unsigned char> data) {
	std::vector<unsigned char> enc;

	size_t messageLen = data.size();
	unsigned char ivC[16];
	memcpy(ivC, usbDevice->key, 16);
	byte * iv = &ivC[0];
	iv[0] = usbDevice->radioChannel;
	byte * key = (byte *) usbDevice->key;
	byte encText[messageLen];
	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(key, 16, iv);
	cfbDecryption.ProcessData(encText, data.data(), messageLen);

	return std::vector<unsigned char>(encText, encText + messageLen);

}

/**
 * extraxt the time from the pump
 */
std::string Hlp::extractTime(UsbDevice * usbDevice, std::vector<unsigned char> data) {

//	Date pumpTime;

	if (extractCommand(data) - minimedCommands.TIME_RESPONSE == 0) {
		usbDevice->pumpTimeRTC = read32BEtoInt(data, 0x04);
		usbDevice->pumpTimeOFFSET = read32BEtoInt(data, 0x08);
	}

	else {
		//FIXME throw Exception
		return "Error: no valid answer!";
	}

	std::string s(decodeDateTime(usbDevice->pumpTimeRTC & 0xFFFFFFFFL, usbDevice->pumpTimeOFFSET));

	return s;
}

/**
 * check whether byte 19 of the answer contains the expected code... CRC will be
 * done in messagedecoding...
 *
 * @param answer
 * @param expectedCode
 * @return
 */
bool Hlp::checkAnswer(std::vector<unsigned char> answer, unsigned char expectedCode) {
	if (answer[18] != expectedCode) {
		/*
		 * Check answer: Byte 1 2 3 4 5 6 7 8 9 10 11 Hex 00 04 00 00 00 00 03 00 01 05
		 * 02
		 *
		 * NB: Medtronic Message payload is empty if it's a response to a
		 * "Negotiate Channel"
		 *
		 * 1-2 Command response code?
		 *
		 * 3-9 Unknown bytes
		 *
		 * 10 Sequence number. Should be the same as the corresponding SEND_MESSAGE
		 * (0x12) sequence number
		 *
		 * 11 Unknown byte
		 *
		 */

		return false;
	}
	return true;
}

/**
 * @brief converts a vector to a string
 *
 */

std::string Hlp::toString(std::vector<unsigned char> writebuffer) {
	return std::string(writebuffer.begin(), writebuffer.end());
}

std::string Hlp::wchartochar(wchar_t *wc) {
	char buffer[64];
	std::wcstombs(buffer, &wc[0], 64);
	return std::string(buffer);

}

std::string Hlp::string_to_hex(const std::string& input) {
	static const char* const lut = "0123456789abcdef";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i) {
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}

/**
 * @desc	converts a char array in a string of its hex values
 */

std::string Hlp::char_to_hex(unsigned char input[], int size) {
	static const char* const lut = "0123456789abcdef";
	std::string output;
	output.reserve(2 * size);
	for (int i = 0; i < size; ++i) {
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;

}

/**
 * @desc returns the 4 bytes of a long in little endian
 * @param  l the long to extract
 * @param  c the 4 byte char array which contains the result
 */
void Hlp::longtocharLE(long l, unsigned char * c) {
	c[3] = l >> 24;
	c[2] = l >> 16;
	c[1] = l >> 8;
	c[0] = l;

}

/**
 * @desc returns the 8 bytes of a long in little endian
 * @param  l the long to extract
 * @param  c the 8 byte char array which contains the result
 */
void Hlp::longlongtocharLE(unsigned long long l, unsigned char * c) {
	c[0] = l;
	c[1] = l >> 8;
	c[2] = l >> 16;
	c[3] = l >> 24;
	c[4] = l >> 32;
	c[5] = l >> 40;
	c[6] = l >> 48;
	c[7] = l >> 56;

}

short int Hlp::charToShort(unsigned char c1, unsigned char c2) {
	short s = 0;
	s += c1 << 8;
	s += c2;

	return s;

}

/**
 * @desc returns the 4 bytes of a long in big endian
 * @param  l the long to extract
 * @param  c the 4 byte char array which contains the result
 */

void Hlp::longtocharBE(long l, unsigned char * c) {

	c[0] = l >> 24;
	c[1] = l >> 16;
	c[2] = l >> 8;
	c[3] = l;

}

/**
 * @desc converts  8 single bytes of unsigned char to long
 * @param every single byte
 * @return: the long value
 */
unsigned long long Hlp::chartolonglong(unsigned char c7, unsigned char c6, unsigned char c5, unsigned char c4, unsigned char c3, unsigned char c2,
		unsigned char c1, unsigned char c0) {

	unsigned long long l;

	l = c7 << 56;

	l += c6 << 48;

	l += c5 << 40;

	l += c4 << 32;

	l += c3 << 24;
	l += c2 << 16;
	l += c1 << 8;
	l += c0;

	return l;
}

/**
 * @desc converts a 8 byte vector of unsigned char to long
 * @param the vector to convert (we start with the offset)
 * @return: the long value
 */
unsigned long long Hlp::chartolong(std::vector<unsigned char> in, int offset) {

	// TODO clean up (THIS IS UGLY!)
	if (in.size()+offset < 8)
		return 0;
	unsigned long long l;
	unsigned long long tmp;
	tmp = in[0+offset];
	l = tmp << 56;
	tmp = in[1+offset];
	l += tmp << 48;
	tmp = in[2+offset];
	l += tmp << 40;
	tmp = in[3+offset];
	l += tmp << 32;
	tmp = in[4+offset];
	l += tmp << 24;
	l += in[5+offset] << 16;
	l += in[6+offset] << 8;
	l += in[7+offset];

	return l;
}

/**
 * @desc converts a 8 byte vector of unsigned char to long
 * @param the vector to convert (we only take the first 8 byte)
 * @return: the long value
 */
unsigned long long Hlp::chartolong(std::vector<unsigned char> in) {

	// TODO clean up (THIS IS UGLY!)
	if (in.size() < 8)
		return 0;
	unsigned long long l;
	unsigned long long tmp;
	tmp = in[0];
	l = tmp << 56;
	tmp = in[1];
	l += tmp << 48;
	tmp = in[2];
	l += tmp << 40;
	tmp = in[3];
	l += tmp << 32;
	tmp = in[4];
	l += tmp << 24;
	l += in[5] << 16;
	l += in[6] << 8;
	l += in[7];

	return l;
}

/**
 * @desc converts a 4 byte vector of unsigned char to long
 * @param the vector to convert (we only take the first 8 byte
 * @return: the long value
 */
unsigned int Hlp::chartouint(std::vector<unsigned char> in, int offset) {

	// TODO clean up (THIS IS UGLY!)
	if (in.size() < 4)
		return 0;
	unsigned int l;
	unsigned int tmp;
	tmp = in[0 + offset];
	l = tmp << 24;
	l += in[1 + offset] << 16;
	l += in[2 + offset] << 8;
	l += in[3 + offset];

	return l;
}

unsigned int Hlp::read32BEtoInt(std::vector<unsigned char> data, int offset) {
	return (data[offset] << 24 & 0xFF000000) | (data[offset + 1] << 16 & 0x00FF0000) | (data[offset + 2] << 8 & 0x0000FF00) | (data[offset + 3] & 0x000000FF);
}

unsigned char Hlp::minimedMessageChecksum(std::vector<unsigned char> msg, unsigned char *payload, int payloadsize) {
	int checksum = 0;

	for (unsigned int i = 0; i < msg.size(); i++) {
		checksum += msg[i];
	}
	if (payloadsize != 0) {
		for (int i = 0; i < payloadsize; i++) {
			checksum += payload[i];
		}
	}
	return (unsigned char) checksum;
}

short Hlp::extractCommand(std::vector<unsigned char> msg) {
	if (msg.empty())
		return 0;
	unsigned char c1 = msg[2];
	unsigned char c2 = msg[1];
	short int s = c1;
	s += c2 << 8; // the command
	return s;

}

char* Hlp::decodeDateTime(long rtc, long long offset) {
	/*
	 * The following code  is taken from  * "https://github.com/pazaan/600SeriesAndroidUploader" - Thanks to pazaan for
	 * his great work. Copyright (c) 2016 Lennart Goedhart (c) 2017
	 * 600SeriesAndroidUploader Authors
	 */
	// Base time is midnight 1st Jan 2000 (GMT)
	long int baseTime = 946684800;
	// The time from the pump represents epochTime in GMT, but we treat it as if it were in our own timezone
	// We do this, because the pump does not have a concept of timezone
	// For example, if baseTime + rtc + offset was 1463137668, this would be
	// Fri, 13 May 2016 21:07:48 GMT.
	// However, the time the pump *means* is Fri, 13 May 2016 21:07:48 in our own timezone
	time_t t = time(NULL);
	struct tm lt = { 0 };
	localtime_r(&t, &lt);
	std::time_t mytime = (baseTime + rtc + offset) - lt.tm_gmtoff;
	return std::ctime(&mytime);
}

void Hlp::copyVectorPart(std::vector<unsigned char> input, std::vector<unsigned char> *output, int startOffset, int endOffset) {
	output->clear();
	for (int i = startOffset; i<endOffset; i++) {
		output->push_back(input.at(i));
	}
}

std::vector<unsigned char> Hlp::copyVectorPart(std::vector<unsigned char> input, int startOffset, int endOffset) {
	std::vector<unsigned char> output;
	for (int i = startOffset; i<endOffset; i++) {
		output.push_back(input.at(i));
	}
	return output;
}
