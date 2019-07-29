/*============================================================================
 *  Name       : PumpDriver.cpp
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



#include "PumpDriver.h"

Consts consts;

PumpDriver::PumpDriver() {

}

PumpDriver::~PumpDriver() {
	// TODO Auto-generated destructor stub
}

void PumpDriver::init() {

	usbDevice->setHMAC();

}

int PumpDriver::resetStick(unsigned short int vid, unsigned short int pid) {
	return usbDriver.resetDevice(vid, pid);
}

/**
 * @brief receiving the stick data
 * it says hello to the stick.
 */
int PumpDriver::getStickData() {
	unsigned char buf[] = { 0x58 };
	usbDriver.send(buf, 1);
	//FIXME when the answer is "bad" do something!
	std::vector<unsigned char> answer = usbDriver.read();
	answer = usbDriver.read();
	if (answer.empty()) {
		return -1;
	}
	return 0;

}

void PumpDriver::openPumpConnection() {
	LOG_F(1, "Opening Pump Connection...");
	unsigned char buf[] = { consts.NAK };
	usbDriver.send(buf, 1);
	std::vector<unsigned char> answer = usbDriver.read();
	unsigned char buf1[] = { consts.ENQ };
	usbDriver.send(buf1, 1);
	answer = usbDriver.read();
	unsigned char buf2[] = { consts.STX, 'W', '|' };
	usbDriver.sendCommand(buf2, 3);
	answer = usbDriver.read();
	unsigned char buf3[] = { consts.STX, 'Q', '|' };
	usbDriver.sendCommand(buf3, 3);
	answer = usbDriver.read();
	unsigned char buf4[] = { consts.STX, '1', '|' };
	usbDriver.sendCommand(buf4, 3);
	answer = usbDriver.read();

}

void PumpDriver::enterPassThroughMode() {
	LOG_F(1, "Entering PassThroughMode...");
	std::vector<unsigned char> message;
	MinimedBinaryMessage bmsg;
	LOG_F(3, "HMAC: %s", hlp.string_to_hex(usbDevice->hmac.c_str()).c_str());

	message = bmsg.getMessage(usbDevice, commands.OPEN_CONNECTION, (unsigned char *) &usbDevice->hmac.c_str()[0], 32);
	usbDriver.send(message.data(), message.size());

	std::vector<unsigned char> answer = usbDriver.read(); // TODO we discard this message, we do not need it (now)... if it says
	// 01ffffffffffffffff000000000000000000 means "already connected to pump
	// network"(?) to check!

}

void PumpDriver::readInfo() {
	LOG_F(1, "Reading Info...");
	// getting the info for linkmac and pumpmac:
	MinimedBinaryMessage bmsg;
	std::vector<unsigned char> message;
	message = bmsg.getMessage(usbDevice, commands.READ_INFO, NULL, 0);
	usbDriver.send(message.data(), message.size());

	//TODO cleaner source code demanded...
	std::vector<unsigned char> answer = usbDriver.read();
	if (answer.empty()) {return; }
	std::vector<unsigned char> payload = bmsg.decode(answer);

	// extract the linkmac and pumpmac
	std::vector<unsigned char> tmp;
	tmp.assign(payload.begin(), payload.begin() + 8);	// the linkmac is found in the first 8 byte of the answer (unsigned long long)
	unsigned long long l = hlp.chartolong(tmp);
	usbDevice->linkMac = l;
	tmp.assign(payload.begin() + 8, payload.begin() + 16); // the pumpmac is found in the next 8 byte of the answer (unsigned long long)
	l = hlp.chartolong(tmp);
	usbDevice->pumpMac = l;
	LOG_F(2, "LinkMac: %llu", usbDevice->linkMac);
	LOG_F(2, "PumpMac: %llu", usbDevice->pumpMac);

	// request link key:
	message = bmsg.getMessage(usbDevice, commands.REQUEST_LINK_KEY, NULL, 0);
	usbDriver.send(message.data(), message.size());
	answer.clear();
	payload.clear();
	answer = usbDriver.read();

	payload = bmsg.decode(answer);
	usbDevice->getLinkKey(payload);
	LOG_F(1, "LinkKey: %s", hlp.char_to_hex(usbDevice->key, 16).c_str());
}

