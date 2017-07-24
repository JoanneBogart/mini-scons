#!/bin/csh

if ($?PFILES) then
  set foundSemi=`echo $PFILES | sed "s%.*;.*%;%"`
#  echo $foundSemi
  if $foundSemi == ";" then
    setenv PFILES ${PFILES}:${INST_DIR}/syspfiles
  else
    setenv PFILES ${PFILES}";"${INST_DIR}/syspfiles
  endif
else
  setenv PFILES ".;"${INST_DIR}/syspfiles
endif

if ${INST_DIR} != ${BASE_DIR} then
  setenv PFILES ${PFILES}:${BASE_DIR}/syspfiles
endif

