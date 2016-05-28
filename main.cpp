#include <iostream>
#include <cassert>

using namespace std;

enum {
    MaxFibers = 4,
    StackSize = 4194304
};

struct fiber {
    struct fiber_context {
        uint64_t rsp;
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t rbx;
        uint64_t rbp;
    } context;

    enum {
        Unused,
        Running,
        Ready
    } status;
};

static struct fiber fibers[MaxFibers];
static struct fiber *current_fiber;

extern "C" {void fiber_switch(struct fiber::fiber_context *old_ctx, struct fiber::fiber_context *new_ctx);}
void fiber_return(int code) __attribute__((noreturn));

void fiber_init(void) {
    current_fiber = &fibers[0];
    current_fiber->status = fiber::Running;
}

bool fiber_yield() {
    struct fiber *f;
    struct fiber::fiber_context *old_ctx, *new_ctx;

    f = current_fiber;
    while (f->status != fiber::Ready) {
        if (++f == &fibers[MaxFibers]) {
            f = &fibers[0];
        }

        if (f == current_fiber) {
            return false;
        }
    }

    if (current_fiber->status != fiber::Unused) {
        current_fiber->status = fiber::Ready;
    }

    f->status = fiber::Running;
    old_ctx = &current_fiber->context;
    new_ctx = &f->context;

    current_fiber = f;
    fiber_switch(old_ctx, new_ctx);
    return true;
}

void fiber_return(int code) {
    if (current_fiber != &fibers[0]) {
        current_fiber->status = fiber::Unused;
        fiber_yield();
        assert(!"reachable");
    }

    while (fiber_yield());
    exit(code);
}

void fiber_stop() {
    fiber_return(0);
}

int fiber_run(void (*fun)(void)) {
    char *stack;
    struct fiber *f;

    for (f = &fibers[0];; f++) {
        if (f == &fibers[MaxFibers]) {
            return -1;

        } else if (f->status == fiber::Unused) {
            break;
        }
    }

    stack = (char *) std::malloc(StackSize);
    if (!stack) return -1;

    *(uint64_t *)&stack[StackSize - 8] = (uint64_t)fiber_stop;
    *(uint64_t *)&stack[StackSize - 16] = (uint64_t)fun;

    f->context.rsp = (uint64_t)&stack[StackSize - 16];
    f->status = fiber::Ready;

    return 0;
}

void
f(void) {
    static int x;
    int i, id;

    id = ++x;
    for (i = 0; i < 10; i++) {
        printf("%d %d\n", id, i);
        fiber_yield();
    }
}

int main(void) {
    fiber_init();
    fiber_run(f);
    fiber_run(f);
    fiber_return(1);
}