int PumpDriver::channelNegotiation() {
	LOG_F(1, "Channel Negotiation...");

	char channels[] = { 0x14, 0x11, 0x0e, 0x17, 0x1a };

	for (unsigned int i = 0; i < sizeof channels; i++) {

		// creating a channel negotiationmessage
		usbDevice->radioChannel = channels[i];
		MinimedBinaryMessage bmsg;
		MimimedMessage mmsg;
		std::vector<unsigned char> message;
		message = mmsg.channelNegotiationMessage(usbDevice->radioChannel, usbDevice->linkMac, usbDevice->pumpMac);
		message = mmsg.buildMessage(0x03, message);
		message = bmsg.getMessage(usbDevice, commands.SEND_MESSAGE, message.data(), message.size());
		usbDevice->incMinimedSequenceNumber();
		LOG_F(2, "Trying channel 0x%x", usbDevice->radioChannel);
		usbDriver.send(message.data(), message.size());
		std::vector<unsigned char> answer = usbDriver.read();
		if (answer.empty())
		{return -2;  }
		answer = bmsg.decode(answer);
		if (bmsg.type == 0x81) {		// first answer must be a 0x81 message!
			usleep(200);
			answer = saveRead();
			if (answer.empty())
				return -2;
			answer = bmsg.decode(answer);
			if (bmsg.type == 0x80) {	// second answer must be a 0x80 message!
				if (answer.size() > 13) {
					if (answer[43] == usbDevice->radioChannel) {
						LOG_F(2, "Channel negotiated with Channel ID %d" + usbDevice->radioChannel);
						usbDevice->radiorssi = answer[26];
						LOG_F(2, "Signal Strength: %d%%", usbDevice->getSignalStrengh());

						return usbDevice->radioChannel;
					}
				}
				LOG_F(2, "No pump at channel 0x%x found.", usbDevice->radioChannel);
			}
		}

	}
	LOG_F(INFO, "No pump found. Is the pump near?");
	return 0;

}

int PumpDriver::enteringEHSM() {
	LOG_F(1, "Entering EHSM...");
	std::vector<unsigned char> payload;
	payload.push_back(0x00);

	std::vector<unsigned char> answer = sendMessage(minimedCommands.HIGH_SPEED_MODE_COMMAND, payload);
	if (answer.empty())
		return 1;
	if (!hlp.checkAnswer(answer, 0x81)) {
		LOG_F(WARNING, "Answer was not 0x81. This should not happen!");
		//FIXME: Error handling!
		//FIXME: Throw exception?

	} else {
		MimimedMessage mmsg;
		answer = mmsg.decodeMessage(answer);
	}

	return 0;

}

int PumpDriver::getPumpTime() {

	LOG_F(1, "Getting Pump Time");
	std::vector<unsigned char> payload;
	// send the time request and check for 0x081 response
	std::vector<unsigned char> answer = sendMessage(minimedCommands.TIME_REQUEST, { });
	if (answer.empty())
		return 1;
	if (!hlp.checkAnswer(answer, 0x81)) {
		LOG_F(WARNING, "Answer was not 0x81. This should not happen!");
		//FIXME: Error handling!
		//FIXME: Throw exception?
		return 1;

	} else {
		answer = saveRead();
		if (answer.empty())
			return 1;
		MimimedMessage mmsg;
		MinimedBinaryMessage bmsg;
		MinimedSendMessageRequest smrq;
		answer = bmsg.decode(answer); //get the payload out of the binary message
		answer = mmsg.decodeMessage(answer); // get the payload out of the minimed message
		answer = smrq.decodeMessage(usbDevice, answer); // decrypt the payload out of the minimed message
		std::string s = hlp.extractTime(usbDevice, answer);

		LOG_F(2, "Actual pumptime: %s", s.c_str());
		LOG_F(2, "Actual pumpOffset / rtc : %ul / %ul", usbDevice->pumpTimeOFFSET, usbDevice->pumpTimeRTC);
		return 0;
	}

}

