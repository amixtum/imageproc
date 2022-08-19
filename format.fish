set files (find . ! -iname "*cmake*" -a \( -iname '*.h' -o -iname '*.cpp' \))
for file in $files
    clang-format -style=Mozilla -i $file
end
