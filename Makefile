CONFIG ?= config.default
-include $(CONFIG)


BINARY    ?= Chocolate-Wolfenstein-3D
BUILD_DIR ?= build
TARGET    := $(BUILD_DIR)/$(BINARY)
PREFIX    ?= /usr/local
MANPREFIX ?= $(PREFIX)
UNAME := $(shell uname -s)

INSTALL         ?= install
INSTALL_PROGRAM ?= $(INSTALL) -m 555 -s
INSTALL_MAN     ?= $(INSTALL) -m 444
INSTALL_DATA    ?= $(INSTALL) -m 444


PKG_CONFIG ?= pkg-config
CFLAGS_SDL  ?= $(shell $(PKG_CONFIG) --cflags sdl3 sdl3-mixer)
LDFLAGS_SDL ?= $(shell $(PKG_CONFIG) --libs sdl3 sdl3-mixer)
ELISA_COMPILER_DIR ?= ../compiler


CFLAGS += $(CFLAGS_SDL)

#CFLAGS += -Wall
#CFLAGS += -W
CFLAGS += -Wpointer-arith
CFLAGS += -Wreturn-type
CFLAGS += -Wwrite-strings
CFLAGS += -Wcast-align

CCFLAGS += $(CFLAGS)
CCFLAGS += -std=gnu99
CCFLAGS += -Werror-implicit-function-declaration
CCFLAGS += -Wimplicit-int
CCFLAGS += -Wsequence-point

CXXFLAGS += $(CFLAGS)

LDFLAGS += $(LDFLAGS_SDL)
ifeq ($(UNAME), Darwin)
	LDFLAGS += -framework OpenGL -Wl,-no_fixup_chains
endif
ifeq ($(UNAME), Linux)
	LDFLAGS += -lGL
endif

SRCS :=
SRCS += fmopl.cpp
SRCS += mix_compat.cpp
SRCS += id_ca.cpp
SRCS += id_in.cpp
SRCS += id_pm.cpp
SRCS += id_sd.cpp
SRCS += id_us_1.cpp
SRCS += id_vh.cpp
SRCS += id_vl.cpp
SRCS += signon.cpp
SRCS += wl_act1.cpp
SRCS += wl_act2.cpp
SRCS += wl_agent.cpp
SRCS += wl_debug.cpp
SRCS += wl_draw.cpp
SRCS += wl_game.cpp
SRCS += wl_inter.cpp
SRCS += wl_main.cpp
SRCS += wl_menu.cpp
SRCS += wl_play.cpp
SRCS += wl_state.cpp
SRCS += wl_text.cpp
SRCS += crt.cpp

DEPS = $(addprefix $(BUILD_DIR)/,$(filter %.d, $(SRCS:.c=.d) $(SRCS:.cpp=.d)))
OBJS = $(addprefix $(BUILD_DIR)/,$(filter %.o, $(SRCS:.c=.o) $(SRCS:.cpp=.o)))
ELISA_SRCS := elisa_wolf3d_effects.elisa elisa_wolf3d_palette.elisa elisa_wolf3d_audio.elisa elisa_wolf3d_save.elisa elisa_wolf3d_ui.elisa elisa_wolf3d_video.elisa elisa_wolf3d_pagefile.elisa elisa_wolf3d_input.elisa
ELISA_OBJS := $(addprefix $(BUILD_DIR)/,$(ELISA_SRCS:.elisa=.o))
OBJS += $(ELISA_OBJS)
ELISA_TEST_SRCS := elisa_wolf3d_palette.elisa elisa_wolf3d_audio.elisa elisa_wolf3d_pagefile.elisa elisa_wolf3d_input.elisa elisa_wolf3d_video.elisa
LEGACY_OBJS := $(filter %.o, $(SRCS:.c=.o) $(SRCS:.cpp=.o)) $(ELISA_SRCS:.elisa=.o)
LEGACY_DEPS := $(filter %.d, $(SRCS:.c=.d) $(SRCS:.cpp=.d))

.SUFFIXES:
.SUFFIXES: .c .cpp .d .o

Q ?= @

all: $(TARGET)

ifndef NO_DEPS
depend: $(DEPS)

ifeq ($(findstring $(MAKECMDGOALS), clean depend Data),)
-include $(DEPS)
endif
endif

$(TARGET): $(OBJS)
	@echo '===> LD $@'
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/id_pm.o: elisa_wolf3d_effects.h elisa_wolf3d_pagefile.h
$(BUILD_DIR)/id_sd.o: elisa_wolf3d_audio.h elisa_wolf3d_effects.h
$(BUILD_DIR)/id_in.o: elisa_wolf3d_effects.h elisa_wolf3d_input.h
$(BUILD_DIR)/id_us_1.o: elisa_wolf3d_ui.h
$(BUILD_DIR)/id_vh.o: elisa_wolf3d_video.h
$(BUILD_DIR)/id_vl.o: elisa_wolf3d_palette.h elisa_wolf3d_effects.h
$(BUILD_DIR)/wl_main.o: elisa_wolf3d_save.h

$(BUILD_DIR)/%.o: %.elisa
	@echo '===> ELISA-O $@'
	$(Q)mkdir -p $(dir $@)
	$(Q)cd $(ELISA_COMPILER_DIR) && go run ./src -emit obj -o "$(abspath $@)" "$(abspath $<)"

$(BUILD_DIR)/%.o: %.c
	@echo '===> CC $<'
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@echo '===> CXX $<'
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.d: %.c
	@echo '===> DEP $<'
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CCFLAGS) -MM -MT "$(@:.d=.o) $@" $< > $@

$(BUILD_DIR)/%.d: %.cpp
	@echo '===> DEP $<'
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) -MM -MT "$(@:.d=.o) $@" $< > $@

clean distclean:
	@echo '===> CLEAN'
	$(Q)rm -fr $(BUILD_DIR) $(LEGACY_OBJS) $(LEGACY_DEPS) $(BINARY)

elisa-tests:
	@set -e; \
	for src in $(ELISA_TEST_SRCS); do \
		echo '===> ELISA-TEST' $$src; \
		cd $(ELISA_COMPILER_DIR) && go run ./src -emit test "$(abspath $$src)"; \
	done

install: $(TARGET)
	@echo '===> INSTALL'
	$(Q)$(INSTALL) -d $(PREFIX)/bin
	$(Q)$(INSTALL_PROGRAM) $(TARGET) $(PREFIX)/bin
