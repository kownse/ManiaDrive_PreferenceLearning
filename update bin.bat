@echo Installing Php Scripts.

copy raydium\*.php bin
copy raydium\php.ini bin
xcopy /s /e /I /Y /EXCLUDE:exclude raydium\locale bin\locale
xcopy /s /e /I /Y /EXCLUDE:exclude raydium\rayphp bin\rayphp
@pause
