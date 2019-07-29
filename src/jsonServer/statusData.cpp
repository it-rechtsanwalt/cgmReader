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
	//fixme: this is ugly!
	std::string msg;
	nlohmann::json js = { { "id", ps.stickSerial }, { "message", ps.message } };

	if (ps.readStatus != 40) {
		msg.append("no pump found.");
		// not a valid read: exit
		return js.dump();

	}
	// pumpstatus says: calibrating
	if (ps.pumpStatus & (1 << 6)) {
		if (ps.sensorStatus == 1) {
			js["sensorBGL"] = ps.sensorBGL;
			js["sensorBattery"] = ps.sensorBattery;
			js["trend"] = ps.trendArrowString().c_str();
			msg.append("Calibrating...(%d min remain)", ps.timeToCalibrate);

		}
		if ((ps.sensorStatus == 2)||(ps.sensorStatus == 0)) {
			js["sensorBGL"] = ps.sensorBGL;
			js["sensorBattery"] = ps.sensorBattery;
			js["trend"] = ps.trendArrowString().c_str();
			msg.append("sensor ok");
		    js["timeToCalibrate"] = ps.timeToCalibrate;
		}
		if (ps.sensorStatus == 4) {
			js["sensorBGL"] = 0;
			js["sensorBattery"] = ps.sensorBattery;
			js["trend"] = "-";
			msg.append("no sensor value available");
		    js["timeToCalibrate"] = ps.timeToCalibrate;
		}

	}
	if (ps.alarm != 0) {
		msg.append(" - ALARM!");
	}
	js["message"] = msg;
	js["readStatus"] = ps.readStatus;
	js["currentBasalRate"] = ps.currentBasalRate;
	js["batteryLevelPercentage"] = ps.batteryLevelPercentage;
	js["insulinUnitsRemaining"] = ps.insulinUnitsRemaining;
	js["activeInsulin"] = ps.activeInsulin;

	return js.dump();

}
