//
// Created by Gabriele Carrettoni on 28/05/16.
//

#include "Fiber.h"

Fiber *Fiber::current;
std::array<Fiber, Fiber::MAXFIBERS> Fiber::fibers;

void Fiber::init() {
    current = &fibers.at(0);
    current->status = Fiber::Ready;
}

bool Fiber::yield() {
    Fiber *f = current;
    for (auto *p = ++f ;; f++) {
        if (p == &fibers.at(MAXFIBERS-1)) {
            p = &fibers.at(0);

        } else if (p->status == Fiber::Ready) {
            f = p;
            break;
        }
        else if (p == f) {
            return false;
        }
    }

    if (current->status != Fiber::Unused) {
        current->status = Fiber::Ready;
    }

    f->status = Fiber::Running;
    auto old_ctx = &current->ctx;
    auto new_ctx = &f->ctx;

    current = f;
    fiber_switch(old_ctx, new_ctx);
    return true;
}

void Fiber::ret(int code) {
    if (current != &fibers.at(0)) {
        current->status = Fiber::Unused;
        yield();
    }

    while (yield());
    exit(code);
}

void Fiber::stop() {
    ret(0);
}

int Fiber::run(void (*fun)(void)) {
    char *stack;
    Fiber *f;

    for (f = &fibers.at(0);; f++) {
        if (f == &fibers.at(MAXFIBERS-1)) {
            return -1;

        } else if (f->status == Fiber::Unused) {
            break;
        }
    }

    stack = (char *) std::malloc(STACKSIZE);
    if (!stack) return -1;

    *(uint64_t *)&stack[STACKSIZE - 8] = (uint64_t)stop;
    *(uint64_t *)&stack[STACKSIZE - 16] = (uint64_t)fun;

    f->ctx.rsp = (uint64_t)&stack[STACKSIZE - 16];
    f->status = Fiber::Ready;

    return 0;
}

void Fiber::wait() {
    ret(1);
}