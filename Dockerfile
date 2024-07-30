FROM debian:latest

RUN apt-get update && \
    apt-get install -y \
    gtk+3.0 \
    libgtk-3-dev \
    glade \
    cmake \
    make \
    gcc \
    g++ \
    pkg-config \
    libx11-dev \
    libxcomposite-dev \
    libxdamage-dev \
    libxrandr-dev \
    libxrender-dev \
    libxfixes-dev \
    libxext-dev \
    libcanberra-gtk3-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /app

WORKDIR /app

CMD ["sh", "./unix-build.sh"]
