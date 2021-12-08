TEMPLATE = subdirs
SUBDIRS += src

!no_submodules {
    !exists(src/shared/shared.pri)|!exists(src/backend/src/src.pro)): \
        error(A Git submodule is missing. Run \'git submodule update --init\' in the project root.)
}

OTHER_FILES += \
    qml/*.qml \
    qml/cover/*.qml \
    qml/dialogs/*.qml \
    qml/settings/*.qml \
    qml/view/*.qml \
    rpm/harbour-communi.spec \
    src/app/harbour-communi.desktop \
    src/app/harbour-communi.png \
    CONTRIBUTORS \
    README.org

misc.files = README.md \
           CONTRIBUTORS
misc.path = /usr/share/$${TARGET}
INSTALLS += misc

TRANSLATIONS = translations/harbour-communi_de.ts \
               translations/harbour-communi_en.ts
               # add new translations here

include($$top_srcdir/QMakeFiles/translate.pri)
