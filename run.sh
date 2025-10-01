#!/bin/sh
#Accept input
read -p "Enter string: " str
echo
read -p "Enter absolute path to word list: " file
echo 
read -p "Enter absolute path to output folder: " out

#Run Rust
cd Rust
cargo run -- $str "$file" "$out/output-rust.txt"
cd ~-

#Run C++
cd C++
g++ lev.cpp -o lev
./lev $str "$file" "$out/output-cpp.txt"
cd ~-

#Run Go
cd Go
go run lev.go $str "$file" "$out/output-go.txt"
cd ~-

#Run Python
cd Python
python ./lev.py $str "$file" "$out/output-py.txt"
cd ~-

#Run Java
cd Java
javac -cp javatuples-1.2.jar lev.java
java -cp ".:javatuples-1.2.jar" lev $str "$file" "$out/output-java.txt"
cd ~-

#Run Haskell
cd Haskell
runhaskell ./lev.hs $str "$file" "$out/output-haskell.txt"
cd ~-