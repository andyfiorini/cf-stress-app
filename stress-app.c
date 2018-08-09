#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BUFSIZE 2048

#define ENVMSG "Please set the PORT environment variable\n"
#define STARTMSG "Starting the crashy application...\n"

void stress_cpu() {
	for(;;) {}
	exit(253);
}

void stress_mem() {
	for(;;) {
		malloc(sizeof(int));
	}
	exit(253);
}
void stress_disk() {

	int mystressfile;
        char buffer[6];

	strncpy((char *)buffer, (const char *) "stress\0", (size_t) 6);
    
	mystressfile = open("stressfile.out", O_CREAT | O_WRONLY,S_IRUSR);
        if(mystressfile!=-1){
          printf("mystressfile was created.\n");
        }	
	for(;;) {
		write(mystressfile, buffer, (size_t) 6);
        }
	exit(253);
}

int main(int argc, char **argv) {

	char buffer[BUFSIZE];
	char protoname[] = "tcp";
	struct protoent *protoent;
	int newline_found = 0;
	int enable = 1;
	int server_sockfd, client_sockfd;
	socklen_t client_len;
	ssize_t nbytes_read;
	struct sockaddr_in client_address, server_address;
	/* unsigned short server_port = 12345u; */

	write(1, STARTMSG, strlen(STARTMSG));
	const char* PORT = getenv("PORT");
	   
        printf("PORT var value is %s.\n", PORT);
 
        protoent = getprotobyname(protoname);
        if (protoent == NULL) {
            perror("getprotobyname");
            exit(EXIT_FAILURE);
        }

        server_sockfd = socket(
            AF_INET,
            SOCK_STREAM,
            protoent->p_proto
            /* 0 */
        );

        if (server_sockfd == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
            perror("setsockopt(SO_REUSEADDR) failed");
            exit(EXIT_FAILURE);
        }

        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if ( PORT != NULL ) {
          server_address.sin_port = htons((short int)strtol(PORT, NULL, 10));
	} else {
          write(1, ENVMSG, strlen(ENVMSG));
	  exit(250);
	}
        if (bind(
                server_sockfd,
                (struct sockaddr*)&server_address,
                sizeof(server_address)
            ) == -1
        ) {
            perror("bind");
            exit(EXIT_FAILURE);
        }

        if (listen(server_sockfd, 5) == -1) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "listening on port %s\n", PORT);

        while (1) {
            client_len = sizeof(client_address);
            client_sockfd = accept(
                server_sockfd,
                (struct sockaddr*)&client_address,
                &client_len
            );

	    sprintf(buffer,(const char *)"HTTP/1.0 200 OK\r\nContent-Type: html\r\n\r\n");
	    write(client_sockfd, buffer, strlen(buffer));

            while ((nbytes_read = read(client_sockfd, buffer, BUFSIZE)) > 0) {
                write(STDOUT_FILENO, buffer, nbytes_read);
                if ((strstr(buffer, "cpu\r\n")) != NULL) {
                    stress_cpu();
                    exit(254);
                }
                if ((strstr(buffer, "mem\r\n")) != NULL) {
                    stress_mem();
                    exit(254);
                }
                if ((strstr(buffer, "disk\r\n")) != NULL) {
                    stress_disk();
                    exit(254);
                }
                if (buffer[nbytes_read - 1] == '\n')
                    newline_found=1;
		strcpy(buffer, "hello\n");
                write(client_sockfd, buffer, nbytes_read);
                if ((strstr(buffer, "Mozilla")) != NULL) {
                    close(client_sockfd);
		}
                if (newline_found) {
                    break;
                }
            }
            close(client_sockfd);
        }
        return EXIT_SUCCESS;  
}
