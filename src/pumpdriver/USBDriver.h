/*============================================================================
 *  Name       : USBDriver.h
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



#ifndef USBDRIVER_H_
#define USBDRIVER_H_

#include <hidapi/hidapi.h>
#include "../utils/loguru.hpp"
#include "UsbDevice.h"
#include <iostream>
#include <exception>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <libusb-1.0/libusb.h>


#include "Hlp.h"

#define MAX_PAYLOAD_SIZE 256 		  // max send buffer size in byte
#define MAX_RECEIVE_BUFFER_SIZE 256	 // max read buffer size in bytes
#define USB_READ_TIMEOUT 1000  // timeout to wait for read in millisec

class USBDriver {
public:
	USBDriver();
	virtual ~USBDriver();

	int initialize(unsigned short vid, unsigned short pid,
			UsbDevice &usbDevice);
	void send(unsigned char *buffer, unsigned int size);
	void sendCommand(unsigned char *buffer, unsigned int size);

	std::vector<unsigned char> read();


	void close_device();
	int resetDevice(unsigned short int vid, unsigned short int pid);
	hid_device *handle;
private:

	libusb_device * getPort (unsigned short vid, unsigned short pid, libusb_device * stick);
	int res;
	wchar_t wstr[MAX_PAYLOAD_SIZE];
	void write(unsigned char *buffer, unsigned int size);
	unsigned int check_valid_response(unsigned char *receive_buffer);



};

struct Consts {
	const unsigned char ACK;
	const unsigned char CAN;
	const unsigned char EOT;
	const unsigned char CR;
	const unsigned char ENQ;

	const unsigned char ETB;
	const unsigned char ETX;
	const unsigned char LF;
	const unsigned char NAK;
	const unsigned char STX;
	const unsigned char NUL;

	Consts() :
			ACK(0x06), CAN(0x18), EOT(0x04), CR(0x0D), ENQ(0x05), ETB(0x17), ETX(
					0x03), LF(0x0A), NAK(0x15), STX(0x02), NUL(0x00)

	{
	}
	;

};

struct Commands {
	const unsigned char OPEN_CONNECTION;
	const unsigned char CLOSE_CONNECTION;
	const unsigned char SEND_MESSAGE;
	const unsigned char READ_INFO;
	const unsigned char REQUEST_LINK_KEY;
	const unsigned char SEND_LINK_KEY;
	const unsigned char RECEIVE_MESSAGE;
	const unsigned char SEND_MESSAGE_RESPONSE;
	const unsigned char REQUEST_LINK_KEY_RESPONSE;

	Commands() :
			OPEN_CONNECTION(0x10), CLOSE_CONNECTION(0x11), SEND_MESSAGE(0x12), READ_INFO(
					0x14), REQUEST_LINK_KEY(0x16), SEND_LINK_KEY(0x17), RECEIVE_MESSAGE(
					0x80), SEND_MESSAGE_RESPONSE(0x81), REQUEST_LINK_KEY_RESPONSE(
					0x86)

	{
	}
	;

};

struct MinimedCommands {

	short int HIGH_SPEED_MODE_COMMAND;
	short int TIME_REQUEST;
	short int TIME_RESPONSE;
	short int READ_PUMP_STATUS_REQUEST;
	short int READ_PUMP_STATUS_RESPONSE;
	short int READ_BASAL_PATTERN_REQUEST;
	short int READ_BASAL_PATTERN_RESPONSE;
	short int READ_BOLUS_WIZARD_CARB_RATIOS_REQUEST;
	short int READ_BOLUS_WIZARD_CARB_RATIOS_RESPONSE;
	short int READ_BOLUS_WIZARD_SENSITIVITY_FACTORS_REQUEST;
	short int READ_BOLUS_WIZARD_SENSITIVITY_FACTORS_RESPONSE;
	short int READ_BOLUS_WIZARD_BG_TARGETS_REQUEST;
	short int READ_BOLUS_WIZARD_BG_TARGETS_RESPONSE;
	short int DEVICE_STRING_REQUEST;
	short int DEVICE_STRING_RESPONSE;
	short int DEVICE_CHARACTERISTICS_REQUEST;
	short int DEVICE_CHARACTERISTICS_RESPONSE;
	short int READ_HISTORY_REQUEST;
	short int READ_HISTORY_RESPONSE;
	short int END_HISTORY_TRANSMISSION;
	short int READ_HISTORY_INFO_REQUEST;
	short int READ_HISTORY_INFO_RESPONSE;
	short int UNMERGED_HISTORY_RESPONSE;
	short int INITIATE_MULTIPACKET_TRANSFER;
	short int MULTIPACKET_SEGMENT_TRANSMISSION;
	short int MULTIPACKET_RESEND_PACKETS;
	short int ACK_MULTIPACKET_COMMAND;
	short int NAK_COMMAND;
	short int BOLUSES_REQUEST;
	short int REMOTE_BOLUS_REQUEST;
	short int REQUEST_0x0124;
	short int REQUEST_0x0405;
	short int TEMP_BASAL_REQUEST;
	short int SUSPEND_RESUME_REQUEST;
	short int NGP_PARAMETER_REQUEST;

	MinimedCommands ():

	HIGH_SPEED_MODE_COMMAND ( 0x0412 ),
	TIME_REQUEST ( 0x0403 ),
	TIME_RESPONSE ( 0x0407 ),
	READ_PUMP_STATUS_REQUEST ( 0x0112 ),
	READ_PUMP_STATUS_RESPONSE ( 0x013C ),
	READ_BASAL_PATTERN_REQUEST ( 0x0116 ),
	READ_BASAL_PATTERN_RESPONSE ( 0x0123 ),
	READ_BOLUS_WIZARD_CARB_RATIOS_REQUEST ( 0x012B ),
	READ_BOLUS_WIZARD_CARB_RATIOS_RESPONSE ( 0x012C ),
	READ_BOLUS_WIZARD_SENSITIVITY_FACTORS_REQUEST ( 0x012E ),
	READ_BOLUS_WIZARD_SENSITIVITY_FACTORS_RESPONSE ( 0x012F),
	READ_BOLUS_WIZARD_BG_TARGETS_REQUEST ( 0x0131 ),
	READ_BOLUS_WIZARD_BG_TARGETS_RESPONSE ( 0x0132 ),
	DEVICE_STRING_REQUEST ( 0x013A ),
	DEVICE_STRING_RESPONSE ( 0x013B ),
	DEVICE_CHARACTERISTICS_REQUEST ( 0x0200 ),
	DEVICE_CHARACTERISTICS_RESPONSE ( 0x0201 ),
	READ_HISTORY_REQUEST ( 0x0304 ),
	READ_HISTORY_RESPONSE ( 0x0305 ),
	END_HISTORY_TRANSMISSION ( 0x030A ),
	READ_HISTORY_INFO_REQUEST ( 0x030C ),
	READ_HISTORY_INFO_RESPONSE ( 0x030D ),
	UNMERGED_HISTORY_RESPONSE ( 0x030E ),
	INITIATE_MULTIPACKET_TRANSFER ( 0xFF00 ),
	MULTIPACKET_SEGMENT_TRANSMISSION ( 0xFF01),
	MULTIPACKET_RESEND_PACKETS ( 0xFF02 ),
	ACK_MULTIPACKET_COMMAND ( 0x00FE ),
	NAK_COMMAND ( 0x00FF ),
	BOLUSES_REQUEST ( 0x0114 ),
	REMOTE_BOLUS_REQUEST ( 0x0100 ),
	REQUEST_0x0124 ( 0x0124 ),
	REQUEST_0x0405 ( 0x0405 ),
	TEMP_BASAL_REQUEST ( 0x0115 ),
	SUSPEND_RESUME_REQUEST ( 0x0107 ),
	NGP_PARAMETER_REQUEST ( 0x0138 )
	{};
};

#endif /* USBDRIVER_H_ */
