#include "test.h"

#include "inet.h"

void main() {
  TEST( create_connection("localhost", 40000) < 0, "server can't be connected to" );

  int server_socket = create_server_socket(40000);
  TEST( server_socket >= 0, "server socket created and bound" );

  int client_socket = create_connection("localhost", 40000);
  TEST( client_socket >= 0, "server can be connected to" );

  char* to_write = "hello from the client!";
  TEST( send_string(client_socket, to_write) == 0, "client successfully sends a string to the server");
}
