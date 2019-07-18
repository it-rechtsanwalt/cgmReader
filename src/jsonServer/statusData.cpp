/*============================================================================
 *  Name       : statusData.cpp
 *  Project    : jsonServerTest
 *  Author     : compuholic - itra.at
 *  created    : Jun 28, 2019
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

#include "statusData.h"

StatusData::StatusData() {
	// TODO Auto-generated constructor stub

}

StatusData::~StatusData() {
	// TODO Auto-generated destructor stub
}


void StatusData::initData(void) {
	ps.activeInsulin = 0;
	ps.sensorBGL = 0;


}

void StatusData::refresh(PumpStatus pumpStatus) {
	ps = pumpStatus;
}

std::string StatusData::getJson() {
nlohmann::json j = {
		  {"id",  "123" },
		  {"readStatus", 1},
//		  {"currentBasalRate", 1.3},
//		  {"tempBasalRate", 0},
//		  {"tempBasalPercentage", 50},
//		  {"tempBasalMinutesRemaining", 30},
//		  {"batteryLevelPercentage", 0x3},
		  {"insulinUnitsRemaining", 70},
		  {"activeInsulin", 1.5},
		  {"sensorBGL", ps.sensorBGL},
//		  {"readTime", 65461215},
//		  {"trendArrow", 0xe0},
//		  {"sensorBattery", 0x5},
//		  {"timeToCalibrate", 120},
//		  {"sensorStatus", 0},
		  {"message", "nothing to say2"}

		};
return j.dump();
}
