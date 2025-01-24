FROM debian:bookworm

RUN apt-get update
RUN apt-get install -y tmux vim python3 vim-fugitive g++ pipx gdb
RUN apt-get install -y sudo less git make
RUN groupadd -g 1000 mattd 
RUN useradd -m -u 1000 -g 1000 mattd
RUN usermod -a -G sudo mattd
RUN mkdir /projects
RUN chown -R mattd:mattd /projects
RUN echo 'mattd ALL=(ALL:ALL) NOPASSWD:ALL' >> /etc/sudoers
USER mattd
COPY tmux.conf /home/mattd/.tmux.conf
COPY bashrc /home/mattd/.bashrc
WORKDIR projects
RUN git clone https://github.com/tmux-plugins/tpm ~/.tmux/plugins/tpm
RUN pipx install poetry 
RUN pipx install gdbgui
RUN pipx ensurepath
