@echo off
SETLOCAL

:: Check for PowerShell availability for downloading the file
echo Checking for PowerShell...
SET PS_AVAILABLE=N
powershell -Command "Exit" >NUL
if ERRORLEVEL 0 (
  SET PS_AVAILABLE=Y
) ELSE (
  SET PS_AVAILABLE=N
)

:: Download firmware.bin if it doesn't exist and PowerShell is available
if not exist "firmware.bin" (
    if "%PS_AVAILABLE%"=="Y" (
        echo Downloading latest firmware.bin...
        powershell -Command "Invoke-WebRequest -Uri 'https://github.com/ChrisDeadman/usb-to-ps2-gameport-hat/releases/download/v1.4/firmware.bin' -OutFile 'firmware.bin'"
    ) else (
        echo PowerShell is not available, please manually download firmware.bin from the GitHub releases page.
        goto end
    )
)

:: Download bossa-x64-1.9.1.msi if it doesn't exist and PowerShell is available
if not exist "bossa-x64-1.9.1.msi" if not exist "BOSSA" (
    if "%PS_AVAILABLE%"=="Y" (
        echo Downloading bossa-x64-1.9.1.msi...
        powershell -Command "Invoke-WebRequest -Uri 'https://github.com/shumatech/BOSSA/releases/download/1.9.1/bossa-x64-1.9.1.msi' -OutFile 'bossa-x64-1.9.1.msi'"
    ) else (
        echo PowerShell is not available, please manually download bossa-x64-1.9.1.msi from the BOSSA GitHub releases page.
        goto end
    )
)

:: Extract the MSI file if it exists and the BOSSA directory doesn't exist
if exist "bossa-x64-1.9.1.msi" if not exist "BOSSA" (
    echo Extracting bossa-x64-1.9.1.msi...
    msiexec /a bossa-x64-1.9.1.msi /qb TARGETDIR="%CD%\BOSSA"
    if ERRORLEVEL 1 (
        echo Extraction of bossa-x64-1.9.1.msi failed. Please check the MSI file and try again.
        goto end
    )
)

:: Check for available COM ports using MODE or REG QUERY as a fallback
echo Checking available COM ports...
set COM_PORTS_FOUND=N
FOR /F "tokens=4 delims=:" %%A IN ('mode ^| find "COM"') DO (
    SET COM_PORTS_FOUND=Y
    echo %%A
)

IF "%COM_PORTS_FOUND%"=="N" (
    echo Attempting registry query for COM ports...
    for /f "tokens=3" %%a in ('reg query HKLM\HARDWARE\DEVICEMAP\SERIALCOMM') do (
        SET COM_PORTS_FOUND=Y
        echo %%a
    )
)

:: If no COM ports are found, inform the user and exit
IF "%COM_PORTS_FOUND%"=="N" (
    echo Warning: No COM ports found.
)

:: Ask user to specify COM port
echo.
echo Please enter the COM port to use (e.g., COM1):
set /p COMPORT=

:: Check if COM port is specified and if bossac.exe exists, then run the tool
if not "%COMPORT%"=="" (
    if exist "%CD%\BOSSA\BOSSA\bossac.exe" (
        echo Running BOSSA on %COMPORT%...
        "%CD%\BOSSA\BOSSA\bossac.exe" --port=%COMPORT% -U -i --offset=0x2000 -e -w "firmware.bin" -R
        if ERRORLEVEL 1 (
            echo Failed on COM port %COMPORT%
        ) else (
            echo Successfully programmed via COM port %COMPORT%
        )
    ) else (
        echo bossac.exe not found, please ensure the BOSSA software was extracted properly.
    )
) else (
    echo No COM port specified, exiting.
)

:end
echo Script execution finished.
pause
