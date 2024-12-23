clang++ -fPIC -shared -o MySimpleLICMPass.so MySimpleLICMPass.cpp \
    `llvm-config --cxxflags --ldflags --system-libs --libs core passes analysis support`

clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S test.c -o test.ll
opt -S -passes=mem2reg test.ll -o test_mem2reg.ll
clang -O0 test_mem2reg.ll -o test
opt -load-pass-plugin=./MySimpleLICMPass.so -passes="loop(my-simple-licm-pass)" \
    -S < test_mem2reg.ll > test_licm.ll
clang -O0 test_licm.ll -o test_licm

echo "BASELINE"
./test
echo "LICM"
./test_licm

rm test.ll test_mem2reg.ll test_licm.ll test test_licm