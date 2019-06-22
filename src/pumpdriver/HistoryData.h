/*============================================================================
 *  Name       : HistoryData.h
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



#ifndef HISTORYDATA_H_
#define HISTORYDATA_H_

#include <vector>
#include <time.h>
#include "Hlp.h"

struct MinimedEvent {
	int type;
	time_t eventTime;
	int minutesBeforeReading;
	short predictedSG;
	int sg;
	float isig;
	int readingStatus;
	int sensorStatus;
	float rateOfChange;

};

class HistoryData {
public:
	HistoryData();
	virtual ~HistoryData();
	std::vector<std::vector<unsigned char>> historyBlock;
	std::vector<std::vector<unsigned char>> historyEvent;
	std::vector<MinimedEvent> events;
	MinimedEvent processEvent (std::vector<unsigned char> historyEvent);
	time_t decodeTime(int t, int offset);


private:
	Hlp hlp;

};

#endif /* HISTORYDATA_H_ */
