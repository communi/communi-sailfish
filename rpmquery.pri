defineReplace(rpmquery) {
  return($$system("rpmspec -q --srpm --queryformat '\"%{$$1}\"' $$2"))
}
defineReplace(rpmquery_appspec) {
  return($$rpmquery("$$1", "$$top_srcdir/rpm/harbour-communi.spec"))
}