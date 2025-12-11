#include "coroutine.h"
#include <stdio.h>

typedef enum {
    EXIT_CODE_CONTINUE,
    EXIT_CODE_SUCCESS,
} ExitCode;

#define EXIT_CODE g_exitCode
#define PROGRAM_SHOULD_CONTINUE EXIT_CODE == EXIT_CODE_CONTINUE
#define PROGRAM_QUEUE_EXIT() EXIT_CODE = EXIT_CODE_SUCCESS

ExitCode EXIT_CODE = EXIT_CODE_CONTINUE;

#if defined(C_ROUTINE_UNSAFE_SHORTHANDS) && !defined(C_ROUTINE_MANUAL_STATE)
// Example of unsafe shorthands.
CR_DEF(ExpensiveOperation, int x; int y; long timer) {
    CR_ARG(x) = CR_ARG(y);

    YIELD;

    printf("timer: %li\n", CR_ARG(timer));

    YIELD_UNTIL(--CR_ARG(timer) == 0);

    printf("this is state 1: %i\n", CR_ARG(x));

    YIELD;
    CR_END;
}
#elif defined(C_ROUTINE_MANUAL_STATE)
// Example of manual state.
C_ROUTINE_DEF(ExpensiveOperation, int x; int y; long timer) {
    C_ROUTINE_ARG(x) = C_ROUTINE_ARG(y);

    C_ROUTINE_YIELD(0);

    printf("timer: %li\n", C_ROUTINE_ARG(timer));

    C_ROUTINE_YIELD_UNTIL(1, --C_ROUTINE_ARG(timer) == 0);

    printf("this is state 1: %i\n", C_ROUTINE_ARG(x));

    C_ROUTINE_YIELD(2);
    C_ROUTINE_END;
}
#else
// Example of standard C_ROUTINE.
C_ROUTINE_DEF(ExpensiveOperation, int x; int y; long timer) {
    C_ROUTINE_ARG(x) = C_ROUTINE_ARG(y);

    C_ROUTINE_YIELD;

    printf("timer: %li\n", C_ROUTINE_ARG(timer));

    C_ROUTINE_YIELD_UNTIL(--C_ROUTINE_ARG(timer) == 0);

    printf("this is state 1: %i\n", C_ROUTINE_ARG(x));

    C_ROUTINE_YIELD;
    C_ROUTINE_END;
}
#endif

#ifndef C_ROUTINE_MANUAL_STATE
// Example of noargs.
C_ROUTINE_DEF_NOARGS(OtherOperation) {
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_YIELD;
    C_ROUTINE_END;
}
#endif

int main() {
    C_RoutineState state = C_ROUTINE_INIT(ExpensiveOperation, 1, 0, 10);

#ifndef C_ROUTINE_MANUAL_STATE
    C_RoutineState state2 = C_ROUTINE_INIT_NOARGS(OtherOperation);
#endif

    while (PROGRAM_SHOULD_CONTINUE) {
        printf("EXIT_CODE: %i, C_ROUTINE STATE: %llu\n", EXIT_CODE, state);
        C_ROUTINE_RUN(ExpensiveOperation, state) C_ROUTINE_ON_END {
            PROGRAM_QUEUE_EXIT();
        }

#ifndef C_ROUTINE_MANUAL_STATE
        printf("C_ROUTINE STATE2: %llu\n", state2);
        C_ROUTINE_RUN_NOARGS(OtherOperation, state2);
#endif
    }

    return EXIT_CODE;
}