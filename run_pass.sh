clang++ -fPIC -shared -o MySimpleLoopPass.so MySimpleLoopPass.cpp   `llvm-config --cxxflags --ldflags --system-libs --libs passes core support`
opt -load-pass-plugin=./MySimpleLoopPass.so \
    -passes="loop(my-simple-loop-pass)" \
    -disable-output < test.ll > /dev/null
