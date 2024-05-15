#include <time.h>

#include "test.h"
#include "inet.h"

#define BUFFER_SIZE 128

void main() {
  srand(time(NULL));

  int port = (rand() % 2000) + 40000;
  debug_printf("Port: %d", port);

  int result = 0;
  debug_printf("CLOCKS_PER_SEC: %ld", CLOCKS_PER_SEC);

  int client_socket = create_connection("127.0.0.1", port);
  TEST( client_socket >= 0, "client socket can be created" );
  time_t start = time(NULL);
  time_t now = start;
  while(now - start < 2 && result == 0) {
    result = is_connected(client_socket);
    now = time(NULL);
  }

  debug_printf("connect result: %d", result);
  TEST( result <= 0, "server can't be connected to" );

  int server_socket = create_server_socket(port);
  TEST( server_socket >= 0, "server socket created and bound" );

  TEST( accept_connection(server_socket) == -1, "accept_connection while no clients are connecting");

  client_socket = create_connection("127.0.0.1", port);
  TEST( client_socket >= 0, "client socket can be created" );
  result = 0;
  start = time(NULL);
  now = start;
  while(now - start < 1 && result == 0) {
    result = is_connected(client_socket);
    now = time(NULL);
  }

  debug_printf("connect result: %d", result);
  TEST( result == 1, "server can be connected to" );

  int server_fd_to_client = -1;
  start = time(NULL);
  now = start;
  while(now - start < 1 && server_fd_to_client == -1) {
    server_fd_to_client = accept_connection(server_socket);
    now = time(NULL);
  }
  TEST( server_fd_to_client >= 0, "accept_connection accepted a connection");

  char* to_write = "hello from the client!";
  TEST( send_string(client_socket, to_write) == 0, "client successfully sends a string to the server");

  struct receiving_buffer server_buffer = make_receive_buffer(BUFFER_SIZE);
  TEST( server_buffer.buffer != NULL, "a valid server side buffer is created");

  result = 0;
  start = time(NULL);
  now = start;
  while(now - start < 1 && result == 0) {
    result = read_buffer(server_fd_to_client, &server_buffer);
    now = time(NULL);
  }
  TEST( result != -1, "server didn't get an error while reading");
  TEST( result == strlen(to_write) + 1, "server recieved correct number of bytes");

  TEST( strcmp(server_buffer.buffer, to_write) == 0, "server recieved correct data");

  close(client_socket);
  
  result = 0;
  start = time(NULL);
  now = start;
  while(now - start < 4 && result == 0) {
    result = read_buffer(server_fd_to_client, &server_buffer);
    now = time(NULL);
  }
  debug_printf("read result: %d", result);
  TEST( read_buffer(server_fd_to_client, &server_buffer) < 1, "server read_buffer() after socket is closed");
  TEST( send_string(client_socket, to_write) == 1, "client fails to send a string after close()");
  TEST( send_string(server_fd_to_client, to_write) == 1, "server fails to send a string after close()");

  free_receiving_buffer(&server_buffer);

  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() after freeing memory");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() after freeing memory");
}
