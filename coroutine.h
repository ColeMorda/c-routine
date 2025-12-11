/****************************************************************************************************************
 *                                             C_ROUTINE
 *                                   LUA-STYLE COROUTINE LIBRARY FOR C
 *                                     AUTHORED BY COLE MORDA, 2025
 *                                 https://github.com/ColeMorda/c-routine
 *
 * C_ROUTINE © 2025 by Cole Morda is licensed under Creative Commons Attribution 4.0 International.
 * To view a copy of this license, visit https://creativecommons.org/licenses/by/4.0/
 ****************************************************************************************************************
 * FAQ:
 *      Q: What is this?
 *      A: This is a lightweight, header-only coroutine system for C.
 *         It allows you to write functions that can "pause" and "resume" execution
 *         at certain points, without using threads or operating system-level context switching.
 *         C_ROUTINES are useful for tasks like:
 *             - Long-running operations that you want to split across multiple frames (game loops, simulations)
 *             - Asynchronous I/O without blocking
 *             - Generators or lazy iterators
 *             - State machines
 *
 *      Q: How do I use this?
 *      A: An example of a simple C_ROUTINE can be found in example.c
 *
 *      Q: AHHH!!!! IT LOOKS LIKE PYTHON!!!!
 *      A: RELAX!!! Look, I know indentation sucks, but trust me, this is pure C.
 *         C_ROUTINES use switch statements to evaluate what state the C_ROUTINE is currently in,
 *         so you technically don't even need the indents.
 *         If it helps, there's a few things you can do:
 *         - Buy some curly bracket stickers and stick them to your monitor where the colons appear
 *         - Think happy thoughts. "I love manual memory management", "Abstraction is evil", etc.
 *         - Use Lua for your project instead. If it's good enough for Cloudflare, it's good enough for you.
 ****************************************************************************************************************
 * FLAGS:
 *      -DC_ROUTINE_UNSAFE_SHORTHANDS: Allows use of potentially conflicting shorthand macros such as ONEND.
 *      -DC_ROUTINE_MANUAL_STATE: Disables the automatic state handler, This means you must explicitly define each state in C_ROUTINE_YIELD, (e.g. C_ROUTINE_YIELD(0))
 ****************************************************************************************************************
 * WARNINGS:
 *      C_ROUTINE reserves the following keywords:
 *          C_ROUTINE_H
 *          C_RoutineState
 *          RESERVED_C_ROUTINE_EXIT_STATE
 *          C_ROUTINE_DEF
 *          C_ROUTINE_DEF_EXTERN
 *          C_ROUTINE_DEF_NOARGS
 *          C_ROUTINE_DEF_NOARGS_EXTERN
 *          C_ROUTINE_INIT
 *          C_ROUTINE_RUN
 *          C_ROUTINE_RUN_NOARGS
 *          C_ROUTINE_ON_END
 *          C_ROUTINE_EXIT
 *          C_ROUTINE_ARG
 *          C_ROUTINE_YIELD
 *          C_ROUTINE_YIELD_UNTIL
 *          C_ROUTINE_END
 *          c_routine_state
 *          c_routine_args
 *          C_ROUTINE_UNSAFE_SHORTHANDS
 *          C_ROUTINE_MANUAL_STATE
 *      As well as the following formats:
 *          CRO_*
 *          CRO_*_ARGS
 *      
 *      Active support may or may not be available for C_ROUTINE.
 *      You may contact the creator for support by emailing cole@morda.us or creating an issue on GitHub,
 *      though response is not guaranteed.
 ****************************************************************************************************************
 * CHANGELOG:
 *      12.09.2025: Initial completion.
 *      12.10.2025: Automatic state management.
*****************************************************************************************************************/

#ifndef C_ROUTINE_H
#define C_ROUTINE_H

#include <limits.h>

