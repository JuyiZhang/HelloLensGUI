gcc -ISymbol -I. `pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` main.c -o main `pkg-config --libs gtk+-3.0 webkit2gtk-4.1`
./main