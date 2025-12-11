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

// Usage of unsafe shorthands.
CR_DEF(ExpensiveOperation, int x; int y; long timer) {
    CR_ARG(x) = CR_ARG(y);

    YIELD(0);

    printf("timer: %li\n", CR_ARG(timer));

    YIELD_UNTIL(1, --CR_ARG(timer) == 0);

    printf("this is state 1: %i\n", CR_ARG(x));

    YIELD(2);
    CR_END;
}

// Usage of full macros and noargs.
C_ROUTINE_DEF_NOARGS(OtherOperation) {
    C_ROUTINE_YIELD(0);
    C_ROUTINE_YIELD(1);
    C_ROUTINE_YIELD(2);
    C_ROUTINE_YIELD(3);
    C_ROUTINE_YIELD(4);
    C_ROUTINE_YIELD(5);
    C_ROUTINE_YIELD(6);
    C_ROUTINE_YIELD(7);
    C_ROUTINE_YIELD(8);
    C_ROUTINE_YIELD(9);
    C_ROUTINE_YIELD(10);
    C_ROUTINE_YIELD(11);
    C_ROUTINE_YIELD(12);
    C_ROUTINE_YIELD(13);
    C_ROUTINE_YIELD(14);
    C_ROUTINE_END;
}

int main() {
    C_RoutineState state = CR_INIT(ExpensiveOperation, 1, 0, 10);
    C_RoutineState state2 = C_ROUTINE_INIT_NOARGS(OtherOperation);

    while (PROGRAM_SHOULD_CONTINUE) {
        printf("EXIT_CODE: %i, C_ROUTINE STATE: %llu\n", EXIT_CODE, state);
        CR_RUN(ExpensiveOperation, state) ONEND {
            PROGRAM_QUEUE_EXIT();
        }

        printf("C_ROUTINE STATE2: %llu\n", state2);
        C_ROUTINE_RUN_NOARGS(OtherOperation, state2);
    }

    return EXIT_CODE;
}