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

void StatusData::refresh(PumpStatus pumpStatus, int result) {
	ps = pumpStatus;
	ps.readStatus = result;
	ps.readTime = time(0);

}

std::string StatusData::getJson() {
	if (ps.readStatus != 40) {
		// not a valid read: exit
		nlohmann::json j = { { "id", ps.stickSerial },  { "message", ps.message }

		};
//		  {"sensorStatus", 0},
//		  {"trendArrow", 0xe0},

		return j.dump();



	}

	if (ps.pumpStatus & (1 << 6)) {
//if (ps.sensorStatus == 0x00) {
		nlohmann::json j = { { "id", ps.stickSerial }, { "sensorBGL",
				ps.sensorBGL }, { "readStatus", ps.readStatus }, {
				"currentBasalRate", ps.currentBasalRate },
//		  {"tempBasalRate", },
//		  {"tempBasalPercentage", 50},
//		  {"tempBasalMinutesRemaining", 30},
				{ "batteryLevelPercentage", ps.batteryLevelPercentage }, {
						"insulinUnitsRemaining", ps.insulinUnitsRemaining }, {
						"activeInsulin", ps.activeInsulin },
//		  {"readTime", 65461215},
				{ "sensorBattery", ps.sensorBattery }, { "timeToCalibrate",
						ps.timeToCalibrate }, { "message", ps.message }

		};
//		  {"sensorStatus", 0},
//		  {"trendArrow", 0xe0},

		return j.dump();
	}
	return "";
}
