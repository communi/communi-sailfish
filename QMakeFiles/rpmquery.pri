specfile = "harbour-communi.spec"
RPM_SOURCE_DIR = $$system("echo $RPM_SOURCE_DIR")

exists("$$RPM_SOURCE_DIR/$$specfile") {
  specfile = "$$RPM_SOURCE_DIR/$$specfile"
} else {
  specfile = "$$top_srcdir/rpm/$$specfile"
}

defineReplace(rpmquery) {
  return($$system("rpmspec -q --srpm --queryformat '\"%{$$1}\"' $$2"))
}
defineReplace(rpmquery_appspec) {
  return($$rpmquery("$$1", $$specfile))
}