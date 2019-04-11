FlatBuffers Protocol
--------------------------------------------------
The PHP Agent (via the axiom libraries) and Go Daemon communicate with each other using Unix sockets.  The wire format (i.e. the format of the data sent via the socket connection) is implemented using [Google's FlatBuffers project](https://google.github.io/flatbuffers/).  This document is intended as a brief overview the what FlatBuffers are generically, as well as how we're using them specifically.

Why Flatbuffers
--------------------------------------------------
To understand the problem the FlatBuffers project is trying to solve, it helps to understand the evolution of Standard Practices™ when two computer programs need to talk to one another.

Throughout the late 90s and 00s, the pattern of encoding data in JSON and transporting data via HTTP became extremely popular.  The simplicity and flexibility of JSON and the ubiquity of HTTP made both formats extremely attractive.  Open source toolchains, (as well as cottage industries) grew up around these formats and there was much rejoicing.

In some quarters, however,  the rejoicing was muted. For all their flexibility, both HTTP and JSON  make inefficient use of a computer's resources.  At a small scale, this doesn't matter.  At a large scale, efficient use of your resources becomes critical.  Also, a lot of the "engineering time" wins that JSON brings to dynamically typed languages aren't necessarily shared when using JSON in systems level programming languages like C or go.

This leaves systems engineers with a dilemma.  Embrace the standard practices and accept the inefficient use of computer resources?  Or eschew them and give up on modern engineering efficiencies?

The FlatBuffers project is an attempt have it both ways. The FlatBuffers project allows engineers to

1. Define their *strictly typed* data format using a plaintext format (an `.fbs` file)

2. "Compile" that `.fbs` file into source-code that provides data objects for engineers in their own programming language (C++, Go, Java, Javascript, TypeScript, C#, Python, or PHP)

3. Provide a standard set of methods for serializing that data as a sequences of bytes, suitable for non-plaintext networking formats (like Unix sockets)

4. Provide a standard set of methods to unserialize that sequence of bytes back into objects using the generated source code

Put another way -- the FlatBuffers project allows you to define structured data, program with that data using classes/types/structs/etc that are native to your preferred programming language, and send that data around the network without needing to worry about laying that data out in memory or marshal that data off the wire.

FlatBuffers and Native Agents
--------------------------------------------------
All communication between the daemon and agent happens via Unix sockets.  The data for the sockets is prepared using flatbuffers.

The `.fbs` file that describes the data sent between agent and daemon [lives in the daemon's src directory](https://source.datanerd.us/php-agent/php_agent/blob/master/src/newrelic/protocol.fbs).  We also use [Google's FlatBuffers's go library](https://github.com/google/flatbuffers/tree/master/go).  This library [is manually vendored in our source tree](https://source.datanerd.us/php-agent/php_agent/tree/master/src/vendor/github.com/google/flatbuffers/go).

Things are a little trickier on the agent side.  The C language is not supported natively by the FlatBuffers project.  We've implemented a sub-set of flatbuffers functionality in axiom.  Some relevant files are

- [`util_flatbuffers.c/util_flatbuffers.h`](https://source.datanerd.us/php-agent/php_agent/blob/master/axiom/util_flatbuffers.c): Flatbuffers Utility Functions
- [`nr_commands_private.h`](https://source.datanerd.us/php-agent/php_agent/blob/master/axiom/nr_commands_private.h): Partially generated file with flatbuffer indexes
- [`cmd_appinfo_transmit.c`](https://source.datanerd.us/php-agent/php_agent/blob/master/axiom/cmd_appinfo_transmit.c): Implementation of the "Send APPINFO to daemon" command
- [`cmd_txndata_transmit.c`](https://source.datanerd.us/php-agent/php_agent/blob/master/axiom/cmd_txndata_transmit.c): Implementation of the "Send TXNDATA to daemon" command

### The FlatBuffers Compiler

FlatBuffers uses a "compiler" program named `flatc`.   You can install this program on your Mac using homebrew.

    brew install flatbuffers

If you're rolling *nix, you'll need to compile from source

    git clone https://github.com/google/flatbuffers.git
    cd flatbuffers
    cmake -G "Unix Makefiles"
    make
    ./flattests # runs the tests!
    ./flatc --helps

The purpose of `flatc` is to take our `.fbs` file, and turn it into a bunch of code we can use in our programs.

### Compiling FlatBuffers for the Daemon

To compile the flatbuffers for the daemon, do the following from `php-agent/php_agent`

    cd src/newrelic
    flatc -g protocol.fbs

This will generate a number of files in the `protocol` folder.

    $ ls -1 protocol
    App.go
    AppReply.go
    AppStatus.go
    Error.go
    Event.go
    Message.go
    MessageBody.go
    Metric.go
    MetricData.go
    SlowSQL.go
    Trace.go
    Transaction.go
    encode.go       #NOTE: not a generated file -- contains helper methods written by us

### Compiling FlatBuffers for the Agent

@TODO: Not sure where we compile the indexes in `nr_commands_private.h`, planning on talking to Mike about about.

@TODO: Not sure if there's other C stuff that's compiled/generated.

@TODO: Maybe we use flatc --cpp and manually transform stuff for C?

@TODO: Also, it seems like since we did our original flat buffers project that [a library and compiler for C popped up](https://github.com/dvidelabs/flatcc).  I don't think we use this for anything but probably worth mentioning to mike.

### Usage in C

@TODO: High level overview of how we use our C based flat buffer libraries

@TODO: Mention importance of doing things in order?  If that's a thing?

### Usage in go

@TODO: High level overview of some go based flat buffers code.
