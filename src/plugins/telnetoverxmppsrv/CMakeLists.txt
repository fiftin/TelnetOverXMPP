project(telnetoverxmppsrv)

set(QT_USE_QTWEBKIT YES)
set(QT_USE_PHONON NO)
set(QT_USE_QTXMLPATTERNS NO) 

set(PLUGIN_NAME "telnetoverxmppsrv")
set(PLUGIN_DISPLAY_NAME "TelnetOverXMPPSrv")

include("telnetoverxmppsrv.cmake")
include("${CMAKE_SOURCE_DIR}/src/plugins/plugins.cmake")
