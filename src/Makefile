#
# This is not a standalone Makefile in its current form; it depends on a bunch
# of variables that are defined by the top level Makefile.
#

.PHONY: clean static
clean:
	rm -f *.o *.d libnewrelic.a libnewrelic.so
	rm -rf .deps

static: libnewrelic.a

OBJS := \
	app.o \
	app_internal.o \
	attribute.o \
	config.o \
	custom_event.o \
	custom_metric.o \
	datastore.o \
	error.o \
	external.o \
	global.o \
	segment.o \
	stack.o \
	transaction.o \
	version.o

%.o: %.c Makefile .deps/compile_flags
	$(CC) $(C_AGENT_CPPFLAGS) $(VERSION_FLAGS) $(CPPFLAGS) $(C_AGENT_CFLAGS) $(PCRE_CFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

#
# Handle version.o separately to minimize the recompilation necessary
# when the version number or commit SHA changes.
#
version.o: version.c Makefile .deps/compile_flags .deps/version_flags $(C_AGENT_ROOT)/VERSION
	$(CC) $(C_AGENT_CPPFLAGS) $(VERSION_FLAGS) $(CPPFLAGS) $(C_AGENT_CFLAGS) $(PCRE_CFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

#
# Note that this rule only builds the C agent object files into the static
# library. The parent Makefile takes care of bundling this static library with
# the axiom static library.
#
libnewrelic.a: $(OBJS)
	$(AR) rcs $@ $^

#
# Track the flags passed to the compiler to force a rebuild when they change.
# This ensures a rebuild occurs when the version number or commit are updated.
# These rules must kept in sync with the pattern rules used to perform the
# actual compilation.
#
# The trick here is forcing the .deps/*_flags targets to be re-evaluated for
# each build while ensuring they are only out of date if their contents need
# to be updated. We use a PHONY dependency to do so.
#
.PHONY: force
.deps/compile_flags: force | .deps/
	@echo '$(AXIOM_CPPFLAGS) $(CPPFLAGS) $(AXIOM_CFLAGS) $(PCRE_CFLAGS) $(CFLAGS)' | cmp -s - $@ || echo '$(AXIOM_CPPFLAGS) $(CPPFLAGS) $(AXIOM_CFLAGS) $(PCRE_CFLAGS) $(CFLAGS)' > $@

.deps/version_flags: force | .deps/
	@echo '$(VERSION_FLAGS)' | cmp -s - $@ || echo '$(VERSION_FLAGS)' > $@

.deps/:
	@mkdir .deps

#
# Dependency handling. When we build a .o file, we also build a .d file
# containing that module's dependencies using -MM. Those files are in Makefile
# format, so we include them here to define dependency rules: this means that
# if we change a header, all affected modules will be recompiled automatically.
#
-include $(OBJS:.o=.d)
