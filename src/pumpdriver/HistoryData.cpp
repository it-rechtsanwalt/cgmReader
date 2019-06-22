/*============================================================================
 *  Name       : HistoryData.cpp
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



#include "HistoryData.h"

HistoryData::HistoryData() {
	// TODO Auto-generated constructor stub

}

HistoryData::~HistoryData() {
	// TODO Auto-generated destructor stub
}

// decodes the received time
time_t HistoryData::decodeTime(int t, int offset) {
	time_t now = time(0);			// time now in seconds
	struct std::tm *timeinfo = std::localtime(&now);
	int utcOffset = timeinfo->tm_gmtoff;
	int epoch = 946684800 + t + offset - utcOffset;			//+ utcdiff;
	if (epoch < 0)
		epoch = 0;
	return epoch;
}


MinimedEvent HistoryData::processEvent(std::vector<unsigned char> historyEvent) {

	unsigned char eventType = historyEvent.at(0);
//	unsigned int sizeCompressed = hlp.read32BEtoInt(historyEvent, 0x02);
	unsigned int ti = hlp.read32BEtoInt(historyEvent, 0x03);
	unsigned int offset = hlp.read32BEtoInt(historyEvent, 0x07);
	time_t t = decodeTime(ti, offset);
	MinimedEvent event = {0};
	event.type = eventType;
	event.eventTime = t;
	switch (eventType) {
	case 0xd6: {			// Sensor Glucose Event

		event.minutesBeforeReading = historyEvent.at(0x0b);
		event.predictedSG = hlp.charToShort(historyEvent.at(0xd), historyEvent.at(0x0e));
		event.sg = ((historyEvent.at(0x0f) & 0x03) << 8) | historyEvent.at(0x10);
		event.isig = hlp.charToShort(historyEvent.at(0x11),historyEvent.at(0x12)) / (float) 100;
		event.readingStatus = historyEvent.at(0x17);
		event.sensorStatus = historyEvent.at(0x16);
		event.rateOfChange = hlp.charToShort(historyEvent.at(0x14), historyEvent.at(0x15)) / (float) 100;
		break;
	}
	case 0x32: {
		event.eventTime = t;
		event.sg = hlp.charToShort(historyEvent.at(0xc), historyEvent.at(0x0d));

		break;
	}

	default:  {
		event.type = 0; // if the date is not processed, we return a "0" type. so we know: no data processed (yet)
		break;
	}
}
return event;
}
