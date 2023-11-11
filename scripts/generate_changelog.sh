#!/bin/bash

COMMIT_URL="https://github.com/Slashcash/enea/commit"

# Get the last two tags if no command-line arguments are provided
if [ "$#" -eq 0 ]; then
  last_tag=$(git describe --tags --abbrev=0)
  previous_tag=$(git describe --tags --abbrev=0 $last_tag^)
else
  previous_tag=$1
  last_tag=$2
fi

echo "# $last_tag"

# Generate changelog between the specified tags and sort by type
git log --pretty=format:"- %s ([commit]($COMMIT_URL/%H))" $previous_tag..$last_tag | grep -E '^(- Feature|- Fix):' | \
  sort -t ' ' -k2,2 | awk '{gsub(/Feature: |Fix: /, ""); $1=toupper(substr(""$0"", 1, 3)); $2=substr(""$0"", 6); print ""$1$2""}'
