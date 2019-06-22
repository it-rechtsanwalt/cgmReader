/*============================================================================
 *  Name       : MinimedBinaryMessage.h
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



#ifndef MINIMEDBINARYMESSAGE_H_
#define MINIMEDBINARYMESSAGE_H_
#include "Hlp.h"
#include "UsbDevice.h"
#include "../utils/loguru.hpp"


class MinimedBinaryMessage {
public:
	MinimedBinaryMessage();
	virtual ~MinimedBinaryMessage();
	const std::vector <unsigned char> getMessage (UsbDevice *device, unsigned char type, unsigned char *payload, int payloadsize);
	const std::vector <unsigned char> decode (std::vector <unsigned char> msg);


	unsigned char type;


private:

	Hlp hlp;


};

#endif /* MINIMEDBINARYMESSAGE_H_ */


