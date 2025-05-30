# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BUILD_SOURCE_DIRS "C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/blds/zlib-ng/src/2.2.3-b95bb615e6.clean;C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/blds/zlib-ng/x64-windows-dbg")
set(CPACK_CMAKE_GENERATOR "Ninja")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "C:/Program Files/CMake/share/cmake-3.31/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "zlib built using CMake")
set(CPACK_GENERATOR "TGZ")
set(CPACK_INNOSETUP_ARCHITECTURE "x64")
set(CPACK_INSTALL_CMAKE_PROJECTS "C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/blds/zlib-ng/x64-windows-dbg;zlib;ALL;/")
set(CPACK_INSTALL_PREFIX "C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/pkgs/zlib-ng_x64-windows/debug")
set(CPACK_MODULE_PATH "")
set(CPACK_NSIS_DISPLAY_NAME "zlib-ng 2.2.3")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_PACKAGE_NAME "zlib-ng 2.2.3")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OUTPUT_CONFIG_FILE "C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/blds/zlib-ng/x64-windows-dbg/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Program Files/CMake/share/cmake-3.31/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "zlib built using CMake")
set(CPACK_PACKAGE_DIRECTORY "C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/blds/zlib-ng/x64-windows-dbg/package")
set(CPACK_PACKAGE_FILE_NAME "zlib-ng-2.2.3-win64")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "zlib-ng 2.2.3")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "zlib-ng 2.2.3")
set(CPACK_PACKAGE_NAME "zlib-ng")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "Humanity")
set(CPACK_PACKAGE_VERSION "2.2.3")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PRODUCTBUILD_DOMAINS "ON")
set(CPACK_RESOURCE_FILE_LICENSE "C:/Program Files/CMake/share/cmake-3.31/Templates/CPack.GenericLicense.txt")
set(CPACK_RESOURCE_FILE_README "C:/Program Files/CMake/share/cmake-3.31/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files/CMake/share/cmake-3.31/Templates/CPack.GenericWelcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES ".git/;_CPack_Packages/;C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/blds/zlib-ng/x64-windows-dbg/")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/blds/zlib-ng/x64-windows-dbg/CPackSourceConfig.cmake")
set(CPACK_SYSTEM_NAME "win64")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "win64")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "C:/Users/jimmy/source/repos/osu!sync.cpp/osu!sync.core/vcpkg_installed/vcpkg/blds/zlib-ng/x64-windows-dbg/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
