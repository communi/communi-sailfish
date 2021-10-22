TS_FILE = $${_PRO_FILE_PWD_}/translations/$${TARGET}.ts
HAVE_TRANSLATIONS = 0

# Translation source directories
TRANSLATION_SOURCE_CANDIDATES = $${_PRO_FILE_PWD_}/src/app $${_PRO_FILE_PWD_}/qml
for(dir, TRANSLATION_SOURCE_CANDIDATES) {
    exists($$dir) {
        TRANSLATION_SOURCES += $$dir
    }
}

# prefix all TRANSLATIONS with the src dir
# the qm files are generated from the ts files copied to out dir
for(t, TRANSLATIONS) {
    TRANSLATIONS_IN  += $${_PRO_FILE_PWD_}/$$t
    TRANSLATIONS_OUT += $${OUT_PWD}/$$t
    HAVE_TRANSLATIONS = 1
}

qm.files = $$replace(TRANSLATIONS_OUT, \.ts, .qm)
qm.path = /usr/share/$${TARGET}/translations
qm.CONFIG += no_check_exist

# update the ts files in the src dir and then copy them to the out dir
qm.commands += lupdate -noobsolete $${TRANSLATION_SOURCES} -ts $${TS_FILE} $$TRANSLATIONS_IN && \
    mkdir -p translations && \
    [ \"$${OUT_PWD}\" != \"$${_PRO_FILE_PWD_}\" -a $$HAVE_TRANSLATIONS -eq 1 ] && \
    cp -af $${TRANSLATIONS_IN} $${OUT_PWD}/translations || :

sailfishapp_i18n_unfinished {
    TRANSLATE_UNFINISHED =
} else {
    TRANSLATE_UNFINISHED = -nounfinished
}

# create the qm files
sailfishapp_i18n_idbased {
    qm.commands += ; [ $$HAVE_TRANSLATIONS -eq 1 ] && lrelease -idbased $${TRANSLATE_UNFINISHED} $${TRANSLATIONS_OUT} || :
} else {
    qm.commands += ; [ $$HAVE_TRANSLATIONS -eq 1 ] && lrelease $${TRANSLATE_UNFINISHED} $${TRANSLATIONS_OUT} || :
}

INSTALLS += qm