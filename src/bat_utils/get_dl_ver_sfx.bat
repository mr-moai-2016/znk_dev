@echo off

REM �ڍ�:
REM   ���Z�I�I�ȏ������s���Ă���.
REM   template_bat__prefix �� '%_BB_PREFIX%' �Ƃ���������
REM   ��ԍŌ�ɉ��s�Ȃ���(�������d�v!)�L�q����Ă���.
REM   (echo�R�}���h�ł͍Ō�Ɏ����I�ɉ��s���}������Ă��܂����߁A
REM   ���̃e���v���[�g�t�@�C���������������邱�Ƃ͂ł��Ȃ�)
REM   ����� type �R�}���h�ňꎞ�t�@�C���֏o�͂�����A
REM   type �Œǉ��o�͂��A�R�}���h�𓮓I�ɐ���������.
REM   ���Ƃ́A���̈ꎞ�t�@�C���� bat �t�@�C���Ƃ��� call���邾���ł���.

set _BB_PREFIX=set
set _BB_BAT_UTILS_DIR=%1
set _BB_VAL_FILE=%2
set _BB_SUFFIX=

echo _BB_VAL_FILE=[%_BB_VAL_FILE%]
if not exist %_BB_VAL_FILE% goto EndOfGen
	type %_BB_BAT_UTILS_DIR%\template_bat__prefix>  __temp_bb_cmd.bat
	type %_BB_BAT_UTILS_DIR%\template_bat__space>>  __temp_bb_cmd.bat
	type %_BB_VAL_FILE%>>                           __temp_bb_cmd.bat
	if not "%_BB_SUFFIX%" == "" echo %_BB_SUFFIX%>> __temp_bb_cmd.bat
	call __temp_bb_cmd.bat
	del  __temp_bb_cmd.bat
:EndOfGen

set DL_VER_SFX=
if not "%DL_VER%" == "" set DL_VER_SFX=-%DL_VER%
