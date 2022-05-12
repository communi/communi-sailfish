TEMPLATE = aux
TARGET = harbour-communi

SIZES = 86 108 128 172

for(size, SIZES) {
    icon_src$${size}x$${size}.depends = $$PWD/artwork/$${TARGET}.svg
    icon_src$${size}x$${size}.target = \
        $$top_builddir/data/icons/$${size}x$${size}/$${TARGET}.png
    icon_src$${size}x$${size}.commands = mkdir -p \
        $$top_builddir/data/icons/$${size}x$${size};cairosvg -f png \
        --output-height $$size \
        --output-width $$size \
        --output $$top_builddir/data/icons/$${size}x$${size}/$${TARGET}.png \
        $$PWD/artwork/$${TARGET}.svg
    icon_src$${size}x$${size}.files = \
        $$top_builddir/data/icons/$${size}x$${size}/$${TARGET}.png
    QMAKE_EXTRA_TARGETS += icon_src$${size}x$${size}
    PRE_TARGETDEPS += \
        $$top_builddir/data/icons/$${size}x$${size}/$${TARGET}.png
}

for(size, SIZES) {
    icon$${size}x$${size}.depends = \
        $$top_builddir/data/icons/$${size}x$${size}/$${TARGET}.png
    icon$${size}x$${size}.files = \
        $$top_builddir/data/icons/$${size}x$${size}/$${TARGET}.png
    icon$${size}x$${size}.path = /usr/share/icons/hicolor/$${size}x$${size}/apps
    icon$${size}x$${size}.CONFIG += no_check_exist
    INSTALLS += icon$${size}x$${size}
}

icon_svg.files = $$PWD/artwork/$${TARGET}.svg
icon_svg.path = /usr/share/icons/hicolor/scalable/apps
INSTALLS += icon_svg

desktop.files = $$PWD/$${TARGET}.desktop
desktop.path = /usr/share/applications
INSTALLS += desktop
