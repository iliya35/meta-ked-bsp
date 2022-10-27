# Building QtWebengine from Qt5.15 fails as libxkbcommon is missing in the build
# dependecies. Let's add this here for now.
DEPENDS += "libxkbcommon"
