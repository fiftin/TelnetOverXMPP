project(telnetoverxmppclient)

set(QT_USE_QTWEBKIT YES)
set(QT_USE_PHONON NO)
set(QT_USE_QTXMLPATTERNS NO) 

set(PLUGIN_NAME "telnetoverxmppclient")
set(PLUGIN_DISPLAY_NAME "Telnet over XMPP client")

include("telnetoverxmppclient.cmake")
include("${CMAKE_SOURCE_DIR}/src/plugins/plugins.cmake")
