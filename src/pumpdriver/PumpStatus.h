/*============================================================================
 *  Name       : PumpStatus.h
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


#ifndef PUMPSTATUS_H_
#define PUMPSTATUS_H_

//#include <string>
//#include "HistoryData.h"


struct PumpStatus {

	int readStatus ;
	float currentBasalRate ;
	float tempBasalRate ;
	int tempBasalPercentage ;
	int tempBasalMinutesRemaining;
	unsigned char batteryLevelPercentage;
	int insulinUnitsRemaining ;
	float activeInsulin ;
	int sensorBGL ;
	unsigned long readTime;

	int * bglVals;
	int valCount;

	/**
	 * Sensor Error or Calibrating = 0xe0 = 1110 0000
	 No arrows = 0x60 =                     0110 0000
	 3 arrows up = 0xc0 =                   1100 0000
	 2 arrows up = 0xa0 =                   1010 0000
	 1 arrow up = 0x80 =                    1000 0000
	 1 arrow down = 0x40 =                  0000 0100
	 2 arrows down = 0x20 =                 0010 0000
	 3 arrows down = 0x00 =                 0000 0000
	 */
	unsigned char trendArrow ;
	unsigned long long sensorBGLTimeStamp;
	bool recentBolusWizard;
	int bolusWizardBGL ;
	unsigned char sensorBattery ;

	/**
	 * @desc Status. Bit 1 = Suspended, Bit 2 = Bolusing, Bit 5 = Active (0 means inactive, rewound pump, etc), Bit 6 = Temp Basal active, Bit 7 = CGM active
	 *
	 */
	unsigned char pumpStatus ;
	unsigned short int timeToCalibrate ;

	/**
	 * @desc status
	 * -1 = not initialized
	 * 1 = calibrating
	 * 2 = calibration complete
	 * 4 = sgv unavailable (if sgv is expected and flag set then there will be a pump graph gap)
	 */
	unsigned char sensorStatus ;

//	std::string trendArrowString();

};
///*
// * OLD VERSION AS C++ CLASS
// *
// *
// * float currentBasalRate = -1;
//	float tempBasalRate = -1;
//	int tempBasalPercentage = -1;
//	int tempBasalMinutesRemaining = -1;
//	unsigned char batteryLevelPercentage = -1;
//	int insulinUnitsRemaining = -1;
//	float activeInsulin = -1;
//	int sensorBGL = -1;
//
//	/**
//	 * Sensor Error or Calibrating = 0xe0 = 1110 0000
//	 No arrows = 0x60 =                   0110 0000
//	 3 arrows up = 0xc0 =                 1100 0000
//	 2 arrows up = 0xa0 =                 1010 0000
//	 1 arrow up = 0x80 =                  1000 0000
//	 1 arrow down = 0x40 =                0000 0100
//	 2 arrows down = 0x20 =               0010 0000
//	 3 arrows down = 0x00 =               0000 0000
//	 */
//	unsigned char trendArrow = 0xe0;
//	unsigned long long sensorBGLTimeStamp = -1;
//	bool recentBolusWizard = false;
//	int bolusWizardBGL = -1;
//	unsigned char sensorBattery = -1;
//
//	/**
//	 * @desc Status. Bit 1 = Suspended, Bit 2 = Bolusing, Bit 5 = Active (0 means inactive, rewound pump, etc), Bit 6 = Temp Basal active, Bit 7 = CGM active
//	 *
//	 */
//	unsigned char pumpStatus = -1;
//	unsigned short int timeToCalibrate = -1;
//
//	/**
//	 * @desc status
//	 * -1 = not initialized
//	 * 1 = calibrating
//	 * 2 = calibration complete
//	 * 4 = sgv unavailable (if sgv is expected and flag set then there will be a pump graph gap)
//	 */
//	unsigned char sensorStatus = -1;
//
////	std::string trendArrowString();
// */

#endif /* PUMPSTATUS_H_ */


