
#include <unistd.h>

char* color(char* color) {
  return isatty(STDOUT_FILENO) ? color : "";
}
