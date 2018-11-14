#include "exception.h"
#include "sll.h"

int plus_one(int x) {
        EXCEPTION("test exception");
        return x + 1;
}
