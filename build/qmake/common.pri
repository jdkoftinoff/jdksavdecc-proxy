!MICROSUPPORT:MICROSUPPORT=$$TOP/../microsupport

QT -= core gui
CONFIG += link_prl debug
win32:QT += core
win32:CONFIG+=console

TEMPLATE = app


HEADERS += \
    $$files( $$TOP/include/*.h )

INCLUDEPATH += \
    $$TOP/include $$MICROSUPPORT/include

DEPENDPATH +=  \
    $$TOP/include $$MICROSUPPORT/include

unix:LIBS += 

macx:LIBS += -lpcap

win32 {
  CONFIG(release):LIBS += 
  CONFIG(debug):LIBS += 
  pcap:LIBS += pcap
  CONFIG(release):PRE_TARGETDEPS += 
  CONFIG(debug):PRE_TARGETDEPS += 
}

win32:LIBS+=
macx:LIBS+=

unix:PRE_TARGETDEPS += 

macx {
 MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk
 if( !exists( $$MAC_SDK) ) {
  error("The selected Mac OSX SDK does not exist at $$MAC_SDK!")
 }
 macx:QMAKE_MAC_SDK = $$MAC_SDK
}


