# Projman
## A C Project generator

This tool allow for the easy creation of makefiles, header files and source files.
It also adds a .gitignore

### Usage:
```console
projman -c <project name>: Creates a new project directory in the current working  directory
				-h Displays this help
				-m <module name> Add module (.h and .c file) and adds it to the makefile
				-mc <module name> Add module (.c file only) and adds it to the makefile
				-mh <module name> Add stb-style module (.h file only)
				-r [args] Build and run the current project with args
				-f forces the makefile to recompile everything
				-i install the project (Can require sudo)
				-b rebuilds the makefile and builds the project
				-bm rebuilds the makefile only

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

Welcome to Projman, the C project creator !

Usage:
projman [options]

Options:
	-c <project name>: Creates a new project directory in the current working  directory
	-h Displays this help
	-m <module name> Add module (.h and .c file) and adds it to the makefile
	-mc <module name> Add module (.c file only) and adds it to the makefile
	-mh <module name> Add stb-style module (.h file only)
	-r [args] Build and run the current project with args
	-f forces the makefile to recompile everything
	-i install the project (Can require sudo)
	-b rebuilds the makefile and builds the project
	-bm rebuilds the makefile only
