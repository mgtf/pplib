#!/bin/sh
xgettext --keyword=_ -keyword=N_ --default-domain=locale/po/pp_lib -C -j base/*.inc pp.inc > /dev/null 2>&1
