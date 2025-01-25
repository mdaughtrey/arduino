FROM debian:bookworm

RUN apt-get update
RUN apt-get install -y tmux vim-nox python3 vim-fugitive g++ pipx gdb
RUN apt-get install -y sudo less git make cmake universal-ctags
RUN groupadd -g 1000 mattd 
RUN useradd -m -u 1000 -g 1000 mattd
RUN usermod -a -G sudo mattd
RUN mkdir /projects
RUN chown -R mattd:mattd /projects
RUN echo 'mattd ALL=(ALL:ALL) NOPASSWD:ALL' >> /etc/sudoers
RUN apt-get install -y curl
USER mattd
COPY tmux.conf /home/mattd/.tmux.conf
COPY vimrc /home/mattd/.vimrc
COPY bashrc /home/mattd/.bashrc
WORKDIR projects
RUN git clone https://github.com/tmux-plugins/tpm ~/.tmux/plugins/tpm
RUN pipx install poetry 
RUN pipx install gdbgui
RUN pipx ensurepath
RUN <<VIMPLUG
curl -fLo ~/.vim/autoload/plug.vim --create-dirs \
    https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim
VIMPLUG

RUN <<YCM
git clone --recurse-submodules https://github.com/ycm-core/YouCompleteMe.git ~/.vim/pack/YouCompleteMe/opt/YouCompleteMe
cd ~/.vim/pack/YouCompleteMe/opt/YouCompleteMe
./install.py --force-sudo 
cd -
YCM

RUN <<FUG
mkdir -p ~/.vim/pack/tpope/start
cd ~/.vim/pack/tpope start
git clone https://tpope.io/vim/fugitive.git
cd -
FUG

