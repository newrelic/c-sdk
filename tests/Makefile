#
# The Makefile for the C agent unit test suite.
#
# all:       Builds but does not run the tests.
# run_tests: Builds and runs the tests.
# valgrind:  Builds and runs the tests under valgrind.

VALGRIND := valgrind

#
# Operating system detection.
#
include ../vendor/newrelic/make/config.mk

# These flags that are required to build the tests, and should be unioned with
# any flags inherited from the environment.
TEST_CFLAGS := -std=gnu99 -pthread $(CMOCKA_INCLUDE) -g

TEST_CFLAGS += -Wall
TEST_CFLAGS += -Werror
TEST_CFLAGS += -Wextra
TEST_CFLAGS += -Wcast-qual
TEST_CFLAGS += -Wdeclaration-after-statement
TEST_CFLAGS += -Wimplicit-function-declaration
TEST_CFLAGS += -Wmissing-declarations
TEST_CFLAGS += -Wmissing-prototypes
TEST_CFLAGS += -Wno-write-strings
TEST_CFLAGS += -Wpointer-arith
TEST_CFLAGS += -Wshadow
TEST_CFLAGS += -Wstrict-prototypes
TEST_CFLAGS += -Wswitch-enum

ifeq (1,$(HAVE_CLANG))
TEST_CFLAGS += -Wbool-conversion
TEST_CFLAGS += -Wempty-body
TEST_CFLAGS += -Wheader-hygiene
TEST_CFLAGS += -Wimplicit-fallthrough
TEST_CFLAGS += -Wlogical-op-parentheses
TEST_CFLAGS += -Wloop-analysis
TEST_CFLAGS += -Wsizeof-array-argument
TEST_CFLAGS += -Wstring-conversion
TEST_CFLAGS += -Wswitch
TEST_CFLAGS += -Wswitch-enum
TEST_CFLAGS += -Wuninitialized
TEST_CFLAGS += -Wunused-label
endif

TEST_CPPFLAGS := -I$(C_AGENT_ROOT)/vendor/newrelic/axiom -I$(C_AGENT_ROOT)/vendor/newrelic/axiom/tests -I$(C_AGENT_ROOT)/include -DC_AGENT_ROOT=$(C_AGENT_ROOT)
ifeq (1,$(HAVE_BACKTRACE))
  TEST_CPPFLAGS += -DHAVE_BACKTRACE=1
endif

TEST_LDFLAGS :=
TEST_LDLIBS := $(C_AGENT_ROOT)/libnewrelic.a $(CMOCKA_LIB) -lm

# -pthread must be passed to the compiler, but not the linker when using Clang.
ifneq (1,$(HAVE_CLANG))
  TEST_LDFLAGS += -pthread
endif

ifeq (1,$(HAVE_LIBEXECINFO))
  TEST_LDLIBS += -lexecinfo
endif

# -ldl is required on GNU/Linux for dladdr. We'll test that via a very simple
# C compiler one-liner.
ifeq (Linux,$(UNAME))
  ifeq (1,$(shell (echo "int dladdr(void *, void *); int main() { dladdr(0, 0); return 0; }" | $(CC) -x c -o /dev/null - 1>/dev/null 2>/dev/null) && echo 0 || echo 1))
    TEST_LDLIBS += -ldl
  endif
endif

ifeq (SunOS,$(UNAME))
  TEST_LDLIBS += -lmalloc -lm -lsocket -lnsl -lrt
endif

#
# Flags required to link PCRE.
#
PCRE_CFLAGS := $(shell pcre-config --cflags)
PCRE_LDLIBS := $(shell pcre-config --libs)

all: tests

#
# Tests. Add a binary to this list to have it built by default. Note that the
# file name must start with test_.
#
TESTS := \
	test_add_attribute \
	test_config \
	test_connect_app \
	test_create_app \
	test_custom_event \
	test_custom_metric \
	test_custom_segment \
	test_datastore \
	test_destroy \
	test_end_transaction \
	test_external \
	test_get_default_options \
	test_get_stack_trace_as_json \
	test_get_transaction_options \
	test_global \
	test_notice_error \
	test_segment \
	test_segment_parent_root \
	test_set_transaction_timing \
	test_start_transaction \
	test_txn \
	test_version \

#
# The list of tests to skip and tests to run.
#
SKIP_TESTS =
RUNNABLE_TESTS = $(filter-out $(SKIP_TESTS),$(TESTS))

