BUILDDIR ?= build
DEPDIR ?= dep
# SERVER_DIR = ./server
# CLIENT_DIR = ./client
PROJ_DIR=$(shell pwd)


CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)


CFLAGS += $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS += $(shell pkg-config --libs gtk+-3.0) -rdynamic

CFLAGS += -I$(PROJ_DIR) -w

HTTP_SRC += http/http-api.c \
			http/socket_linux.c

TARGET_CLIENT := gtk_main
CLIENT_SRC := gtk_main.c
# EXCLUDE HEADER FILES WITH NO C IMPLEMENTATION
# CLIENT_HEADER_SRC := $(filter %.h,$(shell gcc -MM $(CLIENT_SRC) $(CLIENT_CFLAGS) ))
# CLIENT_HEADER_SRC := $(CLIENT_HEADER_SRC:.h=.c)
CLIENT_CUSTOM_SRC := $(HTTP_SRC)
CLIENT_SRCS := $(CLIENT_SRC) $(CLIENT_HEADER_SRC) $(CLIENT_CUSTOM_SRC) 

CLIENT_OBJS := $(CLIENT_SRCS:.c=.o)
CLIENT_OBJS := $(addprefix $(BUILDDIR)/, $(CLIENT_OBJS))

DEPS := $(($(SERVER_SRCS) $(CLIENT_SRCS)):.c=.d)
DEPS := $(addprefix $(DEPDIR)/, $(DEPS))

#CFLAGS += -I$(JSON_C_DIR)/include/json-c
# Or to use lines like: #include <json-c/json_object.h>


$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo compiling $@
	gcc -c $< -o $@ $(CFLAGS) $(LDFLAGS)

# $(DEPDIR)/%.d: %.c
# 	@mkdir -p $(dir $@)
# 	@echo Creating dependency $@
# 	gcc $(CFLAGS) -MM $^ -MT '$(BUILDDIR)/'$(^:.c=.o) -o $@ $(LDFLAGS)


$(TARGET_CLIENT): $(CLIENT_OBJS)
	gcc -o $@  $^ $(CFLAGS) $(LDFLAGS)

build-client: $(TARGET_CLIENT)
	@echo "Build : client"

build: build-client

clean:
	rm -r -f -v $(TARGET_CLIENT) $(TARGET_SERVER) $(DEPDIR) $(BUILDDIR)

.PHONY: build-client build clean
