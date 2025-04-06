#!/bin/bash
../../../build/WebTrader/AppWebTrader "--docroot=web;/favicon.ico,/resources,/style,/image" --http-address=0.0.0.0 --http-port=8091 --config=etc/wt_config.xml
# required resources:
# cp -r ../libs-build/wt/resources x64/debug/AppWebTrader/web/