int PumpDriver::downloadData(PumpStatus * ps) {
	LOG_F(1, "Download Data");
	std::vector<unsigned char> payload;
	// send the time request and check for 0x081 response
	std::vector<unsigned char> answer = sendMessage(minimedCommands.READ_PUMP_STATUS_REQUEST, { });
	if (answer.empty())
		return 1;
	if (!hlp.checkAnswer(answer, 0x81)) {
		LOG_F(WARNING, "Answer was not 0x81. This should not happen!");
		//FIXME: Error handling!
		//FIXME: Throw exception?
		return 1;

	} else {

		// get the data and check for 0x080 message
		answer = saveRead();
		if (answer.empty())
			return 1;
		if (!hlp.checkAnswer(answer, 0x80)) {
			LOG_F(WARNING, "Answer was not 0x80. This should not happen!");
			//FIXME: Error handling!
			//FIXME: Throw exception?
			return 1;
		}
		MimimedMessage mmsg;
		MinimedBinaryMessage bmsg;
		MinimedSendMessageRequest smrq;
		answer = bmsg.decode(answer); //get the payload out of the binary message
		answer = mmsg.decodeMessage(answer); // get the payload out of the minimed message
		smrq.decodeMessageAndStatus(usbDevice, answer, ps); // decrypt the payload out of the minimed message and extract the status
		ps->stickSerial = usbDevice->stickSerial;
		LOG_F(INFO, "Received Pump Status:");
		LOG_F(INFO, "______________________________________________________________________________________________________________");
		LOG_F(INFO, "Sensor BGL / active Insulin              : %d / %f", ps->sensorBGL, ps->activeInsulin);
		LOG_F(INFO, "Trend                                    : %s", ps->trendArrowString().c_str());
		LOG_F(INFO, "Insulin units remaining / batteryLevel(%): %d / %d", ps->insulinUnitsRemaining, ps->batteryLevelPercentage);
		LOG_F(INFO, "Current Basalrate                        : %f", ps->currentBasalRate);
		LOG_F(INFO, "Sensor Time                              : %d", ps->sensorBGLTimeStamp);
		LOG_F(INFO, "Bolus Wizzard                            : %d ", ps->bolusWizardBGL);
		LOG_F(INFO, "Time to calibrate                        : %d ", ps->timeToCalibrate);
        LOG_F(INFO, "Sensorstatus / PumpStatus                : 0x%x / 0x%x, ",ps->sensorStatus, ps->pumpStatus);

		LOG_F(INFO, "______________________________________________________________________________________________________________");

		return 0;
	}

}

