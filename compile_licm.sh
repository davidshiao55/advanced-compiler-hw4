# Compile the LICM pass plugin
clang++ -fPIC -shared -o MySimpleLICMPass.so MySimpleLICMPass.cpp \
    `llvm-config --cxxflags --ldflags --system-libs --libs core passes analysis support`
