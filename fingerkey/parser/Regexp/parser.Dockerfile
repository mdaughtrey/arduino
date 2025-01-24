FROM debian:bookworm

RUN apt-get update
RUN apt-get install -y tmux vim python3 vim-fugitive g++ pipx gdb
RUN pipx install poetry 
RUN pipx install gdbgui
RUN pipx ensurepath
RUN groupadd -g 1000 mattd && useradd -m -u 1000 -g mattd mattd
RUN mkdir /projects
RUN chown -R mattd:mattd /projects
USER mattd
WORKDIR projects


