/*============================================================================
 *  Name       : PumpStatus.cpp
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


#include "PumpStatus.h"



std::string PumpStatus::trendArrowString() {
		switch (trendArrow) {
		case 0x60:{
			return " ";	// no trend
		}
		case 0xc0:{
			return "UP UP UP ";		// 3 up
		}
		case 0xa0:{
			return "up up";  // 2 up
		}
		case 0x80:{
			return "up";   // up
		}
		case 0x40:{
			return "down ";  // down
		}
		case 0x20:{
			return "down";   // 2 down
		}
		case 0x00:{
			return "DOWN DOWN DOWN";   // 3 down
		}
		default: break;
		}
		return "not initialized";
	}

