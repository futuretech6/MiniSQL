To build:

```bash
clang++ -std=c++11 -O3 -o ./MiniSQL -I ./Src/ -I ./Src/IndexManager/ -I ./Src/BufferManager/ -I ./Src/CatalogManager/ -I ./Src/RecordManager/ -I ./Src/Interpreter/ ./Src/main.cpp ./Src/IndexManager/IndexManager.cpp ./Src/BufferManager/BufferManager.cpp ./Src/CatalogManager/CatalogManager.cpp ./Src/RecordManager/RecordManager.cpp ./Src/Interpreter/Interpreter.cpp
```

or

```bash
mkdir build
cd ./build/
cmake ../Src/
make
cd ..
```