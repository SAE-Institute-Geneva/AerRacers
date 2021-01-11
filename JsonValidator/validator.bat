@Echo off

:Beginning 

REM Read local config
for /f %%f in ('dir /b Json') do set json=Json/%%f
for /f %%f in ('dir /b Validator') do set validator=Validator/%%f
validate_json.exe %json% %validator%
if %errorlevel% == 0 (
   echo Json Valide
) else (
   echo Json Non Valide
)
pause