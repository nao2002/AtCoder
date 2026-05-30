set -euo pipefail

read -r -p "コンテスト名を入力してください:" contest_name
folder_path="contest/$contest_name"

git checkout main
git pull
git checkout -b "$contest_name" main

mkdir "$folder_path"

copyfile() {
  local letter="$1"
  local filename="$contest_name$letter"
  sed "s/<contest_name>/$filename/g" "template/Template.py" > "$folder_path/$letter.py"
  sed "s/<contest_name>/$filename/g" "template/Template.cpp" > "$folder_path/$letter.cpp"
}

copyfile a
copyfile b
copyfile c
copyfile d
copyfile e
copyfile f

git add "$folder_path/."
git commit -m "add: ファイル追加"
