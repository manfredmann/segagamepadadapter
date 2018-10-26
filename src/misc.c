#include "misc.h"

void debugf(const char * format, ... ) {
#ifdef DEBUG
  va_list args;
  va_start (args, format);
  vprintf(format, args);
  va_end (args);
#endif
}
