export ARCHS = arm64 arm64e
GO_EASY_ON_ME = 1
PACKAGE_VERSION = 0.0.17

TARGET = iphone:clang:15.0
THEOS_LAYOUT_DIR_NAME = layout-rootless
THEOS_PACKAGE_SCHEME = rootless

export SYSROOT = $(THEOS)/sdks/iPhoneOS15.0.sdk

include $(THEOS)/makefiles/common.mk

TWEAK_NAME = DVirtualHome
DVirtualHome_FILES = Tweak.xm
DVirtualHome_FRAMEWORKS = UIKit AudioToolbox

include $(THEOS_MAKE_PATH)/tweak.mk

after-install::
	install.exec "killall -9 SpringBoard"
SUBPROJECTS += dvirtualhome
include $(THEOS_MAKE_PATH)/aggregate.mk
