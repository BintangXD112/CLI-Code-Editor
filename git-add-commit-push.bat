@echo off
setlocal enabledelayedexpansion

REM Dapatkan branch saat ini
git rev-parse --abbrev-ref HEAD > tmp_branch.txt
set /p BRANCH=<tmp_branch.txt

del tmp_branch.txt

REM Loop semua file yang belum di-track atau sudah diubah
for /f "delims=" %%f in ('git status --porcelain') do (
    set "line=%%f"
    set "file=!line:~3!"
    set "status=!line:~0,2!"
    
    REM Hanya proses file yang belum di-add (??) atau yang diubah ( M, M )
    if "!status!"=="??" (
        git add "!file!"
        git commit -m "Add !file!: initial commit"
        git push origin !BRANCH!
    ) else if "!status!"==" M" (
        git add "!file!"
        git commit -m "Update !file!: update content"
        git push origin !BRANCH!
    )
)

echo Selesai add, commit, dan push satu per satu. 