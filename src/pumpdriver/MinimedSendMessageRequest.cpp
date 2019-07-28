/*============================================================================
 *  Name       : MinimedSendMessageRequest.cpp
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


#include "MinimedSendMessageRequest.h"

#include <ctime>
#include <chrono>

MinimedSendMessageRequest::MinimedSendMessageRequest() {
	// TODO Auto-generated constructor stub

}

MinimedSendMessageRequest::~MinimedSendMessageRequest() {
	// TODO Auto-generated destructor stub
}

/**
 * getting the payload.
 *
 * @param mode EncryptionMode = 0x01. HighSpeedMode = 0x10. They can both be enabled: 0x11
 * @return the crypted payload
 */

std::vector<unsigned char> MinimedSendMessageRequest::getEncryptedMessage(UsbDevice *device, short int cmd, std::vector<unsigned char> payload) {

	int size = 0;
	if (!payload.empty()) {
		size = payload.size();
	}
	unsigned char seq = 3; //ms.getPumpSequenceNumber();
	std::vector<unsigned char> payloadbuf;
	if (cmd == minimedCommands.HIGH_SPEED_MODE_COMMAND) {
		payloadbuf.push_back(0x80);

	} else {
		payloadbuf.push_back(device->pumpSequenceNumber);
	}

	char c1;
	char c2;

	c1 = cmd >> 8;
	c2 = cmd;

	payloadbuf.push_back(c1);
	payloadbuf.push_back(c2);

	if (size != 0) {
		payloadbuf.insert(payloadbuf.end(), &payload[0], &payload[payload.size()]);
	}
	payloadbuf.push_back(0x00);
	payloadbuf.push_back(0x00);

	int val = hlp.CRC16CCITT(payloadbuf, 0, 0xFFFF, 0x1021, (int) payload.size() + 3);
	c1 = val >> 8;
	c2 = val;
	payloadbuf.pop_back();
	payloadbuf.pop_back();

	payloadbuf.push_back(c1);
	payloadbuf.push_back(c2);

	std::vector<unsigned char> buf;
	unsigned long long int ll = device->pumpMac;
	unsigned char pM[8];
	hlp.longlongtocharLE(ll, &pM[0]);
	buf.insert(buf.end(), &pM[0], &pM[8]);
	buf.push_back(seq);
	buf.push_back(0x11);			// mode: highspeed and encrypted
	buf.push_back(payloadbuf.size());
	std::vector<unsigned char> encpayload;
	encpayload = hlp.encrypt(device, payloadbuf);
	buf.insert(buf.end(), &encpayload[0], &encpayload[encpayload.size()]);

	device->incPumpSequenceNumber();

	return buf;
}

/**
 * @desc decode a received crypted MinimedMessage
 * @param payload the payload of the message (a received from MinimedMessage.decodeMessage())
 * @returns the decrypted message
 */

std::vector<unsigned char> MinimedSendMessageRequest::decodeMessage(UsbDevice *device, std::vector<unsigned char> payload) {

	if (payload.size() < 25) {
		//FIXMED add exception
		return {};
		//throw new MinimedMessageException("Wrong message length for decoding... abort");
	}
	int size = payload[21]; // you find the crypted message size in byte 21
	if (size == 0x00) {
		//FIXMED add exception
		return {};
		//	throw new MinimedMessageException("Wrong message length for decoding... abort");
	}
	std::vector<unsigned char> msg;
	msg.insert(msg.begin(), &payload[22], &payload[22 + size]);

	std::vector<unsigned char> decrypted = hlp.decrypt(device, msg);
	msg.clear();
	msg.insert(msg.begin(), &decrypted[0], &decrypted[decrypted.size() - 2]); // cut the crc

	// sanity check:
	char c1 = decrypted[decrypted.size() - 2];
	char c2 = decrypted[decrypted.size() - 1];
	short int s = c2;
	s += c1 << 8; // the checksum in little endian

	int crc = hlp.CRC16CCITT(msg, 0, 0xFFFF, 0x1021, (int) msg.size());
	if (s - crc != 0) {
		// TODO: the checksum often fails and i havent got the faintest idea why!? - even with wrong crc the payload is calculated right!!!
		LOG_F(5, "CRC-ERROR! Ignoring");
		//throw new MinimedMessageException("Message Checksum Error");

	}
	LOG_F(7, "Decrypted:                                  %s", hlp.toHexString(msg).c_str());

	return msg;

}

/*
 * @desc decodes the pump and sensor status
 * @return 0 is success; 1 is error
 */

