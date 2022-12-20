BUILDDIR ?= build
DEPDIR ?= dep
SERVER_DIR = ./server
CLIENT_DIR = ./client
PROJ_DIR=$(shell pwd)

JSON_C_DIR=/usr/local

CFLAGS += -I$(JSON_C_DIR)/include -I$(JSON_C_DIR)/include/json-c -I$(PROJ_DIR)
LDFLAGS+= -L$(JSON_C_DIR)/lib -ljson-c

HTTP_SRC += http/http-api.c \
			http/socket_linux.c

TARGET_SERVER := server_app
SERVER_SRC := $(shell find $(SERVER_DIR) -name "*.c")
# EXCLUDE HEADER FILES WITH NO C IMPLEMENTATION
SERVER_HEADER_SRC := $(filter %.h,$(shell gcc -MM $(SERVER_SRC)))
SERVER_HEADER_SRC := $(SERVER_HEADER_SRC:.h=.c)
SERVER_CUSTOM_SRC := $(HTTP_SRC)
SERVER_SRCS := $(SERVER_SRC) $(SERVER_HEADER_SRC) $(SERVER_CUSTOM_SRC)

TARGET_CLIENT := client_app
CLIENT_SRC := $(shell find $(CLIENT_DIR) -name "*.c")
# EXCLUDE HEADER FILES WITH NO C IMPLEMENTATION
CLIENT_HEADER_SRC := $(filter %.h,$(shell gcc -MM $(CLIENT_SRC) $(CLIENT_CFLAGS) ))
CLIENT_HEADER_SRC := $(CLIENT_HEADER_SRC:.h=.c)
CLIENT_CUSTOM_SRC := $(HTTP_SRC)
CLIENT_SRCS := $(CLIENT_SRC) $(CLIENT_HEADER_SRC) $(CLIENT_CUSTOM_SRC) 


DEPS := $(($(SERVER_SRCS) $(CLIENT_SRCS)):.c=.d)
DEPS := $(addprefix $(DEPDIR)/, $(DEPS))


SERVER_OBJS := $(SERVER_SRCS:.c=.o)
SERVER_OBJS := $(addprefix $(BUILDDIR)/, $(SERVER_OBJS))
CLIENT_OBJS := $(CLIENT_SRCS:.c=.o)
CLIENT_OBJS := $(addprefix $(BUILDDIR)/, $(CLIENT_OBJS))

# CFLAGS += -I$(JSON_C_DIR)/include/json-c
# Or to use lines like: #include <json-c/json_object.h>


$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo compiling $@
	gcc -c $< -o $@ $(CFLAGS) $(LDFLAGS)

$(DEPDIR)/%.d: %.c
	@mkdir -p $(dir $@)
	@echo Creating dependency $@
	gcc $(CFLAGS) -MM $^ -MT '$(BUILDDIR)/'$(^:.c=.o) -o $@ $(LDFLAGS)

$(TARGET_SERVER): $(SERVER_OBJS)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(TARGET_CLIENT): $(CLIENT_OBJS)
	gcc -o $@  $^ $(CFLAGS) $(LDFLAGS)

# build-server: $(TARGET_SERVER)
# 	@echo "SERVER ABOBUS"

build-server: $(TARGET_SERVER)
	@echo "Build : server"

build-client: $(TARGET_CLIENT)
	@echo "Build : client"

build: build-client build-server

clean:
	rm -r -f -v $(TARGET_CLIENT) $(TARGET_SERVER) $(DEPDIR) $(BUILDDIR)

.PHONY: build-server build-client build clean