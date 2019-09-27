#include <stdint.h>
#include <signal.h>
#include "init.h"
#include <ucontext.h>
#include <sys/ucontext.h>
#include <stdlib.h>

void sig_func(int sig, siginfo_t *info, void *ctx) {
    ucontext_t *uc = (ucontext_t *) ctx;
    static int amt_call = 0;
    if (0x138b == *(uint16_t *) uc->uc_mcontext.gregs[REG_RIP]) {
        uc->uc_mcontext.gregs[REG_RDX] = 100000 + (++amt_call);
        uc->uc_mcontext.gregs[REG_RIP] += 2;
    }
    else if (0x0000538b == ((*(uint32_t *) uc->uc_mcontext.gregs[REG_RIP]) & 0x0000ffff)) {
        int index = ((*(uint32_t *) uc->uc_mcontext.gregs[REG_RIP]) & 0x00ff0000) >> 18;
        uc->uc_mcontext.gregs[REG_RDX] = 100000 + 1000 * index + (++amt_call);
        uc->uc_mcontext.gregs[REG_RIP] += 3;
    }
    else if (0x0000558b == ((*(uint32_t *) uc->uc_mcontext.gregs[REG_RIP]) & 0x0000ffff)) {
        int index = (((*(uint32_t *) uc->uc_mcontext.gregs[REG_RIP]) & 0x00ff0000) >> 18) + 4;
        uc->uc_mcontext.gregs[REG_RDX] = 100000 + 1000 * index + (++amt_call);
        uc->uc_mcontext.gregs[REG_RIP] += 3;
    }
    else if (0x0b8b == *(uint16_t *) uc->uc_mcontext.gregs[REG_RIP]) {
        uc->uc_mcontext.gregs[REG_RCX] = 100000 + (++amt_call);
        uc->uc_mcontext.gregs[REG_RIP] += 2;
    }
    else if (0x00004d8b == ((*(uint32_t *) uc->uc_mcontext.gregs[REG_RIP]) & 0x0000ffff)) {
        int index = (((*(uint32_t *) uc->uc_mcontext.gregs[REG_RIP]) & 0x00ff0000) >> 18) + 4;
        uc->uc_mcontext.gregs[REG_RCX] = 100000 + 1000 * index + (++amt_call);
        uc->uc_mcontext.gregs[REG_RIP] += 3;
    }
}

void init(void *base) {
    struct sigaction sigact = {
        .sa_sigaction = sig_func,
        .sa_flags = SA_RESTART,
    };
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGSEGV, &sigact, NULL);
}
