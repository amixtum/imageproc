cd build
cmake ../
cmake --build .
cd ..
rm compile_commands.json
cp ./build/compile_commands.json .
