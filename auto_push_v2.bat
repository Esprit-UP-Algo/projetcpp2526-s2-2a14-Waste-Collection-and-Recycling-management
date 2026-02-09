@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   AUTO-DETECT GIT AND PUSH SCRIPT (Gestion-des-commandes)
echo ========================================================

:: --- 1. SEARCH FOR GIT ---
set "GIT_PATH="

:: Check system PATH
where git >nul 2>nul
if %errorlevel% equ 0 (
    set "GIT_PATH=git"
    goto :GitFound
)

:: Check common installation paths
set "PATHS[1]=C:\Program Files\Git\cmd\git.exe"
set "PATHS[2]=C:\Program Files\Git\bin\git.exe"
set "PATHS[3]=C:\Program Files (x86)\Git\cmd\git.exe"
set "PATHS[4]=%LOCALAPPDATA%\Programs\Git\cmd\git.exe"
set "PATHS[5]=%USERPROFILE%\AppData\Local\Programs\Git\cmd\git.exe"

for /L %%i in (1,1,5) do (
    if exist "!PATHS[%%i]!" (
        set "GIT_PATH=!PATHS[%%i]!"
        goto :GitFound
    )
)

echo [ERROR] Could not find git.exe on your computer.
echo Please install Git from https://git-scm.com/download/win
pause
exit /b 1

:GitFound
echo [OK] Found Git at: "!GIT_PATH!"

:: --- 2. CONFIGURE REPO ---
echo.
echo [1/5] Initializing Repository...
"!GIT_PATH!" init

echo [2/5] Configuring Remote...
"!GIT_PATH!" remote remove origin 2>nul
"!GIT_PATH!" remote add origin https://github.com/Esprit-UP-Algo/projetcpp2526-s2-2a14-Waste-Collection-and-Recycling-management.git

:: --- 3. MANAGE BRANCH ---
echo [3/5] Switching to branch 'Gestion-des-commandes'...
:: Try to checkout existing, or create new
"!GIT_PATH!" checkout -b Gestion-des-commandes 2>nul
if %errorlevel% neq 0 (
    "!GIT_PATH!" checkout Gestion-des-commandes
)

:: --- 4. COMMIT ---
echo [4/5] Committing changes...
"!GIT_PATH!" add .
"!GIT_PATH!" commit -m "Update: Red PDF Button, Logo Fix, and UI Improvements"

:: --- 5. PUSH ---
echo [5/5] Pushing to GitHub...
echo.
echo ----------------------------------------------------------------
echo NOTE: A browser window may open to sign in to GitHub.
echo If it asks for a username/password in the terminal:
echo   - Username: Your GitHub username
echo   - Password: Your Personal Access Token (PAT)
echo ----------------------------------------------------------------
echo.
"!GIT_PATH!" push -u origin Gestion-des-commandes

echo.
echo ========================================================
if %errorlevel% equ 0 (
    echo   SUCCESS! Pushed to branch 'Gestion-des-commandes'
) else (
    echo   PUSH FAILED. Check your internet or credentials.
)
echo ========================================================
pause
