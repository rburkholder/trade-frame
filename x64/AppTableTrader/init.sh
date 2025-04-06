#!/bin/bash
../../../build/TableTrader/AppTableTrader "--docroot=web;/favicon.ico,/resources,/style,/image" --http-address=0.0.0.0 --http-port=8087 --config=etc/wt_config.xml
# required resources:
# cp -r ../libs-build/wt/resources x64/debug/AppTableTrader/web/
# ln -s  /usr/local/share/Wt/resources web/resources
