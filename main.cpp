#include <iostream>
#include "Fiber.h"


void f(void) {
    static int x;
    int i, id;

    id = ++x;
    for (i = 0; i < 10; i++) {
        printf("%d %d\n", id, i);
        Fiber::yield();
    }
}

int main(void) {
    Fiber::init();

    Fiber::run(f);
    Fiber::run(f);

    Fiber::wait();
}
