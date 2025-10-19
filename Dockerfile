FROM debian:stable-slim AS plt-base

ARG USERNAME
ARG USER_UID
ARG USER_GID
ARG DISPLAY


RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

RUN if [ -n "$DISPLAY" ]; then \
        export DISPLAY=$DISPLAY; \
    fi


# Install dependencies


RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    lcov \
    gcovr \
    git

RUN apt-get update && apt-get install -y \
    libsfml-dev \
    libxml2-dev \
    libboost-all-dev

RUN apt-get update && apt-get install -y \
    dia libcanberra-gtk3-module \
    python3-gi gir1.2-gtk-3.0
    
# Set up user and working directory
RUN chsh -s /bin/bash $USERNAME
USER $USERNAME
WORKDIR /app
COPY --exclude=build --exclude=cmake-build-debug --chown=$USERNAME:$USERNAME . /app

# Set environment variables for GUI support
RUN export GDK_BACKEND=$(if [ ! $WAYLAND_DISPLAY ]; then echo x11; else echo wayland; fi)
RUN export QT_QPA_PLATFORM=$(if [ ! $WAYLAND_DISPLAY ]; then echo xcb; else echo wayland; fi)

# Build the project
ENV LC_ALL="C.UTF-8"
ENV LANG="C.UTF-8"

RUN mkdir docker-build && cd docker-build && cmake .. && make client && cd ..

RUN chmod u+x ./entrypoint.sh

SHELL ["/bin/bash", "--login", "-c"]

# Run the application
ENTRYPOINT ["./entrypoint.sh"]
CMD ["tail", "-f", "/dev/null"]