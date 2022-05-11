#!/bin/sh
top_src=$(git rev-parse --show-toplevel)

cd "$top_src" || exit $?

osc -A https://build.sailfishos.org/ co sailfishos:chum:testing communi-sailfish --output-dir=chum_package

tag=$(./scripts/create_nightly_tag.sh)


sed \
    -e "s|<param name=\"revision\">.*|<param name=\"revision\">$tag</param>|" \
    -i chum_package/_service

cd chum_package || exit $?

osc commit -m "Update to $tag" _service

cd "$top_src" || exit $?

rm -rf obs_package
