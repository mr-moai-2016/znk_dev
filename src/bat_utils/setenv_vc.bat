@echo off
echo ===

if "%MACHINE%" == "x64" goto For_x64

REM ------
REM Automatic Recognize
REM
if not "%ZNK_VSCOMNTOOLS%" == "" goto VCUnknown
if not "%VS120COMNTOOLS%"  == "" goto VC120
if not "%VS110COMNTOOLS%"  == "" goto VC110
if not "%VS100COMNTOOLS%"  == "" goto VC100
if not "%VS90COMNTOOLS%"   == "" goto VC90
if not "%VS80COMNTOOLS%"   == "" goto VC80
if not "%VS71COMNTOOLS%"   == "" goto VC71
if not "%VS70COMNTOOLS%"   == "" goto VC70
if not "%ZNK_VC60_DIR%"    == "" goto VC60


echo Error on setenv_vc.bat:
echo Can't find environment variable ZNK_VSCOMNTOOLS.
echo This script recognizes VC by this environment variables set.
echo You must set environment variable ZNK_VSCOMNTOOLS to VC install directory.
pause
exit


REM ------
REM Unknown VC Version...
REM
:VCUnknown
call "%ZNK_VSCOMNTOOLS%\vsvars32.bat"
goto End


REM ------
REM Use VC6.0
REM
:VC60
call "%ZNK_VC60_DIR%\VC98\Bin\VCVARS32.BAT"
goto End


REM ------
REM Use Visual Studio 2002
REM
:VC70
call "%VS70COMNTOOLS%\vsvars32.bat"
goto End


REM ------
REM Use Visual Studio 2003
REM
:VC71
call "%VS71COMNTOOLS%\vsvars32.bat"
goto End


REM ------
REM Use Visual Studio 2005
REM
:VC80
call "%VS80COMNTOOLS%\vsvars32.bat"
goto End


REM ------
REM Use Visual Studio 2008
REM
:VC90
call "%VS90COMNTOOLS%\vsvars32.bat"
goto End


REM ------
REM Use Visual Studio 2010
REM
:VC100
call "%VS100COMNTOOLS%\vsvars32.bat"
goto End


REM ------
REM Use Visual Studio 2012
REM
:VC110
call "%VS110COMNTOOLS%\vsvars32.bat"
goto End


REM ------
REM Use Visual Studio 2013
REM
:VC120
call "%VS120COMNTOOLS%\vsvars32.bat"
goto End



:For_x64

REM ------
REM Automatic Recognize
REM
if not "%VS90COMNTOOLS%"  == "" goto x64_VC90
if not "%VS80COMNTOOLS%"  == "" goto x64_VC80
if not "%VS71COMNTOOLS%"  == "" goto x64_VC71
if not "%VS100COMNTOOLS%" == "" goto x64_VC100

REM ------
REM �Ƃ肠���������_�ł�VC8.0�݂̂̃T�|�[�g�Ƃ���.
REM

REM ------
REM Use Visual Studio 2005 Professional
REM
:x64_VC80
call "%VS80COMNTOOLS%\..\..\VC\bin\amd64\vcvarsamd64.bat"
goto End

:x64_VC71
:x64_VC90
:x64_VC100
echo Error : This version of VC is not supported to x64 in this script.
goto End


:End
if exist setenv_vc_additional.bat call setenv_vc_additional.bat
echo ===
echo INCLUDE=[%INCLUDE%]
echo LIB=[%LIB%]
echo PATH=[%PATH%]
echo ===
