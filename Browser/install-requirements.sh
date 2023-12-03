sudo apt install ninja ruby libgcrypt20-dev libxml2-dev libsqlite3-dev unifdef libsoup-3.0-dev libmanette-0.2-dev libxslt1-dev libsecret-1-dev gobject-introspection gi-docgen libtasn1-6-dev libwpe-1.0-dev libwpebackend-fdo-1.0-dev libenchant-2-dev libxt-dev libjxl-dev libhyphen-dev libopenjp-2-7-dev woff2 libavif-dev libseccomp-dev xdg-dbus-proxy libgstreamer-plugins-bad1.0-dev gperf gettext
sudo apt install ./gstreamer1.0-wpe_1.22.4-1_arm64.deb
sudo apt install ./libwoff-dev_1.0.2-2_arm64.deb
cmake -DPORT=GTK -DCMAKE_BUILD_TYPE=RelWithDebInfo -GNinja