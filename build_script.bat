@echo on
set EPDDIR=%~1
set EPDDIR=%EPDDIR:/=\%

if not exist "epd" mkdir epd
copy /y "%EPDDIR%"\*.epd epd\
