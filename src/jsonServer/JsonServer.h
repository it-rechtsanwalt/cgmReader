/*============================================================================
 *  Name       : jsonServer.h
 *  Project    : cgmReader
 *  Author     : compuholic - itra.at
 *  created    : Jun 27, 2019
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

#ifndef JSONSERVER_H_
#define JSONSERVER_H_

#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "statusData.h"
#include <netinet/tcp.h>

/*
 *
 */
class JsonServer {
public:
	JsonServer();
	virtual ~JsonServer();
	int startServer(std::string id, int port, StatusData * sdata);

private:
	struct sockaddr_in server;
	int sock = 0, fd;
	int reply(int client_socket);
	int check_data_vality(char data[]);
	std::string extractCommand (char data []);
	std::string key;
	StatusData * sd;
};

#endif /* JSONSERVER_H_ */
