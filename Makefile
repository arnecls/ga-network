CLIENT_FILES := $(shell find client -name '*.cpp')
SERVER_FILES := $(shell find server -name '*.cpp')

CLIENT_OBJS := $(addprefix build/,$(CLIENT_FILES:.cpp=.o))
SERVER_OBJS := $(addprefix build/,$(SERVER_FILES:.cpp=.o))

CFLAGS := -std=c++11 \
-I/usr/local/include \
-I/usr/include

LFLAGS := -lSDL2 -lSDL2_net \
-L/usr/local/lib \
-L/usr/lib

COMPILE := c++ -c $(CFLAGS)
LINK := c++ $(LFLAGS)

bin:
	@mkdir -p bin

build/client:
	@mkdir -p build/client

build/server:
	@mkdir -p build/server

build/client/%.o: client/%.cpp
	$(COMPILE) $< -o $@

build/server/%.o: server/%.cpp
	$(COMPILE) $< -o $@

bin/client: $(CLIENT_OBJS)
	$(LINK) -o $@ $^

bin/server: $(SERVER_OBJS)
	$(LINK) -o $@ $^

.PHONY: client
client: bin build/client bin/client

.PHONY: server
server: bin build/server bin/server

.PHONY: deps
deps:
	@sudo apt-get update
	@sudo apt-get install git clang make libsdl2-dev libsdl2-net-dev