#!/bin/sh
for LANG in `ls locale | grep -v CVS | grep -v po` ; do \
  msgmerge locale/$LANG/LC_MESSAGES/pp_lib.po locale/po/pp_lib.po > locale/$LANG/LC_MESSAGES/pp_lib.po.new 2> /dev/null; 
  mv locale/$LANG/LC_MESSAGES/pp_lib.po.new locale/$LANG/LC_MESSAGES/pp_lib.po
done
