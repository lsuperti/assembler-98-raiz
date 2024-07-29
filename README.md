# Passos para build : 

- https://www.gtk.org/docs/installations

Em sistemas UNIX : 

- Dependencias GTK3, cmake, make 

- mkdir build
- cd build
- cmake ..
- make
- ./A9R

Ou rodar o script 

- ./unix-build.sh

No windows :

- Dependencias MSYS2, GTK3, cmake, make, Ninja

Dentro do MSYS2 -> 
- pacman -S mingw-w64-ucrt-x86_64-toolchain base-devel
- pacman -S mingw-w64-ucrt-x86_64-gtk3
- mkdir build
- cd build
- cmake -G Ninja .. | ou | cmake -G "MinGW Makefiles" .. | ou | cmake .. 
- cmake --build .

# Instalando GTK3 no ubuntu : 

- sudo apt update
- sudo apt install libgtk-3-dev

ou 

- sudo add-apt-repository ppa:gnome3-team/gnome3-staging
- sudo apt update
- sudo apt install libgtk-3-dev

# Instalando Glade no ubuntu :  

- glade é para facilitar o desenvolvimento do GTK
- Video explicativo https://www.youtube.com/watch?v=p3I_GMGe2vI
- sudo apt install glade

# Instalando Glade no windows :

-> Dentro do MSYS2 
- pacman -S mingw-w64-x86_64-glade