int PumpDriver::downloadHistory(PumpStatus * ps, HistoryData * hD) {
	LOG_F(1, "Download Pump History");
	int history = 24*3600; // we want the last 24 hours, beside there is an event stored...
	if (hD->events.size()) {
		history = time(0) - hD->events.at(hD->events.size()-1).eventTime;			// ... in this case we want the history to the last event
	}
	LOG_F (6, "Requesting History, starting %d secs ago", history);
	int res = downloadHistoryData(2, history, ps);
	if (res != 0) {

		return res;
	}
	LOG_F(2, "Download completed. Received %d packets", pumpHistory.segments.size());
	LOG_F(2, "Download Sensor History");
	res = downloadHistoryData(3, history, ps);
	if (res != 0) {
		//FIXME: Handle ungraceful return!
			return res;
	}
	LOG_F(2, "Download completed. Received %d packets", pumpHistory.segments.size());

	LOG_F(1, "______________________________________________________________________________________________________________");
	LOG_F(1, "Output: ");
	LOG_F(1, "--------------------------------- RECEIVED MESSAGES: ");

	for (unsigned int i = 0; i < pumpHistory.segments.size(); i++) {
		LOG_F(1, hlp.toHexString(pumpHistory.segments[i]).c_str());
	}
	LOG_F(1, "______________________________________________________________________________________________________________");
	LOG_F(1, "Output: ");
	LOG_F(1, "--------------------------------- DECODED EVENTS: ");

	for (unsigned int i = 0; i < pumpHistory.segments.size(); i++) {
		if (processHistory(pumpHistory.segments[i], hD) == 0) {
			sliceEvents(hD);
			extractEvents(hD);
		}
		else return 2;

	}
	if (hD->events.size()) {
		LOG_F(1, "HD EVENT SIZE %d", hD->events.size());

	}

	MinimedSendMessageRequest smrq;
//	int msgNr = smrq.processMultiPacketTransferMessage(usbDevice, &pumpHistory);
	return 0;
}

void PumpDriver::closeEHSM() {
	LOG_F(1, "Closing EHSM Connection");
	std::vector<unsigned char> answer = sendMessage(minimedCommands.HIGH_SPEED_MODE_COMMAND, { 0x01 });
	if (answer.empty())
		return;
	//FIXME handle the answer

}

void PumpDriver::closeConnection() {
	MinimedBinaryMessage bmsg;
	std::vector<unsigned char> message(usbDevice->hmac.begin(), usbDevice->hmac.end());
	message = bmsg.getMessage(usbDevice, commands.CLOSE_CONNECTION, message.data(), message.size());
	usbDriver.send(message.data(), message.size());

	saveRead();
	// TODO gracefully exiting

	;
}

void PumpDriver::exitPassthroughMode() {
	unsigned char buf2[] = { consts.STX, 'W', '|' };
	usbDriver.sendCommand(buf2, 3);
	std::vector<unsigned char> answer = saveRead();
	unsigned char buf3[] = { consts.STX, 'Q', '|' };
	usbDriver.sendCommand(buf3, 3);
	answer = saveRead();
	unsigned char buf4[] = { consts.STX, '0', '|' };
	usbDriver.sendCommand(buf4, 3);
	answer = saveRead();

}

void PumpDriver::exitControlMode() {

	unsigned char buf[] = { consts.EOT };
	usbDriver.send(buf, 1);
	std::vector<unsigned char> answer = saveRead();

	// TODO check for correct answer: enq;
}

void PumpDriver::closeDriver() {

	usbDriver.close_device();
}

std::vector<unsigned char> PumpDriver::sendMessage(unsigned short command, std::vector<unsigned char> payload) {

	MinimedSendMessageRequest mmSendMsg;
	std::vector<unsigned char> message;
	message = mmSendMsg.getEncryptedMessage(usbDevice, command, payload);
	MimimedMessage mmsg;
	message = mmsg.buildMessage(0x05, message);
	MinimedBinaryMessage bmsg;
	message = bmsg.getMessage(usbDevice, commands.SEND_MESSAGE, message.data(), message.size());
	usbDriver.send(message.data(), message.size());

	return saveRead();
}

std::vector<unsigned char> PumpDriver::saveRead() {
	std::vector<unsigned char> answer = usbDriver.read();
	if (answer.empty()) {

		for (int j = 0; j < retries; j++) {
			usleep(sleep_duration);
			answer = usbDriver.read();
			if (!answer.empty()) {
				break;
			}
		}
		if (answer.empty()) {
			LOG_F(ERROR, "Timeout... ");
			return {};
			// fixme add a good return value
		}

	}
	return answer;
}

/*
 * @desc downloads a history in the given time period
 * @param type 02 is pump 03 is sensor
 * @param history history in seconds to retreive the data
 */
