#include "log.h"

#if LOG_DEBUG
bool verbose = true;
#else
bool verbose = false;
#endif

void log_verbose_enable(bool enabled) {
    verbose = enabled;
}

bool log_verbose_is_enabled() {
    return verbose;
}
