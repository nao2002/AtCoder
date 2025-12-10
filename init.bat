@echo off
setlocal enabledelayedexpansion
chcp 65001 > nul

@REM コンテスト名を入力 (例: ABC001)
set /p contest_number="コンテスト番号を入力してください:"
set contest_name=abc%contest_number%
set folder_path="contest/%contest_name%"

@REM gitのブランチ作成と更新
git checkout main
git -p pull
git checkout -b %contest_name% main

mkdir %folder_path%

call :COPYFILE a
call :COPYFILE b
call :COPYFILE c
call :COPYFILE d
call :COPYFILE e
call :COPYFILE f

git add %folder_path%/.
git commit -m "add: ファイル追加"

exit

@REM ファイルコピー 引数:問題番号(a,b,c...)
:COPYFILE
set filename=%contest_number%%1
> "%folder_path%/%1.py" (
  for /f "usebackq delims=" %%A in ("template/Template.py") do (
    set "line=%%A"
    rem <number>を置き換え
    set "line=!line:<number>=%filename%!"
    echo(!line!
  )
)