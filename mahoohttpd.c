/*
 mahoohttpd Copyright (C) <2016> <matthias holl>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>


 mahoohttpd.c

 small webserver

 by mah00

 www.mahoosoft.com

 linux:

 gcc -o mahoohttpd mahoohttpd.c

 sudo ./mahoohttpd

 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <strings.h>
#include <errno.h>
#define BUF 1000000

/*file pointer */
FILE *index_html;

char *ok_http_response = "HTTP/1.1 200 OK\n"
		"Server: mahoohttpd\n"
		"Accept-Ranges: bytes\n"
		"Connection: close\n"
		"Content-type: text/html\n"
		"\n";

int mahoohttpd_sock_rcv, mahoohttpd_sock_send;
struct sockaddr_in mahoohttpd_dest_address;
const int y = 1;
int addrlen = sizeof(struct sockaddr_in);
int i;
/* sizes of blocks that can be read from client */;
ssize_t size;

static void mahoosocket_set() {

	if ((mahoohttpd_sock_rcv = socket(AF_INET, SOCK_STREAM, 0)) > 0)
		printf("starting web server mahoohttpd ....OK   \n");
	setsockopt(mahoohttpd_sock_rcv, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	mahoohttpd_dest_address.sin_family = AF_INET;
	mahoohttpd_dest_address.sin_addr.s_addr = INADDR_ANY;
	/* server listening on port 80 */
	mahoohttpd_dest_address.sin_port = htons(80);
	/* bind socket to port */
	if (bind(mahoohttpd_sock_rcv, (struct sockaddr *) &mahoohttpd_dest_address,
			sizeof(mahoohttpd_dest_address)) != 0) {
		printf("port busy\n");
	}

}

static void mahoosocket_listen() {
	/* listen on socket */
	listen(mahoohttpd_sock_rcv, 5);

}

void mahooaccept_requests() {
	socklen_t addrlen;
	/* accept requests */
	mahoohttpd_sock_send = accept(mahoohttpd_sock_rcv,

	(struct sockaddr *) &mahoohttpd_dest_address, &addrlen);
}

int main(void) {
	char buffer_html[1000000];

	/* char pointer */
	char *buffer = malloc(BUF);

	/* set index.html from document root  */
	index_html = fopen("html/index.html", "r");

	/* read index.html */
	fread(&buffer_html, sizeof(int), 100, index_html);

	/* set socket */
	mahoosocket_set();

	/* listen on socket */
	mahoosocket_listen();

	/* server loop */
	while (1) {

		/* accept requests*/
		mahooaccept_requests();
		if (mahoohttpd_sock_send > 0)

			/* http status 200 OK */
			buffer = ok_http_response;

		/* if client connected print message and client IP */
		size = recv(mahoohttpd_sock_rcv, buffer, BUF - 1, 0);
		if (size > 0)
			buffer[size] = '\0';
		printf("client connected: IP (%s) \n",
				inet_ntoa(mahoohttpd_dest_address.sin_addr));

		/* send http status 200 OK */
		send(mahoohttpd_sock_send, buffer, strlen(buffer), 0);

		/* html buffer */
		buffer = buffer_html;

		/* send via socket */
		send(mahoohttpd_sock_send, buffer, strlen(buffer), 0);

		/* close socket */
		close(mahoohttpd_sock_send);
	}
	/* close socket */
	close(mahoohttpd_sock_rcv);
	return EXIT_SUCCESS;
}

