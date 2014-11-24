#
# Copyright (C) 2013 Zoltan Hubai
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sakura
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/sakura
  SECTION:=utils
  CATEGORY:=Multimedia
  DEPENDS:= 
  TITLE:=Sakura server
  MAINTAINER:=Andy Huang <jchuang1977@gmail.com>
endef

define Package/sakura/description
 Sakura server for rs232
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) 	CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)"
#	$(TARGET_CC) $(TARGET_CFLAGS)  -I$(TOOLCHAIN_DIR)/include -o $(PKG_BUILD_DIR)/sakura $(PKG_BUILD_DIR)/sakura.c
endef

define Package/sakura/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sakura $(1)/usr/bin/
#	$(INSTALL_DIR) $(1)/etc/init.d
#	$(INSTALL_BIN) ./files/sakura.init $(1)/etc/init.d/sakura
endef

define Package/sakura/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || /etc/init.d/sakura enable || true
endef

$(eval $(call BuildPackage,sakura))
