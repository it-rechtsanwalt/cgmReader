/*============================================================================
 *  Name       : MimimedMessage.cpp
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


#include "MimimedMessage.h"

MimimedMessage::MimimedMessage() {

}

MimimedMessage::~MimimedMessage() {
	// TODO Auto-generated destructor stub
}

std::vector<unsigned char> MimimedMessage::buildMessage(unsigned char command, std::vector<unsigned char> payload) {
	std::vector<unsigned char> buf;
	buf.push_back(command);
	buf.push_back(payload.size() + 2);
	buf.insert(buf.end(), &payload[0], &payload[payload.size()]);
	long l = hlp.CRC16CCITT(buf, 0, 0xFFFF, 0x1021,
			ENCRYPTED_ENVELOPE_SIZE + (int) payload.size());
	unsigned char c[4];
	hlp.longtocharLE(l, &c[0]);
	buf.push_back(c[0]);
	buf.push_back(c[1]);


	return buf;
}


const std::vector<unsigned char> MimimedMessage::decodeMessage(std::vector<unsigned char> payload)  {
		unsigned char c1 = payload [payload.size()-1];
		unsigned char c2 = payload [payload.size()-2];
		unsigned short crc = c1 << 8;
		crc += c2;

		int calcCrc = hlp.CRC16CCITT(payload, 0,  0xFFFF, 0x1021, (int) payload.size()-2);

		if (crc != calcCrc) {
			LOG_F(WARNING, "Warning: CRC error");
			//TODO Sometimes the crc fails, even when the message is correct decoded???
//			throw new MinimedMessageException("CRC Error in Minimed Message. CRC should be "
//					+ (message[message.length - 1] & 0xff) + (message[message.length - 2] & 0xff));
		}
			// we do not need the first 2 bytes....
		// we do not need the last 2 bytes....

		std::vector<unsigned char> vec;
		vec.insert(vec.begin(),&payload[2],&payload[payload.size()-2]);
		return vec;
	}









/**
 * Negotiate the channel - special message
 *
 * @param channel - the channel id: 0x0e - Channel 14 - 2420MHz 0x11 - Channel
 *                17 - 2435MHz 0x14 - Channel 20 - 2450MHz 0x17 - Channel 23 -
 *                2465MHz 0x1a - Channel 26 - 2480MHz
 * @param linkMAC
 * @param pumpMAC
 * @return
 * @throws MinimedMessageException
 */

std::vector<unsigned char> MimimedMessage::channelNegotiationMessage(
		unsigned char channel, unsigned long long linkMAC,
		unsigned long long pumpMAC) {
	std::vector<unsigned char> buf;
	buf.push_back(0x01); // byte 0 (always 0x01)
	buf.push_back(channel); // byte 1 the channel
	buf.push_back(0x00); // byte 2 unknown
	buf.push_back(0x00); // byte 3 unknown
	buf.push_back(0x00); // byte 4 unknown
	buf.push_back(0x07); // byte 5 unknown
	buf.push_back(0x07); // byte 6 unknown
	buf.push_back(0x00); // byte 7 unknown
	buf.push_back(0x00); // byte 8 unknown
	buf.push_back(0x02); // byte 9 unknown

	unsigned char lc[8];
	// this is ugly but vector.insert does not insert the whole array if the last member is 0x0...

	hlp.longlongtocharLE(linkMAC, &lc[0]); // byte 10 to 17 the linkMAC little endian
	buf.push_back(lc[0]);
	buf.push_back(lc[1]);
	buf.push_back(lc[2]);
	buf.push_back(lc[3]);
	buf.push_back(lc[4]);
	buf.push_back(lc[5]);
	buf.push_back(lc[6]);
	buf.push_back(lc[7]);
	hlp.longlongtocharLE(pumpMAC, &lc[0]); // byte 18 to 25 the pumpMAC little endian
	buf.push_back(lc[0]);
	buf.push_back(lc[1]);
	buf.push_back(lc[2]);
	buf.push_back(lc[3]);
	buf.push_back(lc[4]);
	buf.push_back(lc[5]);
	buf.push_back(lc[6]);
	buf.push_back(lc[7]);

	return buf;

}
