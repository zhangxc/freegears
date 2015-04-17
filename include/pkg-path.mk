package-$(CONFIG_PKG_BASEFILES)		+= basefiles
package-$(CONFIG_PKG_ZLIB)		+= libs/zlib
package-$(CONFIG_PKG_OPENSSL)		+= libs/openssl
package-$(CONFIG_PKG_JSONC)		+= libs/libjson-c
package-$(CONFIG_PKG_LIBUBOX)		+= libs/libubox
package-$(CONFIG_PKG_LIBNCURSES)	+= libs/ncurses
package-$(CONFIG_PKG_LIBXML2)		+= libs/libxml2
package-$(CONFIG_PKG_LIBPCRE)		+= libs/pcre
package-$(CONFIG_PKG_LUA)		+= utils/lua
package-$(CONFIG_PKG_UBUS)		+= system/ubus
package-$(CONFIG_PKG_UCI)		+= system/uci
package-$(CONFIG_PKG_UBOX)		+= system/ubox
package-$(CONFIG_PKG_MYSQL)		+= utils/mysql
package-$(CONFIG_PKG_BUSYBOX)		+= utils/busybox
package-$(CONFIG_PKG_LIBCURL)		+= network/utils/curl
package-$(CONFIG_PKG_NGINX)		+= network/services/nginx
package-$(CONFIG_PKG_PHP5)		+= network/services/php5

