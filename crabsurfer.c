#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>      // for htons
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>      // for ssize_t
#include <stddef.h>     // for size_t
#include <unistd.h>
#include <string.h>

#define BUFFERSIZE 4096

enum status { FAILED = -1, LISTEN_QUEUE_SIZE = 1 };

void client_session(uint16_t port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == FAILED) return;
  struct sockaddr_in socketinfo;
  socketinfo.sin_family = AF_INET;
  socketinfo.sin_port = htons(port);
  socketinfo.sin_addr.s_addr = INADDR_ANY;
  int connection = connect(sock, (struct sockaddr *)&socketinfo, sizeof(socketinfo));
  if (connection == FAILED) return;
  else {
    char stream_data[BUFFERSIZE];
    ssize_t receive_status = recv(sock, stream_data, sizeof(stream_data), 0);
    if (receive_status == FAILED) return;
    printf("Received message: %s", stream_data);
  }
}

void server_session(uint16_t port) {
  int server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock == FAILED) return;
  struct sockaddr_in server_socketinfo;
  server_socketinfo.sin_family = AF_INET;
  server_socketinfo.sin_port = htons(port);
  server_socketinfo.sin_addr.s_addr = INADDR_ANY;
  int server_bind_status = bind(server_sock, (struct sockaddr *)&server_socketinfo, sizeof(server_socketinfo));
  int listen_status = listen(server_sock, LISTEN_QUEUE_SIZE);
  if (listen_status == FAILED) return;
  int client_socket = accept(server_sock, NULL, NULL);
  if (client_socket == FAILED) return;
}


// use inet_pton() to make string to network address
int main(int argc, char **argv) {
  int flag;
  char *server_ip, *client_ip, *port, *file_path;
  server_ip = client_ip = port = file_path = NULL;

  const char *help =
    "Usage: ./crabsurfer [flag] [ip] [port]\n"
    "flags:\n"
    "-h, help\n"
    "-s, server mode\n"
    "-c, client mode\n"
    "-t, terminal chat mode\n"
    "-f, file chat mode\n";

  while ((flag = getopt(argc, argv, "htf:s:c:p:")) != -1) {
    switch (flag) {
      case 'h':
        printf("%s", help);
        break;
      case 't':
        //  implement terminal chat interface
        return 1;
      case 's':
        server_ip = strdup(optarg);
        printf("Server IP: %s\n", server_ip);
        break;
      case 'c':
        client_ip = strdup(optarg);
        printf("Client IP: %s\n", client_ip);
        break;
      case 'p':
        port = strdup(optarg);
        printf("Port: %s\n", port);
        break;
      case 'f':
        file_path = strdup(optarg);
        printf("File Path: %s\n", file_path);
        break;
      default:
        printf("No, this isnt recognized...\n");
        return 1;
      }
    }

    if (server_ip && client_ip) {
      printf("Cannot start as both server and client.");
      return 1;
    }

    qm

  return 1;
}
