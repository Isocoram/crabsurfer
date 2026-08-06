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

//typedef enum { FALSE, TRUE } bool;

enum status { FAILED = -1, LISTEN_QUEUE_SIZE = 1 };

void serve(int socket_fd) {
  const char msg[] = "You've been served >:D\n";
  ssize_t send_status = send(socket_fd, msg, sizeof(msg), MSG_CONFIRM);
  if (send_status == FAILED) {
    return;
  }
}

void client_session(const char *ip, uint16_t port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == FAILED) return;

  struct sockaddr_in socketinfo = {0};

  if (inet_pton(AF_INET, ip, &socketinfo.sin_addr) <= 0) {
    return;
  }
  socketinfo.sin_family = AF_INET;
  socketinfo.sin_port = htons(port);

  int connection = connect(sock, (struct sockaddr *)&socketinfo, sizeof(socketinfo));

  if (connection == FAILED) return;
  else {
    char stream_data[BUFFERSIZE];
    while (1) {
      ssize_t receive_status = recv(sock, stream_data, sizeof(stream_data), 0);
      if (receive_status == FAILED) {
        perror("Cant Receive");
        return;
      };
      printf("server sent: %s", stream_data);
      goto stop;
    }
  }
  stop:
  return;
}

void server_session(uint16_t port) {
  printf("Server Started!");
  int server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock == FAILED) return;

  struct sockaddr_in server_socketinfo = {0};

  server_socketinfo.sin_family = AF_INET;
  server_socketinfo.sin_port = htons(port);
  server_socketinfo.sin_addr.s_addr = INADDR_ANY;

  int server_bind_status = bind(server_sock, (struct sockaddr *)&server_socketinfo, sizeof(server_socketinfo));

  int listen_status = listen(server_sock, LISTEN_QUEUE_SIZE);
  if (listen_status == FAILED) goto cleanup;

  while (1) {

    int client_socket = accept(server_sock, NULL, NULL);
    if (client_socket == FAILED) goto cleanup;

    serve(client_socket);
    close(client_socket);
  }

  cleanup:
  int closing_status = close(server_sock);
  return;
}


// use inet_pton() to make string to network address
int main(int argc, char **argv) {
  bool terminal_request, server_mode = false;
  int flag;
  char *server_ip, *destination_ip, *port, *file_path;
  server_ip = destination_ip = port = file_path = NULL;

  const char *help =
    "Usage: ./crabsurfer [flag] [ip] [port]\n"
    "flags:\n"
    "-h, help\n"
    "-s, server mode\n"
    "-c, client mode\n"
    "-t, terminal chat mode\n"
    "-f, file chat mode\n";

  while ((flag = getopt(argc, argv, "htf:sc:p:")) != -1) {
    switch (flag) {
      case 'h':
        printf("%s", help);
        break;
      case 't':
        terminal_request = true;
        return 1;
      case 's':
        server_mode = true;
        //if (optarg == NULL) break;
        //server_ip = strdup(optarg);
        //printf("Server IP: %s\n", server_ip);
        break;
      case 'c':
        destination_ip = strdup(optarg);
        printf("Destination IP: %s\n", destination_ip);
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
        printf("Not valid input.\n");
        return 1;
      }
    }

    if (server_mode && destination_ip) {
      printf("Cannot start as both server and client.");
      return 1;
    }


    if ((destination_ip) && !(port)) {
      printf("Port missing.\n");
      return 1;
    }

    if (file_path && terminal_request) {
      printf("Must choose between terminal and file mode.");
      return 1;
    }

    if ((server_mode || destination_ip) || port) {
      uint16_t port_number = (uint16_t)atoi(port);
      if (server_mode && port) {
        server_session(port_number);
      }

      if (destination_ip && port) {
        client_session(destination_ip, port_number);
        goto done;
      }

      else {
        goto error;
      }
    }

    error:
    printf("Unexpected behavior. I'm scared.");

  return 1;

  done:
  return 0;
}
