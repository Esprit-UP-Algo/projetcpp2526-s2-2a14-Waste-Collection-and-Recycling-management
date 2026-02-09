@echo off
echo ========================================================
echo AUTOMATIC GIT PUSH SCRIPT
echo ========================================================

echo 1. Initializing Git...
git init

echo 2. Adding Remote Repository...
git remote remove origin 2>nul
git remote add origin https://github.com/Esprit-UP-Algo/projetcpp2526-s2-2a14-Waste-Collection-and-Recycling-management.git

echo 3. Switching to branch 'Gestion-des-zones'...
git checkout -b Gestion-des-zones 2>nul
git checkout Gestion-des-zones

echo 4. Staging all files...
git add .

echo 5. Committing changes...
git commit -m "Update: Red PDF Button, Logo Fix, and UI Improvements"

echo 6. Pushing to GitHub...
echo --------------------------------------------------------
echo NOTE: You might be asked to sign in to GitHub in a browser window
echo or enter your username/token below.
echo --------------------------------------------------------
git push -u origin Gestion-des-zones

echo ========================================================
echo DONE. If there were errors, please read the output above.
echo ========================================================
pause