// These shorthand macros may conflict with other modules,
// so they are disabled by default unless -DC_ROUTINE_UNSAFE_SHORTHANDS is passed to compiler.
#ifdef C_ROUTINE_UNSAFE_SHORTHANDS
    #define CRState C_RoutineState
    #define CR_DEF C_ROUTINE_DEF
    #define CR_DEF_EX C_ROUTINE_DEF_EXTERN
    #define CR_DEF_NA C_ROUTINE_DEF_NOARGS
    #define CR_DEF_NA_EX C_ROUTINE_DEF_NOARGS_EXTERN
    #define CR_INIT C_ROUTINE_INIT
    #define CR_INIT_NA C_ROUTINE_INIT_NOARGS
    #define CR_RUN C_ROUTINE_RUN
    #define CR_RUN_NA C_ROUTINE_RUN_NOARGS
    #define ONEND C_ROUTINE_ON_END
    #define CR_EXIT C_ROUTINE_EXIT
    #define CR_ARG C_ROUTINE_ARG
    #define YIELD C_ROUTINE_YIELD
    #define YIELD_UNTIL C_ROUTINE_YIELD_UNTIL
    #define CR_END C_ROUTINE_END
#endif

// C_Routines must not be written in any line after (64 bits - 2), because the system currently uses __LINE__ for automatic state management.
typedef unsigned long long C_RoutineState;

#define RESERVED_C_ROUTINE_EXIT_STATE (ULLONG_MAX - 1)

