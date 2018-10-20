#include "../badge_settings.h"

#ifdef TOKENIZER_OLD
#include "tokenizer_slow.c"
#endif
#ifndef TOKENIZER_OLD
#include "tokenizer_fast.c"
#endif