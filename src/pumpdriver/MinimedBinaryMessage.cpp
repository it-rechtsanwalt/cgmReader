/*============================================================================
 *  Name       : MinimedBinaryMessage.cpp
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



#include "MinimedBinaryMessage.h"

MinimedBinaryMessage::MinimedBinaryMessage() {
	// TODO Auto-generated constructor stub

}

MinimedBinaryMessage::~MinimedBinaryMessage() {
	// TODO Auto-generated destructor stub
}

/*
 * @desc "builds" a minimed message.
 *
 * @param device   pointer to to the usbdevice. this contains informatoions we need
 * @param type 	the type of the message. see the commands in pumpdriver.h
 * @param payload the payload of the message
 * @param payloadsize  the size of the payload
 * @return returns a vecor with the whole build message
 */

const std::vector<unsigned char> MinimedBinaryMessage::getMessage(
		UsbDevice *device, unsigned char type, unsigned char *payload,
		int payloadsize) {
	std::vector<unsigned char> msg;
	// 60 Bytes long message

	msg.push_back(0x51); // byte 1 Passthrough Command, 0x51 ("Q")
	msg.push_back(0x03); // byte 2 Device Type 0x03 for minimed NGP
	for (int i = 0; i < 6; i++) { // byte 3 to 8
		msg.push_back('0');
	}
	for (int i = 0; i < 10; i++) { // byte 9 to 18
		msg.push_back(0x00);
	}
	msg.push_back(type); // byte 19 operation type
	unsigned char c[4];
	hlp.longtocharLE(device->minimedSequenceNumber, &c[0]);
	msg.push_back(c[0]);    // byte 20 to 23 Sequence number little Endian
	msg.push_back(c[1]);
	msg.push_back(c[2]);
	msg.push_back(c[3]);

	for (int i = 0; i < 5; i++) { // byte 24 to 28
		msg.push_back(0x00);
	}
	if (payloadsize != 0) { // byte 29 and 30 payload length in bytes, littel endian 2 byte unsigned int.
		// length > 27: 2 frames!
		msg.push_back((unsigned char) payloadsize);
		msg.push_back(payloadsize >> 8);
	}
	if (payloadsize == 0) {
		msg.push_back(0x00);		 // byte 29;
		msg.push_back(0x00);		 // byte 30;
	}
	msg.push_back(0x00);		 // byte 31;
	msg.push_back(0x00);		 // byte 32;

	msg.push_back(hlp.minimedMessageChecksum(msg, payload, payloadsize));// byte 33 checksum (Binary Message CRC: It’s the lower byte of the
	// rolling checksum of the entire packet from the Q up to the end of
	// the payload but NOT including that 1 byte itself

	msg.insert(msg.end(), &payload[0], &payload[payloadsize]);
	device->incMinimedSequenceNumber();
	return msg;
}

bool checkChecksum(std::vector<unsigned char> message) {
	int checksum = 0;
	for (unsigned int i = 0; i < message.size(); i++) {
		checksum += message[i];
	}
	checksum -= message[32];
	if ((checksum & 0xFF) == message[32]) {
		return true;
	}
	return false;
}

/**
 * @desc	extracts the payload of a received message
 * 			it does a minimalistic checksum check
 * @param	the message to extract
 * @return	the payload of the message
 */

const std::vector<unsigned char> MinimedBinaryMessage::decode(std::vector<unsigned char> msg) {
	std::vector<unsigned char> payload;
	if (msg.empty()) {return {};}

	payload.assign(msg.begin() + 33, msg.end());
	type = msg[18];
	if (!checkChecksum(msg)) {
		LOG_F(ERROR, "Checksumerror in Minimed Binary Message.");
		//TODO add an exception here!
	}
	return payload;
}
