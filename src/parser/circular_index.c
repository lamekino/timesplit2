#include "types/song.h"
#include "parser/circular_index.h"

Song *
circular_index(Parsed *parsed, size_t idx) {
    return parsed->elems[idx % parsed->count];
}
