#!/bin/sh
for LANG in `ls locale | grep -v CVS | grep -v po` ; do 
  msgfmt -o locale/$LANG/LC_MESSAGES/pp_lib.mo locale/$LANG/LC_MESSAGES/pp_lib.po
done
