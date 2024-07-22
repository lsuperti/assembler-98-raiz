
Em sistemas UNIX : 
Passos para build : Dependencias GTK4, cmake, make 
- mkdir build
- cd build
- cmake ..
- make
- ./A9R

No windows :
https://www.gtk.org/docs/installations/windows
Passos para build : Dependencias MSYS2, GTK4, cmake, make, Ninja
- Dentro do MSYS2 -> pacman -S mingw-w64-ucrt-x86_64-gtk4
- mkdir build
- cd build
- cmake -G Ninja .. | ou | cmake -G "MinGW Makefiles" | ou | cmake .. 
- cmake --build .

Instalando GTK4 no ubuntu : 
- sudo apt update
- sudo apt install libgtk-4-dev

ou 

- sudo add-apt-repository ppa:gnome3-team/gnome3-staging
- sudo apt update
- sudo apt install libgtk-4-dev
