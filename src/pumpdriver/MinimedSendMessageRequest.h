/*============================================================================
 *  Name       : MinimedSendMessageRequest.h
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



#ifndef MINIMEDSENDMESSAGEREQUEST_H_
#define MINIMEDSENDMESSAGEREQUEST_H_

#include <vector>

#include "Hlp.h"
#include "UsbDevice.h"
#include "USBDriver.h"
#include "../utils/loguru.hpp"
#include "PumpHistory.h"

class MinimedSendMessageRequest {
public:
	MinimedSendMessageRequest();
	virtual ~MinimedSendMessageRequest();
	std::vector<unsigned char> getEncryptedMessage(UsbDevice *device, short cmd, std::vector<unsigned char> payload);
	std::vector<unsigned char> decodeMessage(UsbDevice *device, std::vector<unsigned char> payload);
	int decodeMessageAndStatus(UsbDevice *device, std::vector<unsigned char> payload, PumpStatus * ps);
	int decodePumpHistoryInfo(UsbDevice *device, std::vector<unsigned char> payload, PumpHistory * ps);
	int decodePumpHistory(UsbDevice *device, std::vector<unsigned char> payload, PumpHistory * ps);
	int initiateMultiPacketTransfer(UsbDevice *device, std::vector<unsigned char> payload, PumpHistory * ps);
	int processMultiPacketTransferMessage(UsbDevice *device, PumpHistory * ps);


private:
	Hlp hlp;
	MinimedCommands minimedCommands;
};

#endif /* MINIMEDSENDMESSAGEREQUEST_H_ */