int PumpDriver::downloadHistoryData(char type, int historyPeriod, PumpStatus * ps) {
	std::vector<unsigned char> payload;

	// lets build our payload

	payload.push_back(type);
	payload.push_back(0x04); // ???

	time_t now = time(0);			// time now in seconds
	struct std::tm *timeinfo = std::localtime(&now);
	int offset = timeinfo->tm_gmtoff;
	int rtc = now + offset - (int) usbDevice->pumpTimeOFFSET - 946684800 - historyPeriod;
	LOG_F(4, "Request  History from: %ul", rtc);
	payload.push_back(*((char*) &rtc + 3));
	payload.push_back(*((char*) &rtc + 2));
	payload.push_back(*((char*) &rtc + 1));
	payload.push_back(*((char*) &rtc + 0));   // from date/time

//	unsigned int rtcTo = now - usbDevice->pumpTimeOFFSET - 946684800 + (3600 * 24); //our base time is 1.1.2000, 0:00 UTC the end date is one day in future...

	unsigned int rtcTo = 0xFFFFFFFF;
	payload.push_back(*((char*) &rtcTo + 0));
	payload.push_back(*((char*) &rtcTo + 1));
	payload.push_back(*((char*) &rtcTo + 2));
	payload.push_back(*((char*) &rtcTo + 3));   // from date/time

	payload.push_back(0x00); // End-of-Payload
	payload.push_back(0x00); // End-of-Payload

	LOG_F(1, " ------------------- History Message:  %s", hlp.toHexString(payload).c_str());
	// send the pump history request and check for 0x081 response
	std::vector<unsigned char> answer = sendMessage(minimedCommands.READ_HISTORY_INFO_REQUEST, payload);
	if (answer.empty())
		return 1;
	if (!hlp.checkAnswer(answer, 0x81)) {
		LOG_F(WARNING, "Answer was not 0x81. This should not happen!");
		//FIXME: Error handling!
		//FIXME: Throw exception?
		return 1;

	} else {

		// get the data and check for 0x080 message
		answer = saveRead();
		if (answer.empty())
			return 1;
		if (!hlp.checkAnswer(answer, 0x80)) {
			LOG_F(WARNING, "Answer was not 0x80. This should not happen!");
			//FIXME: Error handling!
			//FIXME: Throw exception?
			return 1;
		}
		MimimedMessage mmsg;
		MinimedBinaryMessage bmsg;
		MinimedSendMessageRequest smrq;

		answer = bmsg.decode(answer); //get the payload out of the binary message
		answer = mmsg.decodeMessage(answer); // get the payload out of the minimed message
		if (smrq.decodePumpHistoryInfo(usbDevice, answer, &pumpHistory) == 0) // decrypt the payload out of the minimed message and extract the status
				{
			// all clear. lets download the data
			answer.clear();					//not necessary but we are anal
			answer = sendMessage(minimedCommands.READ_HISTORY_REQUEST, payload);
			if (answer.empty())
				return 1;
			if (!hlp.checkAnswer(answer, 0x81)) {
				LOG_F(WARNING, "Answer was not 0x81. This should not happen!");
				//FIXME: Error handling!
				//FIXME: Throw exception?
				return 1;
			}
			bool downloadComplete = false;
			int packetCount = 0;
			std::vector<unsigned char> packet;
			while (!downloadComplete) {
				// get the data and check for 0x080 message
				answer = saveRead();
				if (answer.empty())
					return 1;
				answer = bmsg.decode(answer); //get the payload out of the binary message
				answer = mmsg.decodeMessage(answer); // get the payload out of the minimed message
				std::vector<unsigned char> decoded = smrq.decodeMessage(usbDevice, answer); // decrypt the payload out of the minimed message

				int response = hlp.extractCommand(decoded);
				if (response == minimedCommands.HIGH_SPEED_MODE_COMMAND) {
					LOG_F(5, "High Speed Command");
					continue;
				}
				if (response == minimedCommands.INITIATE_MULTIPACKET_TRANSFER) { // lets start with the multipacket transfer
					LOG_F(5, "Multi packet transfer initiate.  ");
					smrq.initiateMultiPacketTransfer(usbDevice, decoded, &pumpHistory);
					LOG_F(5, "Segment size		: %d ", pumpHistory.multiPacketSegmentSize);
					LOG_F(5, "Packet size		: %d ", pumpHistory.multiPacketPacketSize);
					LOG_F(5, "Last packet size	: %d ", pumpHistory.multiPacketLastPacketSize);
					LOG_F(5, "Packet count		: %d ", pumpHistory.multiPacketPacketToFetch);
					payload.clear();
					payload.push_back(*((char*) &minimedCommands.INITIATE_MULTIPACKET_TRANSFER + 1)); // the response paylod is: initiate multipacket transfer
					payload.push_back(*((char*) &minimedCommands.INITIATE_MULTIPACKET_TRANSFER + 0));

					answer = sendMessage(minimedCommands.ACK_MULTIPACKET_COMMAND, payload); // send the response
					if (answer.empty())
						return 1;
					if (!hlp.checkAnswer(answer, 0x81)) {
						LOG_F(WARNING, "Answer was not 0x81. This should not happen!");
					}

					continue;
				}
				if (response == minimedCommands.MULTIPACKET_SEGMENT_TRANSMISSION) { // we receive a packet. process it

					LOG_F(5, "Multi packet segment received. Message ID: %d", packetCount);
					packet.insert(packet.end(), decoded.begin() + 5, decoded.end());
					packetCount++;
					//FIXME: check packet size
					//FIXME: check last packet size
					//FIXME: check packetcount

				}

				if (packetCount == pumpHistory.multiPacketPacketToFetch) {
					LOG_F(5, "Last packet received. requesting next segment");
					pumpHistory.segments.push_back(packet);
					packet.clear();
					packetCount = 0;
					// FIXME: ERROR HANDLING!!!

					payload.clear();
					payload.push_back(*((char*) &minimedCommands.MULTIPACKET_SEGMENT_TRANSMISSION + 1));	// ask for the next segment
					payload.push_back(*((char*) &minimedCommands.MULTIPACKET_SEGMENT_TRANSMISSION + 0));
					answer = sendMessage(minimedCommands.ACK_MULTIPACKET_COMMAND, payload);
					if (answer.empty())
						return 1;

					if (!hlp.checkAnswer(answer, 0x81)) {
						LOG_F(WARNING, "Answer was not 0x81. This should not happen!");
					}

					continue;

				}
				if (response == minimedCommands.END_HISTORY_TRANSMISSION) {	// end of transmission, exit!
					// FIXME ERROR HANDLING
					LOG_F(5, "Multi packet end transmission received. Segment count: %d", packetCount);
					downloadComplete = true;
					saveRead();
					continue;

				}
				if (packetCount % 5 == 0) {	// prompt every 5 packages that we are still working
					LOG_F(1, "History: Received %d packets", packetCount);

				}

			}
		}
	}
	return 0;
}

