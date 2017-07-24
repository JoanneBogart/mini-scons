#!/bin/sh

if [ "x$PFILES" != x ]
then
  foundSemi=`echo $PFILES | sed "s%.*;.*%;%"`
#  echo $foundSemi
  if [ "x$foundSemi" = "x;" ]
  then
    export PFILES=${PFILES}:${INST_DIR}/syspfiles
  else
    export PFILES=${PFILES}";"${INST_DIR}/syspfiles
  fi
else
  export PFILES=".;"${INST_DIR}/syspfiles
fi

if [ ${INST_DIR} != ${BASE_DIR} ]
then
  export PFILES=${PFILES}:${BASE_DIR}/syspfiles
fi


