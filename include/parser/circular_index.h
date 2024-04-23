#ifndef CIRCULAR_INDEX_H
#define CIRCULAR_INDEX_H

#include "types/stack.h"
#include "types/song.h"

typedef const struct Stack Parsed;

Song *
circular_index(Parsed *parsed, size_t idx);


#endif /* CIRCULAR_INDEX_H */
