@ECHO OFF

REM ***************************************
REM   Cleanup
REM ***************************************

IF EXIST Cust RD Cust /S/Q
FOR %%i IN (ZIP, TXT) DO DEL *.%%i