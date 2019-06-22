/*============================================================================
 *  Name       : USBDriver.cpp
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


#include "USBDriver.h"

#include "Hlp.h"

Hlp hlp;

USBDriver::USBDriver() {

}

USBDriver::~USBDriver() {
	// TODO Auto-generated destructor stub
}


int USBDriver::initialize(unsigned short vid, unsigned short pid,
		UsbDevice &usbDevice) {
	// Initialize the hidapi library
	res = hid_init();
	// Open the device using the VID, PID,
	// and optionally the Serial number.

	handle = hid_open(vid, pid, NULL);
	if (handle == 0) {
		return 1;
	}
	res = hid_get_manufacturer_string(handle, wstr, MAX_PAYLOAD_SIZE);
	LOG_F(INFO, "Found Stick: %S", wstr);

	res = hid_get_product_string(handle, wstr, MAX_PAYLOAD_SIZE);
	LOG_F(3, "Product String: %S", wstr);

	res = hid_get_serial_number_string(handle, wstr, MAX_PAYLOAD_SIZE);
	LOG_F(INFO, "Serial Number String: %S", wstr);


	std::wstring s;

	usbDevice.setStickSerial(hlp.wchartochar(wstr));

	return 0;
}

/**
 *   @brief closes an open usb device
 *
 */

void USBDriver::close_device() {
	if (handle) {
		hid_close(handle);

		LOG_F(INFO, "Closing Device...");
		hid_exit();
		handle = 0;
	}
}

/**  @brief sends a buffer to the stick
 *   the function divides the buffer in the right stick size. the
 *   header is added automatically
 *   TODO there are only two frames possible (now). are more necessary?
 *   TODO check for sent bytes and do some error handling.
 *
 *   @param buffer the buffer to send
 *   @param size the size to send
 *
 */
void USBDriver::send(unsigned char *buffer, unsigned int size) {
	if (size <= 60) {
		write(buffer, size);
	} else {
		// FIXME for simplicity reasons we just allow 2 frames - are more frames necessary?
		unsigned char buf1[60];
		std::memcpy(buf1, buffer, 60);
		write(buf1, 60);
		std::memcpy(buf1, &buffer[60], size - 60);
		write(buf1, size - 60);

	}

}

void USBDriver::write(unsigned char *buffer, unsigned int size) {
	std::vector<unsigned char> writebuffer { };
	writebuffer.push_back(0x41);
	writebuffer.push_back(0x42);
	writebuffer.push_back(0x43);
	writebuffer.push_back(size);
	writebuffer.insert(writebuffer.end(), &buffer[0], &buffer[size]);
	LOG_F(5, "<-- Send %d bytes: %s", writebuffer.size(),
			hlp.toHexString(writebuffer).c_str());
	unsigned int val = hid_write(handle, writebuffer.data(),
			writebuffer.size());
	if (val != writebuffer.size()) {
		LOG_F(ERROR, "Error! Just sent %d byte should be %d...", val,
				writebuffer.size());
		//FIXME throw exception?
	}
	writebuffer.clear();

}

/**  @brief sends a command (without size-byte!) to the stick
 *
 *   @param buffer the buffer to send
 *   @param size the size to send
 *
 */
void USBDriver::sendCommand(unsigned char *buffer, unsigned int size) {
	std::vector<unsigned char> writebuffer { };
	writebuffer.push_back(0x41);
	writebuffer.push_back(0x42);
	writebuffer.push_back(0x43);
	writebuffer.insert(writebuffer.end(), &buffer[0], &buffer[size]);
	LOG_F(5, "<-- Send %d bytes: %s", writebuffer.size(),
			hlp.toHexString(writebuffer).c_str());
	unsigned int val = hid_write(handle, writebuffer.data(),
			writebuffer.size());
	if (val != writebuffer.size()) {
		LOG_F(ERROR, "Error! Just sent %d byte should be %d...", val,
				writebuffer.size());
		//FIXME throw exception?
	}
	writebuffer.clear();

}

std::vector<unsigned char> USBDriver::read() {
	std::vector<unsigned char> buf;
	int received = 0;
	unsigned char receive_buffer[MAX_RECEIVE_BUFFER_SIZE] = { 0 };

	int val = hid_read_timeout(handle, receive_buffer, MAX_PAYLOAD_SIZE, 1000);
	if (val == -1 || val == 0) {
		LOG_F(7, "Nothing received. Returning");
		return {};
	}

	val = check_valid_response(receive_buffer);
	if (val == 0) { //if we do not receive a good header oder nothing (0) return nothing.
		return {};
	}
	received = val;
	buf.insert(buf.end(), &receive_buffer[4],	// otherwise insert to buffer
			&receive_buffer[val + 4]);
	LOG_F(9, "--> Read raw (%d bytes): %s", val, receive_buffer);

	while (true) {
		if (val != 60) {	// if we exceed the length: read another buffer
			break;
		}

		hid_read_timeout(handle, receive_buffer, MAX_PAYLOAD_SIZE, 1000);
		LOG_F(9, "--> Read raw (%d bytes): %s", val, receive_buffer);
		val = check_valid_response(receive_buffer);
		if (val == 0) {
			return buf;
		}
		received += val;
		buf.insert(buf.end(), &receive_buffer[4], &receive_buffer[val + 4]);

	}
	LOG_F(5, "--> Read %d bytes: %s", received, hlp.toHexString(buf).c_str());
	return buf;
}

unsigned int USBDriver::check_valid_response(unsigned char *receive_buffer) {
	if (receive_buffer[0] == 'A' && receive_buffer[1] == 'B'
			&& receive_buffer[2] == 'C') {

		return receive_buffer[3];
	}
	LOG_F(ERROR, "Invalid header");
	//FIXME add exception here!
	return 0;
}


/**
 * @desc resets the device
 * @return 0 success,  LIBUSB_ERROR_NOT_FOUND disconnected, -999 not initialized
 *
 *
 */
int USBDriver::resetDevice(unsigned short int vid, unsigned short int pid) {
	libusb_device * usbStick =NULL;
	int result;
	libusb_device_handle * handle;

	usbStick = getPort(vid, pid, usbStick);
//	if (result < 0) {
//			return result;
//		}
	result = libusb_open(usbStick, &handle);

	if (result != 0) {
		return result;
	}

	result = libusb_reset_device(handle);
	if (result != 0){
		return result;
	}
	//if (handle) libusb_close(handle);
	LOG_F(5, "Stick software reset done");

	return result;
}
libusb_device * USBDriver::getPort(unsigned short int vid, unsigned short int pid, libusb_device * stickDev) {

	libusb_device **devs;
	ssize_t cnt;

	int r = libusb_init(NULL);
	if (r < 0) {
		return NULL;
	}
	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0) {
		libusb_exit(NULL);
		return NULL;
	}

	libusb_device *dev;
	int i = 0;
	uint8_t path[8];

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			LOG_F(ERROR, "Failed to get file desc.");
		}
		if (desc.idVendor == vid && desc.idProduct == pid) {
			r = libusb_get_port_numbers(dev, path, sizeof(path));
			if (r > 0) {
				LOG_F(5,"Found %04x:%04x (bus %d, device %d) @ port %d", desc.idVendor, desc.idProduct,
						libusb_get_bus_number(dev), libusb_get_device_address(dev), path [0]);
			}

			break;
		}
	}

	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
	return dev;

}

