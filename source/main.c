#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <3ds.h>

#define PORT	 8869
#define MAXLINE 1024
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("MemeDriver\n");

	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from client";
	struct sockaddr_in	 servaddr;
    // allocate buffer for SOC service
	SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);



	// Now intialise soc:u service
	int ret;
	if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
    	
	}
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}   

	memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.8");
    sendto(sockfd, (const char *)hello, strlen(hello),
		0, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();
        touchPosition touch;
        hidTouchRead(&touch);
        if (touch.py == 0 && touch.px == 0) {
            continue;
        }
        unsigned char bytes[4];
        bytes[0] = touch.px & 0xFF;
        bytes[1] = touch.px >> 8;
        bytes[2] = touch.py & 0xFF;
        bytes[3] = touch.py >> 8;
        sendto(sockfd, bytes, 4,
		0, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
        printf("\x1b[2;0H%03d; %03d", touch.px, touch.py);
	}

	gfxExit();
	return 0;
}
