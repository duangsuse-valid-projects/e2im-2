LOCAL_PATH := $(call my-dir)
include $(CLEAN_VARS)

LOCAL_MODULE := libfilepin
LOCAL_MODULE_CLASS := EXECUTABLE
LOCAL_MODULE_FILENAME := libfilepin.so
LOCAL_SRC_FILES := filepin.cpp ext2pin/ext2pin.cpp

LOCAL_BUILD_SCRIPT := BUILD_EXECUTABLE
LOCAL_MAKEFILE := $(local-makefile)

my := TARGET_
include $(BUILD_SYSTEM)/build-module.mk
$(call handle-module-built)
