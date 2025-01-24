FROM debian:bookworm

RUN apt-get update
RUN apt-get install -y tmux vim python3 vim-fugitive g++ pipx gdb
RUN apt-get install -y sudo 
RUN pipx install poetry 
RUN pipx install gdbgui
RUN pipx ensurepath
RUN groupadd -g 1000 mattd 
RUN useradd -m -u 1000 -g 1000 mattd
RUN usermod -a -G sudo mattd
RUN mkdir /projects
RUN chown -R mattd:mattd /projects
RUN echo 'mattd ALL=(ALL:ALL) NOPASSWD:ALL' >> /etc/sudoers
USER mattd
COPY tmux.conf /home/mattd/.tmux.conf
WORKDIR projects