int PumpDriver::processHistory(std::vector<unsigned char> history, HistoryData * hD) {
	static int BLOCK_SIZE = 2048;

	LOG_F(7, "Size of vector: %d", history.size());
	LOG_F(7, "vector: %s", hlp.toHexString(history).c_str());
	unsigned char type = history[2];
	unsigned int sizeCompressed = hlp.read32BEtoInt(history, 3);
	unsigned int sizeUnCompressed = hlp.read32BEtoInt(history, 7);
	unsigned char compressed = history[11];

	LOG_F(5, "Type (2 = Pump, 3 = Sensor)     : %d", hlp.charToShort(0, type));;
	LOG_F(5, "size UnCompressed               : %d", sizeUnCompressed);
	LOG_F(5, "size Compressed                 : %d", sizeCompressed);
	LOG_F(5, "Compressed?                     : %d", hlp.charToShort(0, compressed));

	// lzo decompression
	lzo_bytep out;
	out = (lzo_bytep) malloc(sizeUnCompressed);
	long unsigned int len;
	int r = lzo1x_decompress(&history[12], history.size() - 12, out, &len, NULL);
	if (r == LZO_E_OK && len == sizeUnCompressed) {

		LOG_F(5, "Decompressed %lu bytes to %lu bytes", (unsigned long ) sizeCompressed, (unsigned long ) len);
		std::vector<unsigned char> decompressed(out, out + len);
		LOG_F(7, "vector uncompressed: %s", hlp.toHexString(decompressed).c_str());
		free(out);
		// slice the blocks and check crc
		for (int i = 0; i < (int) len / BLOCK_SIZE; i++) {
			int blockSize = hlp.charToShort(decompressed[(i + 1) * BLOCK_SIZE - 4], decompressed[(i + 1) * BLOCK_SIZE - 3]);
			int blockCrc = hlp.charToShort(decompressed[(i + 1) * BLOCK_SIZE - 2], decompressed[(i + 1) * BLOCK_SIZE - 1]);
			auto first = decompressed.cbegin() + i * BLOCK_SIZE;
			auto last = decompressed.cbegin() + i * BLOCK_SIZE + blockSize;
			std::vector<unsigned char> buf;
			buf.assign(first, last);
			hD->historyBlock.push_back(buf);
			if ((hlp.CRC16CCITT(buf, 0, 0xFFFF, 0x1021, buf.size()) - blockCrc) != 0) {
				LOG_F(ERROR, "CRC Error - ignoring!");
				// FIXME: add error handler
			};
			hD->historyBlock.push_back(buf);
			buf.clear();
		}
	} else {
		LOG_F(ERROR, "Decompression Error!!!");
		return 1;
	}
	return 0;
}

