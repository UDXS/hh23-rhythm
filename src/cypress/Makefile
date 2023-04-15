# https://github.com/Infineon/mtb-example-psoc6-capsense-buttons-slider/blob/master/Makefile

MTB_TYPE=COMBINED
TARGET=CY8CPROTO-062-4343W
APPNAME=mtb-example-psoc6-capsense-buttons-slider
TOOLCHAIN=GCC_ARM
CONFIG=Debug
VERBOSE=
COMPONENTS=
DISABLE_COMPONENTS=
SOURCES=
INCLUDES=
DEFINES=
VFP_SELECT=
CFLAGS=
CXXFLAGS=
ASFLAGS=
LDFLAGS=
LDLIBS=
LINKER_SCRIPT=
PREBUILD=
POSTBUILD=
CY_APP_PATH=
CY_GETLIBS_SHARED_PATH=../
CY_GETLIBS_SHARED_NAME=mtb_shared
CY_COMPILER_PATH=
CY_WIN_HOME=$(subst \,/,$(USERPROFILE))
CY_TOOLS_PATHS ?= $(wildcard \
    $(CY_WIN_HOME)/ModusToolbox/tools_* \
    $(HOME)/ModusToolbox/tools_* \
    /Applications/ModusToolbox/tools_*)
CY_TOOLS_PATHS+=
CY_TOOLS_DIR=$(lastword $(sort $(wildcard $(CY_TOOLS_PATHS))))

ifeq ($(CY_TOOLS_DIR),)
$(error Unable to find any of the available CY_TOOLS_PATHS -- $(CY_TOOLS_PATHS). On Windows, use forward slashes.)
endif

$(info Tools Directory: $(CY_TOOLS_DIR))

include $(CY_TOOLS_DIR)/make/start.mk
