if not exist build mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j8
cd ..

echo.
echo === Running config ^& logger test ===
.\bin\manual_test.exe

echo.
echo === Running map writer test ===
.\bin\manual_test_map_writer.exe

echo.
echo === Unpacking map output ===
python _pre-demo\as1_map_unpack.py test_cpp_output.map

@pause