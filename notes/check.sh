# cat check.sh
#!/bin/bash

cd /root/projects/trade-frame/x64/release
# create a state file, if it doesn't exist
if [ ! -e state.txt ]
then
  echo created > state.txt
fi
# test if application is running
NOTIFY=no
if pidof -s -q intervalsampler
then
  if [ "running" != "`cat state.txt`" ]
  then
    echo running > state.txt
    #echo running
    NOTIFY=yes
  fi
else
  if [ "stopped" != "`cat state.txt`" ]
  then
    echo stopped > state.txt
    #echo stopped
    NOTIFY=yes
  fi
fi
# notify on state change
if [ "yes" == "${NOTIFY}" ]
then
  for EMAIL in user1@example.com user2@example.com
  do
    mail --subject="intervalsampler state: `cat state.txt`" ${EMAIL} < state.txt
  done
fi
