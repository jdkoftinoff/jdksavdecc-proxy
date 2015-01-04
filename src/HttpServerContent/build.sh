#! /bin/bash -e

rm -f all_content.h

for i in *.html *.css *.jpg *.png *.js; do
    if [ -f "$i" ]; then
        xxd -i "$i" >>all_content.h
    fi
done