/** Define a c_routine function. Use the same way you would define a normal function.
 * NOTE: If function needs no args or persistent state, use C_ROUTINE_DEF_NOARGS.
 * @param id Symbol definition for the function
 * @param args Arguments to be passed to the function, as well as all variables inside the function NOTE: Separator must be ; instead of ,
 * @example C_ROUTINE_DEF(ExpensiveOperation, int x; int y) {
**/
#define C_ROUTINE_DEF(id, args)                                                                     \
typedef struct {args;} CRO_##id##_ARGS;                                                             \
void CRO_##id(C_RoutineState* c_routine_state, CRO_##id##_ARGS* c_routine_args) {                   \
    switch(*c_routine_state) {                                                                      \
        case 0:


/** For use in header files.
 * @example C_ROUTINE_DEF_EXTERN(ExpensiveOperation, int x; int y);
**/
#define C_ROUTINE_DEF_EXTERN(id, args)                                                              \
typedef struct {args;} CRO_##id##_ARGS;                                                             \
extern void CRO_##id(C_RoutineState* c_routine_state, CRO_##id##_ARGS* c_routine_args)


/** Define a c_routine function without arguments.
 * NOTE: If function needs args or persistent state, use C_ROUTINE_DEF.
 * @param id Symbol definition for the function
 * @example C_ROUTINE_DEF_NOARGS(ExpensiveOperation) {
**/
#define C_ROUTINE_DEF_NOARGS(id)                                                                    \
void CRO_##id(C_RoutineState* c_routine_state) {                                                    \
    switch(*c_routine_state) {                                                                      \
        case 0:


/** For use in header files.
 * @example C_ROUTINE_DEF_NOARGS_EXTERN(ExpensiveOperation);
**/
#define C_ROUTINE_DEF_NOARGS_EXTERN(id)                                                             \
extern void CRO_##id(C_RoutineState* c_routine_state)


/** Initialize the c_routine state for later use.
 * NOTE: If function doesnt need args or persistent state, use C_ROUTINE_INIT_NOARGS.
 * @param id Symbol definition for the function
 * @example C_RoutineState state = C_ROUTINE_INIT(ExpensiveOperation, 0, 10);
**/
#define C_ROUTINE_INIT(id, ...)                                                                     \
0;                                                                                                  \
CRO_##id##_ARGS c_routine_args##_##id = {__VA_ARGS__}


/** Initialize the c_routine state for later use without arguments.
 * NOTE: If function needs args or persistent state, use C_ROUTINE_INIT.
 * @param id Symbol definition for the function
 * @example C_RoutineState state = C_ROUTINE_INIT_NOARGS(ExpensiveOperation);
**/
#define C_ROUTINE_INIT_NOARGS(id)                                                                   \
0

/** Resume a c_routine.
 * NOTE: If function doesnt need args or persistent state, use C_ROUTINE_RUN_NOARGS.
 * @param id Symbol definition for the function
 * @param state Saved state (from C_ROUTINE_INIT)
 * @example C_ROUTINE_RUN(ExpensiveFunction, state);
**/
#define C_ROUTINE_RUN(id, state)                                                                    \
if (state < RESERVED_C_ROUTINE_EXIT_STATE) {                                                        \
    CRO_##id(&state, &c_routine_args##_##id);                                                       \
}


/** Resume a c_routine without arguments.
 * NOTE: If function needs args or persistent state, use C_ROUTINE_RUN_NOARGS.
 * @param id Symbol definition for the function
 * @param state Saved state (from C_ROUTINE_INIT)
 * @example C_ROUTINE_RUN(ExpensiveFunction, state);
**/
#define C_ROUTINE_RUN_NOARGS(id, state)                                                             \
if (state < RESERVED_C_ROUTINE_EXIT_STATE) {                                                        \
    CRO_##id(&state);                                                                               \
}


/** Run something when a c_routine ends.
 * @example C_ROUTINE_RUN(ExpensiveFunction, state) C_ROUTINE_ON_END {return 0;}
**/
#define C_ROUTINE_ON_END                                                                            \
else


/** Set the c_routine as completed and abort execution
 * @example if (C_ROUTINE_ARG(x) == 0) {C_ROUTINE_EXIT;}
**/
#define C_ROUTINE_EXIT                                                                              \
*c_routine_state = RESERVED_C_ROUTINE_EXIT_STATE;                                                   \
return


/** Get an argument from the c_routine argument struct.
 * @example C_ROUTINE_ARG(x) = 0;
**/
#define C_ROUTINE_ARG(arg)                                                                          \
(c_routine_args->arg)


// These next couple macros make it painfully obvious that c_routines are just switch statements.
// Regardless, we don't want people mixing macros and raw switch syntax. Ugly.
#ifdef C_ROUTINE_MANUAL_STATE
    #define C_ROUTINE_INCREMENT_STATE                                                               \
        (*c_routine_state)++;
#else
    #define C_ROUTINE_INCREMENT_STATE                                                               \
        *c_routine_state = __LINE__;
#endif


#ifdef C_ROUTINE_MANUAL_STATE
    #define C_ROUTINE_YIELD(state)                                                                  \
        C_ROUTINE_INCREMENT_STATE                                                                   \
        return;                                                                                     \
    case (state + 1):
#else
    #define C_ROUTINE_YIELD                                                                         \
        C_ROUTINE_INCREMENT_STATE                                                                   \
        return;                                                                                     \
    case __LINE__:
#endif


/** Similar to C_ROUTINE_YIELD, but will not increment state until cond is met
 * @param cond The conditon to be met
 * NOTE: All code in the current state up to this macro will still be ran regardless of if cond is met!
 * @example C_ROUTINE_YIELD_UNTIL(C_ROUTINE_ARG(x) == 0);
**/
#ifdef C_ROUTINE_MANUAL_STATE
    #define C_ROUTINE_YIELD_UNTIL(state, cond)                                                      \
        if (!(cond)) return;                                                                        \
        C_ROUTINE_INCREMENT_STATE                                                                   \
        return;                                                                                     \
    case (state + 1):
#else
    #define C_ROUTINE_YIELD_UNTIL(cond)                                                             \
        if (!(cond)) return;                                                                        \
        C_ROUTINE_INCREMENT_STATE                                                                   \
        return;                                                                                     \
    case __LINE__:
#endif


#define C_ROUTINE_END                                                                               \
    default:                                                                                        \
        C_ROUTINE_EXIT;                                                                             \
    }                                                                                               \
}

#endif