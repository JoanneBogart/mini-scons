@echo off
if defined PFILES (
  call :parsepfiles
) else (
  set PFILES=.;%INST_DIR%\syspfiles
)

:appendbase
if not %INST_DIR% == %BASE_DIR% @set PFILES=%PFILES%:%BASE_DIR%\syspfiles
goto done


:parsepfiles
for /F "usebackq delims=" %%A in (`set PFILES ^| find /C ";"`) do (
  if %%A EQU 0 (set PFILES=%PFILES%;%INST_DIR%\syspfiles) else set PFILES=%PFILES%:%INST_DIR%\syspfiles
)

:done









