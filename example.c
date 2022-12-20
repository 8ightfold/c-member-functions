#include "member_function.h"
#include <stdio.h>

typedef struct S_ {
    const char* name;
    int i;
} S;

void add(S* val, int j) {
    printf("assigning %i to %s\n", j, val->name);
    val->i += j;
}

void print(S* val) {
    printf("%s value: %i\n", val->name, val->i);
}

$register(S, (void)(add)(S*, int), (void)(print)(S*));


int main() {
    $def(S, s) = {"test_obj", 5};
    $(s)->add(5);
    $(s)->print();
}
