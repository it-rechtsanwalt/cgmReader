/*============================================================================
 *  Name       : PumpDriver.h
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



#ifndef PUMPDRIVER_H_
#define PUMPDRIVER_H_

#include "USBDriver.h"
#include "UsbDevice.h"

#include "MinimedBinaryMessage.h"
#include "MinimedSendMessageRequest.h"
#include "MimimedMessage.h"
#include "PumpHistory.h"
#include <unistd.h>

#include <string>
#include "Hlp.h"
#include <time.h>
#include <lzo/lzoconf.h>
#include <lzo/lzo1x.h>
#include "HistoryData.h"

class PumpDriver {
public:
	PumpDriver();
	virtual ~PumpDriver();
	void init();
	int resetStick(unsigned short int vid, unsigned short int pid);
	int getStickData();
	void openPumpConnection();
	void enterPassThroughMode();
	void readInfo();
	int channelNegotiation();
	int enteringEHSM();
	int getPumpTime();
	int downloadData(PumpStatus * ps);
	int downloadHistory (PumpStatus * ps,HistoryData * hD);

	void closeEHSM();
	void closeConnection();
	void exitPassthroughMode();
	void exitControlMode();
	void closeDriver();
	UsbDevice* usbDevice;
	USBDriver usbDriver;
	int downloadHistoryData (char type, int history, PumpStatus * ps);
	PumpHistory pumpHistory;
	int processHistory(std::vector<unsigned char> history, HistoryData * hD);
	int sliceEvents(HistoryData * hD);
	time_t decodeTime(long time) ;
	int extractEvents(HistoryData * hD);

private:
	std::vector<unsigned char> sendMessage (unsigned short command, std::vector<unsigned char> payload);
	std::vector<unsigned char> saveRead();


	Commands commands;
	MinimedCommands minimedCommands;
	Hlp hlp;
	unsigned int sleep_duration = 2000;
	int retries = 10;

};

#endif /* PUMPDRIVER_H_ */
