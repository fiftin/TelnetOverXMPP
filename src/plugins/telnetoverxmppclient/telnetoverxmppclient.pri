
HEADERS += \
    ../telnetoverxmpp/base/telnetoverxmppbase.h \
    ../telnetoverxmpp/base/connectioninfo.h \
    ../telnetoverxmpp/base/connectionbase.h \
    ../telnetoverxmpp/base/message2.h \
    ../telnetoverxmpp/base/filemessage.h \
#    base/telnetoverxmppbase.h \
#    base/connectioninfo.h \
#    base/connectionbase.h \
#    base/message2.h \
#    base/filemessage.h \
    connection.h \
    itelnetoverxmppclient.h \
    telnetoverxmppclient.h \
    remoteterminalwidget.h \
    remoteterminaldialog.h \
    commandlinewidget.h \
    terminalhistorywidget.h

SOURCES += \
    ../telnetoverxmpp/base/telnetoverxmppbase.cpp \
    ../telnetoverxmpp/base/connectionbase.cpp \
    ../telnetoverxmpp/base/message2.cpp\
    ../telnetoverxmpp/base/filemessage.cpp \
#    base/telnetoverxmppbase.cpp \
#    base/connectionbase.cpp \
#    base/message2.cpp\
#    base/filemessage.cpp \
    connection.cpp \
    telnetoverxmppclient.cpp \
    remoteterminalwidget.cpp \
    remoteterminaldialog.cpp \
    commandlinewidget.cpp \
    terminalhistorywidget.cpp


FORMS += \
    remoteterminaldialog.ui \
    remoteterminalwidget.ui
