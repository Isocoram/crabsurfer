#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>      // for htons
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>      // for ssize_t
#include <stddef.h>         // for size_t
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define PAGESIZE 4096

typedef struct llblock {
  char buffer[PAGESIZE];
  struct llblock *previous;
  struct llblock *next;
  size_t index;
} llblock_t;

enum status { SUCCESS = 0, FAILED = -1, LISTEN_QUEUE_SIZE = 1 };


inline static void initialize_block(llblock_t *block) {
  for (size_t i = 0; i < PAGESIZE; i++) {
    block->buffer[i] = '\0';
  }
  block->previous = NULL;
  block->next = NULL;
  block->index = 0;
}

llblock_t *create_block(void) {
  llblock_t *block = (llblock_t *)malloc(sizeof(llblock_t));
  initialize_block(block);
  return block;
}

llblock_t *append_block(llblock_t *head, llblock_t *block) {
  llblock_t *tail = head;
  while (tail->next) {
    tail = tail->next;
  }
  tail->next = block;
  block->previous = tail;
  tail = block;
  tail->index = tail->previous->index + 1;
  return tail;
}

llblock_t *prepend_block(llblock_t *head, llblock_t *block) {
  block->previous = NULL;
  block->next = head;
  block->index = 0;
  llblock_t *temp = block;
  while (temp->next != NULL) {
    temp->next->index = temp->index + 1;
    temp = temp->next;
  }
  return block;
}

char get_single_char(void) {
  char text[1];
  fgets(text, sizeof(text), stdin);
  return text[0];
}

char *get_current_time(void) {
  time_t time_ = time(NULL);
  return ctime(&time_);
}

void memflood_char(char *buffer, size_t size, char character) {
  for (size_t i = 0; i < size; i++) {
    buffer[i] = character;
  }
  return;
}

int compare_strings_strict(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s2 != '\0') {
    char character1 = *(s1++);
    char character2 = *(s2++);
    if (character1 != character2) return 0;
  }
  return 1;
}

void serve(int socket_fd) {
  llblock_t *head = create_block();
  read(0, head->buffer, sizeof(head->buffer));
  printf("-> To Client: %s\n", get_current_time());
  fflush(stdout);
  if (compare_strings_strict(head->buffer, "killy")) {
    free(head);
    exit(0);
  }
  if ()
  ssize_t send_status = send(socket_fd, head->buffer, sizeof(head->buffer), MSG_DONTWAIT);
  if (send_status == FAILED) {
    free(head);
    return;
  }
}

void client_session(const char *ip, uint16_t port) {
  next_iteration:
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
    char stream_data[PAGESIZE];
    while (1) {
      ssize_t receive_status = recv(sock, stream_data, sizeof(stream_data), 0);
      if (receive_status == FAILED) {
        return;
      };
      printf("%s-> By Server: %s\n", stream_data, get_current_time());
      goto out;
    }
  }
  //char character = get_single_char();
  //if (character != 's') goto next_iteration;
  out:
  close(sock);
  goto next_iteration;
  return;
}

void server_session(uint16_t port) {
  int server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock == FAILED) return;

  struct sockaddr_in server_socketinfo;

  server_socketinfo.sin_family = AF_INET;
  server_socketinfo.sin_port = htons(port);
  server_socketinfo.sin_addr.s_addr = INADDR_ANY;

  bind_to_socket:
  int server_bind_status = bind(server_sock, (struct sockaddr *)&server_socketinfo, sizeof(server_socketinfo));
  if (server_bind_status == FAILED) goto bind_to_socket;

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
  if (closing_status != SUCCESS) return;
  return;
}


// use inet_pton() to make string to network address
int main(int argc, char **argv) {
  bool terminal_request = false, server_mode = false;
  int flag;
  char *destination_ip, *port, *file_path, *byte_count;
  destination_ip = port = file_path = NULL;

  const char *help =
    "Usage:\n"
    "Server  ./crabsurfer -s <port>\n"
    "Client  ./crabsurfer -c <destination ip> -p <destination port>\n"
    "\nType 'killy' to end hosting the server.\n\n"
    "Flags:\n"
    "-h  help\n"
    "-s  specify server mode\n"
    "-c  specify client mode\n"
    "-p  specify port\n"
    "-t  terminal chat mode\n"
    "-f  file chat mode\n"
    "-b  specify max byte count (standard 4096)\n";

  while ((flag = getopt(argc, argv, "htsbf:c:p:")) != -1) {
    switch (flag) {
      case 'h':
        printf("%s", help);
        goto done;
        break;
      case 't':
        terminal_request = true;
        return 1;
      case 's':
        server_mode = true;
        break;
      case 'c':
        destination_ip = strdup(optarg);
        //printf("Destination IP: %s\n", destination_ip);
        break;
      case 'p':
        port = strdup(optarg);
        //printf("Port: %s\n", port);
        break;
      case 'f':
        file_path = strdup(optarg);
        printf("File Path: %s\n", file_path);
        break;
      case 'b':
        byte_count = strdup(optarg);
        size_t new_byte_count = strtoul(byte_count, NULL, 10);
        printf("Byte Count: %ld\n", new_byte_count);
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

    if ((server_mode || destination_ip) && (port != NULL)) {
      uint16_t port_number = (uint16_t)atoi(port);
      if (server_mode && port) {
        server_session(port_number);
      }

      if (destination_ip && port) {
        //printf("Press [s] to stop! %s\n", get_current_time());
        //fflush(stdout);
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
