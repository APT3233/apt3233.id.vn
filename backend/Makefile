CC=gcc
CFLAGS=-Wall -Wextra

BUILD_DIR=build
DST_DIR=dst
LOG_FILE=sys.log
PID_FILE=server.pid

OBJS=$(BUILD_DIR)/apt_server.o $(BUILD_DIR)/apt_net.o $(BUILD_DIR)/apt_file.o $(BUILD_DIR)/apt_mime.o $(BUILD_DIR)/apt_cache.o $(BUILD_DIR)/apt_hashtable.o $(BUILD_DIR)/apt_linkedlist.o $(BUILD_DIR)/apt_parse.o $(BUILD_DIR)/apt_time.o $(BUILD_DIR)/apt_log.o

all: $(BUILD_DIR) $(DST_DIR) $(DST_DIR)/server

$(DST_DIR)/server: $(OBJS)
	$(CC) -o $@ $^

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
$(DST_DIR):
	mkdir -p $(DST_DIR)


$(BUILD_DIR)/%.o: src/core/%.c src/core/%.h | $(BUILD_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@ 
$(BUILD_DIR)/%.o: src/utils/%.c src/utils/%.h | $(BUILD_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@ 
$(BUILD_DIR)/apt_server.o: src/core/apt_server.c | $(BUILD_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DST_DIR)
	rm -f cache_tests/cache_tests
	rm -f cache_tests/cache_tests.exe
	rm -f cache_tests/cache_tests.log

run: $(DST_DIR)/server
	$(DST_DIR)/server

runbg: $(DST_DIR)/server
	nohup stdbuf -oL $(DST_DIR)/server >> $(LOG_FILE) 2>&1 & echo $$! > $(PID_FILE)

stop:
	@if [ -f $(PID_FILE) ]; then \
		kill `cat $(PID_FILE)`; \
		rm -f $(PID_FILE); \
		echo "---- Server stopped. ----"; \
	else \
		echo "No server process is running."; \
	fi

# Mục tiêu giả
.PHONY: all clean run runbg stop
