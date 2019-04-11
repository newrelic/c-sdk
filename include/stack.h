/*!
 * @file stack.h
 *
 * @brief Function declarations necessary to support backtraces in the C agent.
 */
#ifndef LIBNEWRELIC_STACK_H
#define LIBNEWRELIC_STACK_H

/*!
 * @brief Return the current stack trace as a JSON string.
 *
 * The caller should free the string subsequent to the
 * newrelic_get_stack_trace_as_json() call.
 *
 * @return The current stack trace as a JSON string.
 *
 */
char* newrelic_get_stack_trace_as_json(void);

#endif /* LIBNEWRELIC_STACK_H */
