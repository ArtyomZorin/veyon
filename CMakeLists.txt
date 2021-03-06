CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(veyon)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
SET(CMAKE_BUILD_TYPE relwithdebinfo)

IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0009 NEW)
	CMAKE_POLICY(SET CMP0020 NEW)
ENDIF()

INCLUDE(AddFileDependencies)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)
INCLUDE(GNUInstallDirs)
INCLUDE(CotireVeyon)

FIND_PACKAGE(Git)

IF(GIT_FOUND)
    EXECUTE_PROCESS(COMMAND "${GIT_EXECUTABLE}" describe --tags
		            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
					OUTPUT_STRIP_TRAILING_WHITESPACE
					OUTPUT_VARIABLE VERSION_STRING)
	STRING(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION_STRING}")
	STRING(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_STRING}")
	STRING(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION_STRING}")

	# determine build number to use in NSIS installer and resource files
	EXECUTE_PROCESS(COMMAND "${GIT_EXECUTABLE}" describe --tags
		            COMMAND cut -d "-" -f2
					WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
					OUTPUT_STRIP_TRAILING_WHITESPACE
					OUTPUT_VARIABLE VERSION_BUILD)
	IF(NOT VERSION_BUILD GREATER 0)
	    SET(VERSION_BUILD 0)
	ENDIF()

	# Get list of all committers from git history, ordered by number of commits.
	# The CONTRIBUTORS file is used by AboutDialog. This information can be provided
	# with -DCONTRIBUTORS=/path/to/CONTRIBUTORS instead. For instance, to generate
	# this file for version 3.0.2, the command is:
	#   git shortlog -sne v3.0.2 | cut -c8-
	IF(NOT CONTRIBUTORS)
	    SET(CONTRIBUTORS "${CMAKE_BINARY_DIR}/CONTRIBUTORS")
		EXECUTE_PROCESS(COMMAND "${GIT_EXECUTABLE}" shortlog -sne
			            COMMAND cut -c8-
						COMMAND grep -v root
						OUTPUT_FILE "${CONTRIBUTORS}"
						WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
						TIMEOUT 10)
		FILE(RELATIVE_PATH CONTRIBUTORS ${CMAKE_BINARY_DIR}/core ${CONTRIBUTORS})
	ENDIF()

ENDIF()

