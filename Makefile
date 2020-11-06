
cov-build:
	clang++ -std=c++2a -stdlib=libc++ -O3 -fprofile-instr-generate -fcoverage-mapping -march=native main.cpp -o main

build:
	clang++ -std=c++2a -stdlib=libc++ -O3 -march=native main.cpp -o main_b

instr:
	clang++ -std=c++2a -stdlib=libc++ -O3 -fprofile-instr-use=merged.profdata -march=native main.cpp -o main

merge:
	xcrun llvm-profdata merge -instr *.profraw -o merged.profdata 

show:
	xcrun llvm-profdata show -all-functions merged.profdata  

cov:
	xcrun llvm-cov show main -instr-profile=merged.profdata