int MinimedSendMessageRequest::decodeMessageAndStatus(UsbDevice *device, std::vector<unsigned char> payload, PumpStatus * ps) {
	std::vector<unsigned char> decoded = decodeMessage(device, payload); // decrypt the payload out of the minimed message

	/*
	 * Example: Payload received:
	 *	0         5        10         5        20         5        30         5        40         5        50         5        60         5        70         5        80         5        90         5        1

	 *	00                              10                             20                               30                              40                             50                               60
	 *	02013c500000000000000000000000000000138824c675c3bb000200001b580000000000000000005cc664001227761900000003e800ce80a3b8dea422dd9800600000016323ffae000000000000000000000000000000000000008700000087
	 *  02013c500000000000000000000000000000138824c7a9bbc2000200001b58000000000000000000109a64000e21301900000007d0006880a4e2a7a422dd980060000002bc27ffef000000000000000000000000000000000000008700000087
	 */

	//FIXME: This log shall on level 4!
	LOG_F(9,"Payload received: %s", hlp.toHexString(decoded).c_str());

	if (hlp.extractCommand(decoded) - minimedCommands.READ_PUMP_STATUS_RESPONSE == 0) {

		std::vector<unsigned char> tmp(4, 0);

		ps->sensorBGL = hlp.charToShort(decoded[53], decoded[54]);
		tmp.insert(tmp.end(), &decoded[0x1b], &decoded[0x1e]);
		ps->currentBasalRate = hlp.chartolong(tmp) / 10000;
		tmp.clear();

		tmp.insert(tmp.begin(), &decoded[0x21], &decoded[0x22]);
		ps->tempBasalRate = hlp.chartolonglong(0, 0, 0, 0, 0, 0, decoded[0x21], decoded[0x22]) / 10000;
		ps->tempBasalPercentage = decoded[0x23];
		ps->batteryLevelPercentage = decoded[0x2a];
		ps->insulinUnitsRemaining = hlp.chartolonglong(0, 0, 0, 0, decoded[0x2b], decoded[0x2c], decoded[0x2d], decoded[0x2e]) / 10000;
		ps->tempBasalMinutesRemaining = hlp.chartolonglong(0, 0, 0, 0, 0, 0, decoded[0x24], decoded[0x25]);

		ps->activeInsulin = (float) hlp.chartolonglong(0, 0, 0, 0, 0, 0, decoded[51], decoded[52]) / 10000;
		ps->pumpStatus = decoded[0x03];
		ps->timeToCalibrate = hlp.charToShort(decoded[0x43], decoded[0x44]);
		ps->sensorStatus = decoded[0x41];
		ps->sensorBGLTimeStamp = hlp.chartolonglong(decoded[0x37], decoded[0x38], decoded[0x39], decoded[0x3a], decoded[0x3b], decoded[0x3c], decoded[0x3d],
				decoded[0x3e]);
		ps->sensorBattery = decoded[0x45];
		if (decoded[72] == 0)
			ps->recentBolusWizard = false;
		else
			ps->recentBolusWizard = true;
		ps->bolusWizardBGL = hlp.chartolonglong(0, 0, 0, 0, 0, 0, decoded[73], decoded[74]);
		ps->trendArrow = decoded[0x40];
		ps->readTime = time(0);
		return 0;
	}
	return 1;

}

int MinimedSendMessageRequest::decodePumpHistoryInfo(UsbDevice *device, std::vector<unsigned char> payload, PumpHistory * ph) {

	std::vector<unsigned char> decoded = decodeMessage(device, payload); // decrypt the payload out of the minimed message

	if (hlp.extractCommand(decoded) - minimedCommands.READ_HISTORY_INFO_RESPONSE == 0) {
		std::vector<unsigned char> tmp(4, 0);
//		tmp.insert(tmp.end(), &decoded[4], &decoded[4]+4);
		ph->historySize = hlp.chartouint(decoded, 4);
		tmp.clear();
		ph->historyStart = hlp.chartolonglong(decoded[8], decoded[23], decoded[10], decoded[11], decoded[12], decoded[13], decoded[14], decoded[15]);
		ph->historyEnd = hlp.chartolonglong(decoded[16], decoded[17], decoded[18], decoded[19], decoded[20], decoded[21], decoded[22], decoded[23]);
		return 0;

	}
	LOG_F(ERROR, "Wrong message type returned");
	return 1;
}

int MinimedSendMessageRequest::initiateMultiPacketTransfer(UsbDevice *device, std::vector<unsigned char> payload, PumpHistory * ph) {
	ph->multiPacketSegmentSize = hlp.chartouint(payload, 3);
	ph->multiPacketPacketSize = hlp.charToShort(payload[7], payload[8]);
	ph->multiPacketLastPacketSize= hlp.charToShort(payload[9], payload[10]);
	ph->multiPacketPacketToFetch= hlp.charToShort(payload[11], payload[12]);

	return 0;
}


int MinimedSendMessageRequest::processMultiPacketTransferMessage(UsbDevice *device, PumpHistory * ph) {



	return 0;
//	ph->segments.push_back(hlp.copyVectorPart(payload,5,payload.size()));
//	return hlp.charToShort(payload[3], payload[4]);

}

int MinimedSendMessageRequest::decodePumpHistory(UsbDevice *device, std::vector<unsigned char> payload, PumpHistory * ph) {

	std::vector<unsigned char> decoded = decodeMessage(device, payload); // decrypt the payload out of the minimed message
	LOG_F(7, "decoding history message");

	return 1;
}

