# Create a list of all cpp files in the folder client and server
# Attention! Linux is case sensitie!
CLIENT_FILES := $(shell find client -name '*.cpp')
SERVER_FILES := $(shell find server -name '*.cpp')

# Now go through all the files in the list and
# 1. Add the prefix "build/" so that "client/*.cpp" becomes "build/client/*.cpp"
# 2. Replace ".cpp" with ".o"
CLIENT_OBJS := $(addprefix build/,$(CLIENT_FILES:.cpp=.o))
SERVER_OBJS := $(addprefix build/,$(SERVER_FILES:.cpp=.o))

# These are the COMPILER flags
# Use c++14 and add header search paths via -I.
# SDL headers come with "make deps" but if they did not we also have them in
# this repository.
CFLAGS := -std=c++14 \
-I/usr/local/include \
-I/usr/include \
-Iinclude

# These are the LINKER flags
# Link against the libraries SDL2 and SDL2_net. The corresponding files are
# named "libSDL2.so" and "libSDL2_net.so" and these are searched in the two
# directories added with -L.
# The files are installed via "make deps"
LFLAGS := -lSDL2 -lSDL2_net \
-L/usr/local/lib \
-L/usr/lib

# Define how to call the compiler and linker
COMPILE := c++ -c $(CFLAGS)
LINK := c++ $(LFLAGS)

# Now starts the target section.
# Targets are referring to FILES or DIRECTORIES. The target "bin" will not be
# processed if a file or directory "bin" exists (and has not changed).
# If a target is added as a dependency to the ".PHONY" target it stands on its
# own and is NOT bound to a file or directory.

# The additional target mentioned after the ":" are dependent targets. These
# have to be resolved BEFORE a target is executed. One example is the target
# "build/client/%.o: client/%.cpp" here we generate .o files only if .cpp files
# exist and have changed. The "%" acts like a "*".
# The "magic" variables $<, $@ and so on have special meanings
# $< means "insert each dependency one-by-one"
# $@ means is equal to the name of the target
# $^ means "insert the list of all dependencies and eliminate duplicates"
# A single @ at the start of the command means "do not print the command"

bin:
	@mkdir -p bin

build/client:
	@mkdir -p build/client

build/server:
	@mkdir -p build/server

# For each .cpp file in client generate a .o file in build/client
build/client/%.o: client/%.cpp
	$(COMPILE) $< -o $@

# For each .cpp file in server generate a .o file in build/server
build/server/%.o: server/%.cpp
	$(COMPILE) $< -o $@

# bin/client depends on all .o files that should be generated for client
bin/client: $(CLIENT_OBJS)
	$(LINK) -o $@ $^

# bin/server depends on all .o files that should be generated for server
bin/server: $(SERVER_OBJS)
	$(LINK) -o $@ $^

# The following targets are the actual "entry points" for make.
# This is what you add after "make". E.g. "make client" will call all targets
# required to build the client.

.PHONY: client
client: bin build/client bin/client

.PHONY: server
server: bin build/server bin/server

.PHONY: clean
clean:
	@rm -rf bin build

.PHONY: all
all: client server

.PHONY: deps
deps:
	@sudo apt-get update
	@sudo apt-get install clang libsdl2-dev libsdl2-net-dev