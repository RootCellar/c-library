#ifndef TERMINAL_COLOR_H
#define TERMINAL_COLOR_H

char* color(char* color);

#define __COLOR(col) color(col)

#define ANSI_COLOR_RED     __COLOR("\x1b[31m")
#define ANSI_COLOR_GREEN   __COLOR("\x1b[32m")
#define ANSI_COLOR_YELLOW  __COLOR("\x1b[33m")
#define ANSI_COLOR_BLUE    __COLOR("\x1b[34m")
#define ANSI_COLOR_MAGENTA __COLOR("\x1b[35m")
#define ANSI_COLOR_CYAN    __COLOR("\x1b[36m")

#define ANSI_COLOR_GRAY           __COLOR("\x1b[90m")
#define ANSI_COLOR_BRIGHT_RED     __COLOR("\x1b[91m")
#define ANSI_COLOR_BRIGHT_GREEN   __COLOR("\x1b[92m")
#define ANSI_COLOR_BRIGHT_YELLOW  __COLOR("\x1b[93m")
#define ANSI_COLOR_BRIGHT_BLUE    __COLOR("\x1b[94m")
#define ANSI_COLOR_BRIGHT_MAGENTA __COLOR("\x1b[95m")
#define ANSI_COLOR_BRIGHT_CYAN    __COLOR("\x1b[96m")

#define ANSI_COLOR_RESET   __COLOR("\x1b[0m")

#endif
