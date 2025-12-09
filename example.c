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

C_ROUTINE_DEF(ExpensiveOperation, int x; int y; long timer) {
    C_ROUTINE_STATE(0):
        C_ROUTINE_ARG(x) = C_ROUTINE_ARG(y);

        C_ROUTINE_YIELD;
    C_ROUTINE_STATE(1):
        printf("timer: %li\n", C_ROUTINE_ARG(timer));

        C_ROUTINE_YIELD_UNTIL(--C_ROUTINE_ARG(timer) == 0);
    C_ROUTINE_STATE(2):
            printf("this is state 1: %i\n", C_ROUTINE_ARG(x));

        C_ROUTINE_YIELD;
    C_ROUTINE_END;
}

C_ROUTINE_DEF_NOARGS(OtherOperation) {
    C_ROUTINE_STATE(0):
        C_ROUTINE_YIELD;
    C_ROUTINE_STATE(1):
        C_ROUTINE_YIELD;
    C_ROUTINE_STATE(2):
        C_ROUTINE_YIELD;
    C_ROUTINE_END;
}

int main() {
    C_RoutineState state = C_ROUTINE_INIT(ExpensiveOperation, 1, 0, 10);
    C_RoutineState state2 = C_ROUTINE_INIT_NOARGS(OtherOperation);

    while (PROGRAM_SHOULD_CONTINUE) {
        printf("EXIT_CODE: %i, C_ROUTINE STATE: %llu\n", EXIT_CODE, state);
        C_ROUTINE_RUN(ExpensiveOperation, state) C_ROUTINE_ON_END {
            PROGRAM_QUEUE_EXIT();
        }

        printf("C_ROUTINE STATE2: %llu\n", state2);
        C_ROUTINE_RUN_NOARGS(OtherOperation, state2);
    }

    return EXIT_CODE;
}