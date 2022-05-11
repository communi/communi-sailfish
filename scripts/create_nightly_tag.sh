#!/bin/sh

# List tags and sort by version number
for tag in $(git tag --list|sort -V) ; do
    case $tag in
        *.99*|*+*|*.r*) : ;; # Exclude tags from nightlys
        *) latest_release_tag=$tag
    esac
done

revs_since_last_release=$(git rev-list --count $latest_release_tag..HEAD)

head_rev_short=$(git rev-parse --short HEAD)

git tag ${latest_release_tag}.r${revs_since_last_release}.${head_rev_short}

git push origin ${latest_release_tag}.r${revs_since_last_release}.${head_rev_short}

echo ${latest_release_tag}.r${revs_since_last_release}.${head_rev_short}
