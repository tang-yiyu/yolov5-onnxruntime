mkdir build

cd build

cmake ..

cmake --build . --config release -j8

./Release/yolo_ort.exe -m ../models/cardetect.onnx -c ../models/carclass.txt --gpu 1