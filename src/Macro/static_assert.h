#pragma once

#define static_assert(COND,MSG) typedef char static_assert_##MSG[(COND)?1:-1]
