#include <assert.h>

#ifdef DEBUG
#define DEBUG_ASSERT(p, msg) assert((p) && (msg))
#else
#define DEBUG_ASSERT(_1, _2) do {} while (0)
#endif
