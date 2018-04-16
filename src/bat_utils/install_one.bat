@echo off
set CP_=xcopy /H /C /Y
set CP_CONFIRM_=xcopy /H /C

REM
REM IF_DST_EXIST_�͊��Ƀt�@�C�������݂���ꍇ�ɂǂ����邩�������w��ł���A
REM �ȉ��̈Ӗ�������.
REM
REM /F : �m�F�Ȃ��ŋ����I�ɏ㏑��.
REM /N : �������Ȃ�.
REM /C : �m�F���b�Z�[�W���o��.
REM
set IF_DST_EXIST_=%1
set FILE_=%2
set SRC_DIR_=%3
set DST_DIR_=%4
if "%FILE_%"    == "" goto End
if "%SRC_DIR_%" == "" goto End
if "%DST_DIR_%" == "" goto End

if not exist %SRC_DIR_%\%FILE_% goto End

if not "%IF_DST_EXIST_%"    == "/F" goto End_F
	%CP_% %SRC_DIR_%\%FILE_% %DST_DIR_%\
:End_F

if not "%IF_DST_EXIST_%"    == "/N" goto End_N
	if not exist %DST_DIR_%\%FILE_% %CP_% %SRC_DIR_%\%FILE_% %DST_DIR_%\
:End_N

if not "%IF_DST_EXIST_%"    == "/C" goto End_C
	%CP_CONFIRM_% %SRC_DIR_%\%FILE_% %DST_DIR_%\
:End_C

:End
