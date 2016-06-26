#!/bin/bash
CWD=`dirname $0`
cd "$CWD"
CWD=`pwd`
git pull || exit 1
cd "$CWD/autonomy/frontend"
make clean all
cd "$CWD/autonomy/backend"
make clean all
cd "$CWD/autonomy/aruco/viewer"
make
