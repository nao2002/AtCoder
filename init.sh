set -euo pipefail

read -r -p "コンテスト番号を入力してください:" contest_number
contest_name="abc$contest_number"
folder_path="contest/$contest_name"

git checkout main
git pull
git checkout -b "$contest_name" main

mkdir "$folder_path"

copyfile() {
  local letter="$1"
  local filename="$contest_number$letter"
  sed "s/<number>/$filename/g" "template/Template.py" > "$folder_path/$letter.py"
}

copyfile a
copyfile b
copyfile c
copyfile d
copyfile e
copyfile f

git add "$folder_path/."
git commit -m "add: ファイル追加"
