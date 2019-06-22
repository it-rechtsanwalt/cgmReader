/*============================================================================
 *  Name       : MimimedMessage.h
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



#ifndef MIMIMEDMESSAGE_H_
#define MIMIMEDMESSAGE_H_

#include <vector>

#include "Hlp.h"
#include "UsbDevice.h"
#include "../utils/loguru.hpp"

class MimimedMessage {
public:
	MimimedMessage();
	virtual ~MimimedMessage();
	std::vector<unsigned char> channelNegotiationMessage(unsigned char channel, unsigned long long linkMAC, unsigned long long pumpMAC);
	std::vector<unsigned char> buildMessage(unsigned char command, std::vector<unsigned char> payload);
	const std::vector<unsigned char> decodeMessage(std::vector<unsigned char> payload);

private:
	Hlp hlp;
	int CRC_SIZE = 2;
	unsigned char command;
	std::vector<unsigned char> payload;

	int ENCRYPTED_ENVELOPE_SIZE = 2;


};

#endif /* MIMIMEDMESSAGE_H_ */
