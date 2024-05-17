# Projman
## A C Project generator

This tool allow for the easy creation of makefiles, header files and source files.
It also cereates a .gitignore

### Usage:
```console
projman -c <project name> # Creates a new project with a .c file with this name
        -m [modules to add (.c and .h files)]
        -mc [source only modules to add]
        -mh [stb style header-only modules to add]
```

### installation
```console
git clone https://github.com/Paul-Passeron/projman.git
cd projman
make
sudo make install
cd ..
rm -rf projman
```

