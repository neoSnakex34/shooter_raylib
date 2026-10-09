CC = gcc
CFLAGS = -Wall -Wextra -O2 -I ./raylib-5.5_linux_amd64/include
LDFLAGS = -L ./raylib-5.5_linux_amd64/lib -lraylib

TARGET = game
SRC = $(filter-out main.c, $(wildcard *.c))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# --- Web (WebAssembly) build ---
# Prerequisites:
#   1. Emscripten installed and activated (source ~/emsdk/emsdk_env.sh)
#   2. raylib built for web: clone raylib 5.5 source, then:
#        mkdir build_web && cd build_web
#        emcmake cmake .. -DPLATFORM_WEB=1 -DCMAKE_BUILD_TYPE=Release
#        emmake make
#   3. Set RAYLIB_WEB_PATH to the directory containing the built libraylib.a
#      and the raylib/src headers (defaults to ./raylib)
RAYLIB_WEB_PATH ?= ./raylib
WEB_TARGET = game.html
WEB_CC = emcc
WEB_CFLAGS = -Wall -Wextra -O2 -DPLATFORM_WEB \
	-I $(RAYLIB_WEB_PATH)/src
WEB_LDFLAGS = -L $(RAYLIB_WEB_PATH)/build_web -lraylib \
	-s USE_GLFW=3 -s FORCE_FILESYSTEM=1 -s ASYNCIFY \
	-s TOTAL_MEMORY=134217728 \
	--preload-file assets

web: $(WEB_TARGET)

$(WEB_TARGET): $(SRC)
	$(WEB_CC) $(WEB_CFLAGS) $(SRC) -o $(WEB_TARGET) $(WEB_LDFLAGS)

clean:
	rm -f $(TARGET) $(WEB_TARGET) *.wasm *.data
