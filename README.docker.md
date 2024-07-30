# Instalação do docker
```
sudo apt-get install docker
```

# Buildar projeto
```
docker build -t a98r . 
```

# Liberar acesso a interface
```
xhost +local:root
```

# Rodar o projeto
```
docker run -it --rm \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    a98r
```

# Remover acesso a interface
```
xhost -local:root
```