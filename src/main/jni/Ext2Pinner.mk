LOCAL_PATH := $(call my-dir)
include $(CLEAN_VARS)

LOCAL_MODULE := ext2pin
LOCAL_MODULE_CLASS := EXECUTABLE
LOCAL_MODULE_FILENAME := ext2pin.so
LOCAL_SRC_FILES := ext2pin.cpp

LOCAL_BUILD_SCRIPT := BUILD_EXECUTABLE
LOCAL_MAKEFILE := $(local-makefile)

my := TARGET_
include $(BUILD_SYSTEM)/build-module.mk
$(call handle-module-built)
