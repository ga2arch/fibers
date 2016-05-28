//
// Created by Gabriele Carrettoni on 28/05/16.
//

#ifndef FIBERS_FIBER_H
#define FIBERS_FIBER_H

#include <cstdint>
#include <array>
#include <cstdlib>

class Fiber {
public:
    struct Context {
        uint64_t rsp;
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t rbx;
        uint64_t rbp;
    };

    static void init();
    static bool yield();
    static void __attribute__((noreturn)) ret(int code);
    static void stop();
    static int run(void (*fun)(void));
    static void wait();

private:
    const static int MAXFIBERS = 4;
    const static int STACKSIZE = 1024*1024*4;

    Context context;

    enum {
        Unused,
        Running,
        Ready
    } status;

    static Fiber *current;
    static std::array<Fiber, MAXFIBERS> fibers;

};

extern "C" {
    void fiber_switch(Fiber::Context *old_ctx, Fiber::Context *new_ctx);
}

#endif //FIBERS_FIBER_H
