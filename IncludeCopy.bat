:: 명령어 콘솔 창에 안띄우기
@echo off

set "SOLUTION=%~1" :: 솔루션 폴더 주소
set "PROJNAME=%~2" :: 프로젝트 이름

set "PROJECT=%SOLUTION%\%PROJNAME%" :: 프로젝트 폴더 주소
set "FULL=%SOLUTION%\Libraries\Include\%PROJNAME%" :: 현재 폴더에 목적 상대 위치를 변수 DST에 대입

:: 해당 PROJECT 폴더의 모든 .h 파일들을 가지고 DST에 옮기기
xcopy "%PROJECT%\*.h" "%FULL%\" /Y /Q /S /R
xcopy "%PROJECT%\*.inl" "%FULL%\" /Y /Q /S /R

:: 복제된 파일들을 읽기 전용으로 설정
attrib +R "%FULL%\*.h"   /S
attrib +R "%FULL%\*.inl" /S

exit /b 0