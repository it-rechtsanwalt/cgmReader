/*============================================================================
 *  Name       : jsonServer.cpp
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

#include "JsonServer.h"

#include "../utils/loguru.hpp"

#define RCVBUFSIZE 1024

JsonServer::JsonServer() {
	// TODO Auto-generated constructor stub

}

JsonServer::~JsonServer() {
	// TODO Auto-generated destructor stub
}

int JsonServer::check_data_vality(char * data) {
	std::string valid_header = "GET /ID=" + key;
	std::string answer(data);

	if (answer.compare(0, key.length() + 8, valid_header) == 0) {
		LOG_F(8, "VALID request, sending data.");
		return 1;
	} else {
		LOG_F(8, "INVALID request, ignoring.");
		return 0;
	}

	return 0;
}

std::string JsonServer::extractCommand(char * data) {
	std::string answer = "";
	return sd->getJson();  // change 1234 to the id of the pump

//	return answer;
}


int JsonServer::reply(int client_socket) {

	char echo_buffer[RCVBUFSIZE];
	int recv_size;
	std::string reply = "HTTP/1.1 403 Forbidden\n";




	if ((recv_size = recv(client_socket, echo_buffer, RCVBUFSIZE, 0)) < 0)
		LOG_F(ERROR, "Error: receiving Data");

	LOG_F(9, "answer: %s\n", echo_buffer);
	int answer = check_data_vality(echo_buffer);
	if (answer == 1) {
		reply = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n";
		reply += extractCommand(echo_buffer);
		reply += "\n";

		int sent_bytes = send(client_socket, reply.c_str(), reply.length(), 0);
		if (sent_bytes != (int) reply.length()) {
			LOG_F(ERROR, "Send error!");
			return 1;
		}
		LOG_F(8, "reply (Bytes sent: %d): \n%s", sent_bytes, reply.c_str());
	} else {
		if (send(client_socket, reply.c_str(), reply.length(), 0) != (int) reply.length()) {
			LOG_F(ERROR, "Send error!");
			return 1;
		}

	}

	return 0;
}

int JsonServer::startServer(std::string id, int port, StatusData * sdata) {

	struct sockaddr_in client;
	unsigned int len;
	key = id;
	sd = sdata;
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		LOG_F(ERROR, "Error creating Socket");
		return -1;
	}
	// set option to reuse the socket without delay
	int reuse = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof(reuse)) < 0) {
		LOG_F(ERROR, "setsockopt(SO_REUSEADDR) failed");
	}

//	// set option to reuse the socket without delay
//	int no_delay = 1;
//	if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &no_delay, sizeof(int)) < 0) {
//		LOG_F(ERROR, "setsockopt(TCP_NODELAY) failed");
//	}


	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	if (bind(sock, (struct sockaddr*) &server, sizeof(server)) < 0) {
		LOG_F(ERROR, "Could not bind");
		return -2;
	}
	if (listen(sock, 1) == -1) {
		LOG_F(ERROR, "Could not listen");
		return -3;
	}
	LOG_F(INFO, "Server started at %s:%d", server.sin_addr.s_addr, port);

	while (true) {
		len = sizeof(client);
		fd = accept(sock, (struct sockaddr*) &client, &len);
		if (fd < 0) {
			LOG_F(ERROR, "Error accepting client");
		}
		LOG_F(7, "Client from %s connected.", inet_ntoa(client.sin_addr));
		reply(fd);
		int repl = close(fd);
		if (repl == -1) {
			LOG_F(ERROR, "Error closing connection!");
		}
	}

	return 0;
}