#
# Implicit rule to build object files. We define them as precious to avoid
# recompilation; we're figuring out dependencies anyway, so we'll want to keep
# them.
#
.PRECIOUS: %.o
%.o: %.c Makefile .deps/compile_flags
	$(CC) $(TEST_CPPFLAGS) $(CPPFLAGS) $(TEST_CFLAGS) $(PCRE_CFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

#
# The top level phony rule to build the tests.
#
.PHONY: tests
tests: $(TESTS)

#
# All test binaries depend on libnewrelic.a.
#
# The shell portion of this pattern rule does the following:
# 1) extracts all symbols from the object file that begin with `__wrap`;
# 2) transforms the symbols to strings representing the gcc linker options necessary to link with libcmocka.
#
# For example, suppose the following object file has the following symbols:
#
# $ nm test_example.o | grep -o "__wrap\w*"
# __wrap_foo
# __wrap_bar
#
# These symbols would be transformed into these linker optioms:
#
# -Wl,--wrap=foo -Wl,--wrap=bar
#
# Note that GNU ld's function wrapping functionality can only be applied to
# functions that are being linked. Notably, this means that you cannot change a
# call from one function to another within the same translation unit. This
# generally doesn't pose major issues when mocking axiom functions, but care
# must be taken when mocking libnewrelic functions that those functions are not
# called from within the same source file.
#
test_%: test_%.o common.o ../libnewrelic.a Makefile .deps/link_flags
	$(CC) $(TEST_LDFLAGS) $(LDFLAGS) -o $@ $< common.o $(TEST_LDLIBS) $(PCRE_LDLIBS) $(LDLIBS) $(shell nm $< | grep -o "__wrap\w*" | sed 's/__wrap_/-Wl,--wrap=/g')

#
# The top level rule to run the tests.
#
.PHONY: check run_tests
check run_tests: $(RUNNABLE_TESTS:%=%.phony) Makefile | tests

#
# This is effectively a phony implicit rule, which isn't supported by make as
# such. We'll obviously never create a (for example) test_config.phony file, but
# this means that make will run the test binary each time the run-unit-tests
# target above is invoked.
#
# The advantage to doing it this way rather than having run-unit-tests run each
# binary in a sh for loop is that make will detect non-zero return codes and
# abort immediately.
#
%.phony: %
	@./$< $(TESTARGS)

#
# Track the flags passed to the compiler to force a rebuild when they change.
# These rules must kept in sync with the pattern rules used to perform the
# compilation and linking.
#
# The trick here is forcing the .deps/*_flags targets to be re-evaluated for
# each build while ensuring they are only out of date if their contents need
# to be updated. We use a PHONY dependency to do so.
#
.PHONY: force
.deps/compile_flags: force | .deps/
	@echo '$(TEST_CPPFLAGS) $(CPPFLAGS) $(TEST_CFLAGS) $(PCRE_CFLAGS) $(CFLAGS) -MMD -MP' | cmp -s - $@ || echo '$(TEST_CPPFLAGS) $(CPPFLAGS) $(TEST_CFLAGS) $(PCRE_CFLAGS) $(CFLAGS) -MMD -MP' > $@

.deps/link_flags: force | .deps/
	@echo '$(TEST_LDFLAGS) $(LDFLAGS) $(TEST_LDLIBS) $(PCRE_LDLIBS) $(LDLIBS)' | cmp -s - $@ || echo '$(TEST_LDFLAGS) $(LDFLAGS) $(TEST_LDLIBS) $(PCRE_LDLIBS) $(LDLIBS)' > $@

.deps/:
	@mkdir .deps

#
# Clean up build products: that's object files, dependency files,
# and any output files.
#
clean:
	rm -f *.gcov *.gcno *.gcda *.valgrind.log
	rm -f *.d *.o $(TESTS)
	rm -rf .deps *.dSYM

#
# Valgrind variables. These aren't defined at the top due to the extreme
# unlikeliness that we'll ever want to override them at runtime, but can be
# overridden if you really want to.
#
CHECK_VALGRIND_OUTPUT := $(C_AGENT_ROOT)/vendor/newrelic/make/check_valgrind_output.awk

#
# The exit code handling below is subtle. Valgrind returns the exit code of the
# called test, which (if it's non-zero) we need to detect to abort the for loop
# manually (by default, the exit code never makes it back to make, as the only
# exit code make sees is the eventual one from the for loop). So we grab it in
# $TEST_EXIT_CODE, then check that after we've finished reporting and exit if
# needed.
#
# We _also_ need to abort if Valgrind itself detected errors. You can't
# configure Valgrind to report a non-zero exit code for its own errors _and_
# the underlying program's exit code (it's one or the other), so we'll do this
# by post-processing the already post-processed output from
# CHECK_VALGRIND_OUTPUT. Ugly, but effective.
#
.PHONY: valgrind
valgrind: tests Makefile
	@for T in $(TESTS); do \
	   $(VALGRIND) --tool=memcheck --num-callers=30 --dsymutil=yes --leak-check=full --show-reachable=yes --log-file=$$T.valgrind.log ./$$T $(TESTARGS); \
	   TEST_EXIT_CODE=$$?; \
	   VALGRIND_RESULTS=$$(VALGRIND_VERBOSE=1 $(CHECK_VALGRIND_OUTPUT) $$T.valgrind.log); \
	   printf "%42s with %s\n" ' ' "$$VALGRIND_RESULTS"; \
	   test "$$TEST_EXIT_CODE" -ne 0 && exit "$$TEST_EXIT_CODE"; \
(	   echo "$$VALGRIND_RESULTS" | grep -Eqv '^0 errors from 0 contexts') && exit 1; \
	done; \
	exit 0;

#
# Dependency handling. When we build a .o file, we also build a .d file
# containing that module's dependencies using -MM. Those files are in Makefile
# format, so we include them here to define dependency rules: this means that
# if we change a header, all affected modules will be recompiled automatically.
#
-include $(TESTS:%=%.d)
