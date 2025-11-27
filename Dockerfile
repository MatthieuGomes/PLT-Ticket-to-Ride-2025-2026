FROM debian:bullseye-slim AS plt-base

ARG USERNAME
ARG USER_UID
ARG USER_GID
ARG DISPLAY
ARG WAYLAND_DISPLAY


RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

# Install dependencies


RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    cmake \
    lcov \
    gcovr \
    git

RUN apt-get update && apt-get install -y \
    libsfml-dev \
    libxml2-dev \
    libboost1.74-all-dev

RUN apt-get update && apt-get install -y \
    dia libcanberra-gtk3-module \
    python3-gi gir1.2-gtk-3.0
    
# Set up user and working directory
RUN chsh -s /bin/bash $USERNAME
USER $USERNAME
WORKDIR /app
COPY --exclude=build --exclude=cmake-build-debug --chown=$USERNAME:$USERNAME . /app

# Set environment variables for GUI support
RUN [ -z "$WAYLAND_DISPLAY" ] && echo 'export DISPLAY='"$DISPLAY" >> ~/.bashrc && export DISPLAY=$DISPLAY || echo 'export WAYLAND_DISPLAY='"$WAYLAND_DISPLAY" >> /home/plt/.bashrc && export WAYLAND_DISPLAY=$WAYLAND_DISPLAY 
RUN [ -z "$WAYLAND_DISPLAY" ] && echo 'export GDK_BACKEND=x11' >> ~/.bashrc && export GDK_BACKEND=x11 || echo "export GDK_BACKEND=wayland" >> /home/plt/.bashrc && export GDK_BACKEND=wayland
RUN [ -z "$WAYLAND_DISPLAY" ] && echo 'export QT_QPA_PLATFORM=xcb' >> ~/.bashrc && export QT_QPA_PLATFORM=xcb || echo "export QT_QPA_PLATFORM=wayland" >> /home/plt/.bashrc && export QT_QPA_PLATFORM=wayland

# Build the project
ENV LC_ALL="C.UTF-8"
ENV LANG="C.UTF-8"

#RUN mkdir docker-build && cd docker-build && cmake .. && make client && cd ..

SHELL ["/bin/bash", "--login", "-c"]

# Run the application
CMD ["tail", "-f", "/dev/null"]