int PumpDriver::sliceEvents(HistoryData * hD) {
	unsigned int pos = 0;
	for (unsigned int i = 0; i < hD->historyBlock.size(); i++) {
		LOG_F(7, "Blocks uncompressed: %s \n", hlp.toHexString(hD->historyBlock[i]).c_str());
		while (pos < (hD->historyBlock[i]).size()) {
			int size = (int) (hD->historyBlock[i]).at(2);
			auto first = (hD->historyBlock[i]).cbegin() + pos;
			auto last = (hD->historyBlock[i]).cbegin() + pos + size;
			std::vector<unsigned char> buf;
			buf.assign(first, last);
			hD->historyEvent.push_back(buf);
			pos += size;
		}
		pos = 0;
	}
	LOG_F(7, "processed %lu events \n", hD->historyEvent.size());
	return 0;
}

int PumpDriver::extractEvents(HistoryData * hD) {

	for (unsigned int i = 0; i < hD->historyEvent.size(); i++) {
		// FIXME it is possible to get "subevents" (MULTI_ITEM_HISTORY) - we ignore this
		MinimedEvent event = hD->processEvent((hD->historyEvent).at(i));
		if (event.type) {
			LOG_F(9, "------------------------------------------------------------------------\n");
			LOG_F(9, "#%d - Type: %x - Time:  %s", i, event.type, ctime(&event.eventTime));
			LOG_F(9, "minutes before reading         : %x ", event.minutesBeforeReading);
			LOG_F(9, "predicted sg / sg              : %d / %d ", event.predictedSG, event.sg);
			LOG_F(9, "isig, rate of change           : %2.2f , %2.2f ", event.isig, event.rateOfChange);
			LOG_F(9, "reading status / sensor status : %d / %d ", event.readingStatus, event.sensorStatus);
			hD->events.push_back(event);
		}
	}
	return 0;

}

