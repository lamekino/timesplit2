#pragma once

#include "Parser/parse_file.h"
#include "Parser/parse_stream.h"

#define PARSER_HIDE_IMPL \
    typedef char module_error[-1];
