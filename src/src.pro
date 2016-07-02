TEMPLATE = subdirs
backend.file = backend/src/src.pro
app.depends = backend
plugins.depends = backend
SUBDIRS += backend app 
#plugins
