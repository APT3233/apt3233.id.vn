#!/bin/bash

PID_FILE="react_pid.txt" 
LOG_FILE="fe_react.log"
DST_DIR="../frontend/build"

if [ "$1" == "start" ]; then
  if [ -f "$PID_FILE" ]; then
    echo "App is already running!"
    exit 1
  fi

  nohup stdbuf -oL serve -s $DST_DIR >> $LOG_FILE 2>&1 & 
  echo $! > $PID_FILE 
  echo "PID $(cat $PID_FILE) is running."

elif [ "$1" == "stop" ]; then
  if [ ! -f "$PID_FILE" ]; then
    echo "PID Not Found!"
    exit 1
  fi    

  pid=$(cat $PID_FILE)

  if ps -p $pid > /dev/null; then
    kill $pid  
    echo "$pid Stopped."
    rm $PID_FILE
  else
    echo "Process with PID $pid not found or already stopped."
  fi

else
  echo "Command is incorrect!"
fi