# can't retrieve version information as not building from Git repository?
IF(NOT VERSION_STRING)
    SET(VERSION_MAJOR 4)
	SET(VERSION_MINOR 0)
	SET(VERSION_PATCH 3)
	SET(VERSION_BUILD 0)
	SET(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
ELSE()
    # remove leading character from tag name
	STRING(REPLACE "v" "" VERSION_STRING "${VERSION_STRING}")
ENDIF()


INCLUDE(DetectMachine)

# check for pthreads and TLS support
IF(WIN32)
    SET(VEYON_HAVE_LIBPTHREAD true)
ELSE(WIN32)
    FIND_PACKAGE(Threads)
	IF(CMAKE_USE_PTHREADS_INIT)
	    SET(VEYON_HAVE_LIBPTHREAD true)
	ENDIF(CMAKE_USE_PTHREADS_INIT)
ENDIF(WIN32)

CHECK_C_SOURCE_COMPILES("static __thread int p = 0; int main() {}" VEYON_HAVE_TLS)

CHECK_INCLUDE_FILES(arpa/inet.h VEYON_HAVE_ARPA_INET_H)
CHECK_INCLUDE_FILES(unistd.h VEYON_HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(fcntl.h VEYON_HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(limits.h VEYON_HAVE_LIMITS_H)
CHECK_INCLUDE_FILES(memory.h VEYON_HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(netdb.h VEYON_HAVE_NETDB_H)
CHECK_INCLUDE_FILES(netinet/in.h VEYON_HAVE_NETINET_IN_H)
CHECK_INCLUDE_FILES(stdint.h VEYON_HAVE_STDINT_H)
CHECK_INCLUDE_FILES(stdlib.h VEYON_HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(stdbool.h VEYON_HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES(string.h VEYON_HAVE_STRING_H)
CHECK_INCLUDE_FILES(strings.h VEYON_HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(sys/ioctl.h VEYON_HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILES(sys/socket.h VEYON_HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILES(sys/time.h VEYON_HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(sys/timeb.h VEYON_HAVE_SYS_TIMEB_H)
CHECK_INCLUDE_FILES(sys/stat.h VEYON_HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(syslog.h VEYON_HAVE_SYSLOG_H)
CHECK_INCLUDE_FILES(unistd.h VEYON_HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(pwd.h VEYON_HAVE_PWD_H)
CHECK_INCLUDE_FILES(sys/types.h VEYON_HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(utmpx.h VEYON_HAVE_UTMPX_H)
CHECK_INCLUDE_FILES(sys/wait.h VEYON_HAVE_SYS_WAIT_H)
CHECK_INCLUDE_FILES(time.h VEYON_HAVE_TIME_H)
CHECK_INCLUDE_FILES(errno.h VEYON_HAVE_ERRNO_H)
CHECK_INCLUDE_FILES(pthread.h VEYON_HAVE_PTHREAD_H)
CHECK_INCLUDE_FILES(sys/ipc.h VEYON_HAVE_SYS_IPC_H)
CHECK_INCLUDE_FILES(sys/shm.h VEYON_HAVE_SYS_SHM_H)
CHECK_INCLUDE_FILES(stdarg.h VEYON_HAVE_STDARG_H)
CHECK_INCLUDE_FILES(signal.h VEYON_HAVE_SIGNAL_H)
CHECK_INCLUDE_FILES(ctype.h VEYON_HAVE_CTYPE_H)
CHECK_INCLUDE_FILES(process.h VEYON_HAVE_PROCESS_H)
CHECK_INCLUDE_FILES(dlfcn.h VEYON_HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(inttypes.h VEYON_HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(vfork.h VEYON_HAVE_VFORK_H)
CHECK_INCLUDE_FILES(linux/fb.h VEYON_HAVE_LINUX_FB_H)
CHECK_INCLUDE_FILES(linux/input.h VEYON_HAVE_LINUX_INPUT_H)
CHECK_INCLUDE_FILES(linux/uinput.h VEYON_HAVE_LINUX_UINPUT_H)
CHECK_INCLUDE_FILES(linux/videodev.h VEYON_HAVE_LINUX_VIDEODEV_H)
CHECK_INCLUDE_FILES(linux/videodev2.h VEYON_HAVE_LINUX_VIDEODEV2_H)
CHECK_INCLUDE_FILES(ws2tcpip.h VEYON_HAVE_WS2TCPIP_H)

IF(VEYON_BUILD_LINUX)
    FIND_PACKAGE(PAM REQUIRED)
	INCLUDE(XdgInstall)
ENDIF()

SET(CMAKE_REQUIRED_LIBRARIES "-lm")

SET(FUNCS dup2 floor ftime geteuid gethostbyname gethostname getpwnam getpwuid getspnam gettimeofday getuid grantpt inet_ntoa initgroups memcmp memcpy memmove memset mkfifo mmap fork pow putenv select seteuid setpgrp setsid setutxent socket strchr strdup strerror strpbrk strrchr strstr uname waitpid)
FOREACH(_func ${FUNCS})
    STRING(TOUPPER "${_func}" fuc)
	CHECK_FUNCTION_EXISTS(${_func} VEYON_HAVE_${fuc})
ENDFOREACH(_func ${FUNCS})

# TODO: VEYON_WORDS_BIGENDIAN

# find required Qt5 modules
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)
FIND_PACKAGE(Qt5LinguistTools REQUIRED)


# find required libraries
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(LZO REQUIRED)
FIND_PACKAGE(QCA REQUIRED)

# FindOpenSSL.cmake in recent versions of CMake will only find the DLLs instead
# of the import libraries if CYGWIN is not set
SET(CYGWIN TRUE)
FIND_PACKAGE(OpenSSL REQUIRED)
UNSET(CYGWIN)

# libraries and functions for LDAP support
FIND_PACKAGE(Ldap REQUIRED)
SET(CMAKE_REQUIRED_INCLUDES lber.h ldap.h)
SET(CMAKE_REQUIRED_LIBRARIES ${Ldap_LIBRARIES})
CHECK_FUNCTION_EXISTS(ldap_start_tls_s HAVE_LDAP_START_TLS_S)
CHECK_FUNCTION_EXISTS(ldap_initialize HAVE_LDAP_INITIALIZE)
CHECK_FUNCTION_EXISTS(ber_memfree HAVE_BER_MEMFREE)
CHECK_FUNCTION_EXISTS(ldap_unbind_ext HAVE_LDAP_UNBIND_EXT)
CHECK_FUNCTION_EXISTS(ldap_extended_operation HAVE_LDAP_EXTENDED_OPERATION)
CHECK_FUNCTION_EXISTS(ldap_extended_operation_s HAVE_LDAP_EXTENDED_OPERATION_S)
CHECK_SYMBOL_EXISTS(ldap_extended_operation ldap.h HAVE_LDAP_EXTENDED_OPERATION_PROTOTYPE)
CHECK_SYMBOL_EXISTS(ldap_extended_operation_s ldap.h HAVE_LDAP_EXTENDED_OPERATION_S_PROTOTYPE)
CHECK_INCLUDE_FILES(ldap.h HAVE_LDAP_H)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)

FIND_PACKAGE(Sasl2)

# find X libraries
IF(VEYON_BUILD_WIN32)
    ADD_DEFINITIONS(-DLIBVNCSERVER_NEED_INADDR_T)
ELSE()
    FIND_PACKAGE(X11 REQUIRED)

	SET(CMAKE_REQUIRED_LIBRARIES ${X11_LIBRARIES} ${X11_XTest_LIB})
	SET(VEYON_HAVE_X11 TRUE)

	IF(X11_XShm_FOUND)
	    SET(VEYON_HAVE_XSHM TRUE)
	ENDIF(X11_XShm_FOUND)
	IF(X11_XTest_FOUND)
	    SET(VEYON_HAVE_XTEST TRUE)
	ENDIF(X11_XTest_FOUND)
	IF(X11_Xinerama_FOUND)
	    SET(VEYON_HAVE_LIBXINERAMA TRUE)
	ENDIF(X11_Xinerama_FOUND)
	IF(X11_Xrandr_FOUND)
	    SET(VEYON_HAVE_LIBXRANDR TRUE)
	ENDIF(X11_Xrandr_FOUND)
	IF(X11_Xi2_FOUND)
	    SET(VEYON_HAVE_LIBXI2 TRUE)
	ENDIF()
	IF(X11_Xfixes_FOUND)
	    SET(VEYON_HAVE_LIBXFIXES TRUE)
	ENDIF(X11_Xfixes_FOUND)
	IF(X11_Xdamage_FOUND)
	    SET(VEYON_HAVE_LIBXDAMAGE TRUE)
	ENDIF(X11_Xdamage_FOUND)

	CHECK_FUNCTION_EXISTS(XReadScreen VEYON_HAVE_SOLARIS_XREADSCREEN)
	CHECK_FUNCTION_EXISTS(FBPMForceLevel VEYON_HAVE_FBPM)
	CHECK_FUNCTION_EXISTS(DPMSForceLevel VEYON_HAVE_DPMS)
	CHECK_FUNCTION_EXISTS(XTestGrabControl VEYON_HAVE_XTESTGRABCONTROL)
	CHECK_FUNCTION_EXISTS(XRecordEnableContextAsync VEYON_HAVE_RECORD)
	CHECK_INCLUDE_FILES(X11/extensions/readdisplay.h VEYON_HAVE_IRIX_XREADDISPLAY)
	CHECK_INCLUDE_FILES(X11/XKBlib.h VEYON_HAVE_XKBLIB_H)
	IF(VEYON_HAVE_XKBLIB_H)
	    CHECK_FUNCTION_EXISTS(XkbSelectEvents VEYON_HAVE_XKEYBOARD)
	ENDIF(VEYON_HAVE_XKBLIB_H)
	SET(VEYON_HAVE_LIBCRYPT FALSE)
ENDIF()
SET(VEYON_ALLOW24BPP TRUE)
SET(VEYON_BACKCHANNEL TRUE)

SET(VEYON_CORE_INCLUDE_DIR core/include)
SET(VEYONCONFIG ${CMAKE_BINARY_DIR}/${VEYON_CORE_INCLUDE_DIR}/veyonconfig.h)
SET(RFBCONFIG ${CMAKE_BINARY_DIR}/${VEYON_CORE_INCLUDE_DIR}/rfb/rfbconfig.h)
SET(X11VNC_CONFIG ${CMAKE_BINARY_DIR}/${VEYON_CORE_INCLUDE_DIR}/config.h)

SET(IN_FILES ${VEYON_CORE_INCLUDE_DIR}/veyonconfig.h nsis/veyon.nsi core/builddata.qrc server/veyon-server.rc service/veyon-service.rc master/data/veyon-master.desktop master/veyon-master.rc configurator/data/veyon-configurator.desktop configurator/data/io.veyon.veyon-configurator.policy configurator/veyon-configurator.rc ctl/veyon-ctl.rc worker/veyon-worker.rc)
FOREACH(f ${IN_FILES})
    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/${f}.in ${CMAKE_BINARY_DIR}/${f} @ONLY)
ENDFOREACH(f ${IN_FILES})

FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${VEYON_CORE_INCLUDE_DIR}/rfb)
IF(${VEYONCONFIG} IS_NEWER_THAN ${RFBCONFIG})
    EXECUTE_PROCESS(COMMAND sed -e "s/VEYON_/LIBVNCSERVER_/g" INPUT_FILE ${VEYONCONFIG} OUTPUT_FILE ${RFBCONFIG})
ENDIF(${VEYONCONFIG} IS_NEWER_THAN ${RFBCONFIG})

IF(${VEYONCONFIG} IS_NEWER_THAN ${X11VNC_CONFIG})
    EXECUTE_PROCESS(COMMAND sed -e "s/VEYON_//g" INPUT_FILE ${VEYONCONFIG} OUTPUT_FILE ${X11VNC_CONFIG})
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions -std=c++11 ${CXXFLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall ${CFLAGS}")
IF(NOT VEYON_BUILD_WIN32)
    SET(SYS_INC SYSTEM)
ENDIF()

ADD_DEFINITIONS(-DLIBVNCSERVER_HAVE_LIBZ)
ADD_DEFINITIONS(-DLIBVNCSERVER_HAVE_LIBJPEG)
ADD_DEFINITIONS(-DLIBVNCSERVER_HAVE_LIBSSL)
ADD_DEFINITIONS(-DLIBVNCSERVER_HAVE_LIBPNG)

ADD_DEFINITIONS(-DQT_DEPRECATED_WARNINGS -DQT_DISABLE_DEPRECATED_BEFORE=0x050600)

SET(3rdparty_DIR ${CMAKE_SOURCE_DIR}/3rdparty)
SET(ultravnc_DIR ${3rdparty_DIR}/ultravnc)
SET(libvncserver_DIR ${3rdparty_DIR}/libvncserver)
SET(x11vnc_DIR ${3rdparty_DIR}/x11vnc)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${VEYON_CORE_INCLUDE_DIR} ${CMAKE_BINARY_DIR}/${VEYON_CORE_INCLUDE_DIR} ${libvncserver_DIR})
INCLUDE_DIRECTORIES(${SYS_INC} ${ZLIB_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${QCA_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${LZO_INCLUDE_DIR})

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/core)

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${VEYON_LIB_DIR}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# make sub-directories
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(auth-helper)
ADD_SUBDIRECTORY(server)
ADD_SUBDIRECTORY(service)
ADD_SUBDIRECTORY(master)
ADD_SUBDIRECTORY(configurator)
ADD_SUBDIRECTORY(ctl)
ADD_SUBDIRECTORY(worker)
ADD_SUBDIRECTORY(plugins)

#
# rules for building localizations
#
FILE(GLOB veyon_LOCALES core/resources/*.ts)
SET(ts_targets "")
SET(qm_targets "")
FILE(GLOB_RECURSE veyon_SOURCES ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.h ${CMAKE_SOURCE_DIR}/*.ui)
STRING(REGEX REPLACE "${CMAKE_SOURCE_DIR}/3rdparty[^;]+;?" "" veyon_SOURCES "${veyon_SOURCES}")

FOREACH(_ts_file ${veyon_LOCALES})
    STRING(REPLACE "${CMAKE_SOURCE_DIR}/core/resources/" "" _ts_target "${_ts_file}")
	STRING(REPLACE ".ts" ".qm" _qm_file "${_ts_file}")
	STRING(REPLACE ".ts" ".qm" _qm_target "${_ts_target}")
	ADD_CUSTOM_TARGET(${_ts_target} COMMAND ${Qt5_LUPDATE_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/core/include -locations none -no-obsolete ${veyon_SOURCES} -ts ${_ts_file})
	# add command and target for generating/updating QM file if TS file is newer or no QM file exists yet
	ADD_CUSTOM_COMMAND(OUTPUT ${_qm_file} COMMAND ${Qt5_LRELEASE_EXECUTABLE} ${_ts_file} -qm ${_qm_file} DEPENDS ${_ts_file})
	ADD_CUSTOM_TARGET(${_qm_target} ALL DEPENDS ${_qm_file})
	LIST(APPEND ts_targets "${_ts_target}")
	LIST(APPEND qm_targets "${_qm_target}")
ENDFOREACH(_ts_file ${veyon_LOCALES})

ADD_CUSTOM_TARGET(update-locales)
FOREACH(_item ${ts_targets})
    ADD_DEPENDENCIES(update-locales ${_item})
ENDFOREACH(_item ${ts_targets})

ADD_CUSTOM_TARGET(GenerateTranslationFiles)
FOREACH(_item ${qm_targets})
    ADD_DEPENDENCIES(GenerateTranslationFiles ${_item})
ENDFOREACH(_item ${qm_targets})


#
# add target for generating Windows installer
#

IF(WIN32)
    SET(InterceptionZip ${CMAKE_BINARY_DIR}/Interception.zip)
	SET(InterceptionInstaller ${CMAKE_BINARY_DIR}/install-interception.exe)
	IF (NOT EXISTS "${MY_DOWNLOAD_PATH}")
	    FILE(DOWNLOAD "https://github.com/oblitum/Interception/releases/download/v1.0.1/Interception.zip" ${InterceptionZip} SHOW_PROGRESS EXPECTED_HASH SHA256=e9da708d334d1b5d9369ddb57dc8b63b2da1f5fe3ce9d14a24aa95d140d1bfbe)
	ENDIF()

	SET(TMP "veyon-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_BUILD}")

	ADD_CUSTOM_TARGET(win-nsi
		    COMMAND make
			COMMAND rm -rf ${TMP}
			COMMAND mkdir -p ${TMP}
			COMMAND unzip -j -d ${TMP} ${InterceptionZip} Interception/command\ line\ installer/install-interception.exe
			COMMAND cp core/veyon-core.dll server/veyon-server.exe service/veyon-service.exe plugins/vncserver/ultravnc-builtin/vnchooks/vnchooks.dll master/veyon-master.exe configurator/veyon-configurator.exe ctl/veyon-ctl.exe worker/veyon-worker.exe ${TMP}
			COMMAND mkdir -p ${TMP}/plugins
			COMMAND cp plugins/*/*.dll ${TMP}/plugins/
			COMMAND cp plugins/*/*/*.dll ${TMP}/plugins/
			COMMAND ${STRIP} ${TMP}/*.dll ${TMP}/*.exe ${TMP}/plugins/*
			COMMAND cp -r ${CMAKE_SOURCE_DIR}/contrib ${TMP}
			COMMAND cp ${CMAKE_SOURCE_DIR}/COPYING ${TMP}
			COMMAND cp ${CMAKE_SOURCE_DIR}/COPYING ${TMP}/LICENSE.TXT
			COMMAND cp ${CMAKE_SOURCE_DIR}/README.md ${TMP}/README.TXT
			COMMAND todos ${TMP}/*.txt ${TMP}/*.TXT
			COMMAND cp nsis/veyon.nsi ${TMP}
			COMMAND makensis ${TMP}/veyon.nsi
			COMMAND mv ${TMP}/veyon-*setup.exe .
			#COMMAND rm -rf ${TMP}
		)
ENDIF()

#
# package generation
#
INCLUDE(cmake/CPackDefinitions.cmake)



#
# display configuration information
#

MESSAGE("\n"
"Veyon build summary\n"
"--------------------\n"
"* Version                     : ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_BUILD} (${VERSION_STRING})\n"
"* Install prefix              : ${CMAKE_INSTALL_PREFIX}\n"
"* Library & plugin directory  : ${CMAKE_INSTALL_PREFIX}/${VEYON_LIB_DIR}\n"
"* Build type                  : ${CMAKE_BUILD_TYPE}\n"
"* Platform                    : ${CMAKE_SYSTEM_PROCESSOR} (${Machine})\n"
"* Compile flags               : ${CMAKE_C_FLAGS} (CXX: ${CMAKE_CXX_FLAGS})\n"
)

