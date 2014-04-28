TEMPLATE = subdirs
SUBDIRS += src

!exists(src/shared/shared.pri)|!exists(src/backend/src/src.pro): \
    error(A Git submodule is missing. Run \'git submodule update --init\' in the project root.)

OTHER_FILES += \
    ui/*.qml \
    ui/cover/*.qml \
    ui/dialogs/*.qml \
    ui/settings/*.qml \
    ui/view/*.qml \
    rpm/harbour-communi.spec \
    src/app/harbour-communi.desktop \
    src/app/harbour-communi.png \
    README.md
