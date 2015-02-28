# include this pri to have the epd directory deployed to your
# target directory

build_script.depends = FORCE
win32:build_script.commands=\"$$PWD/build_script.bat\" \"$$PWD/epd\"
unix:build_script.commands=\"$$PWD/build_script.sh\" \"$$PWD/epd\"

QMAKE_EXTRA_TARGETS += build_script
PRE_TARGETDEPS += build_script

