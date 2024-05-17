#define NEW_FILE_IMPL
#include "../include/new_file_lib.h"

#include <dirent.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define CC "gcc"
#define CFLAGS "-Wall -Wextra -pedantic"

char *project_name = NULL;
char *prg_name;
char directory[128];

bool streq(char *s1, char *s2) {
  if (s1 == NULL && s2 == NULL)
    return true;
  if (s1 == NULL)
    return false;
  if (s2 == NULL)
    return false;
  return strcmp(s1, s2) == 0;
}

void add_deps_to_makefile(FILE *mfile) {
  fprintf(mfile, "DEPS=");

  DIR *d;
  struct dirent *dir;
  char src_dir[256] = {0};
  sprintf(src_dir, "%s/src", directory);
  d = opendir(src_dir);
  if (!d) {
    fprintf(stderr, "[ERROR]: Bad directory !\n");
    exit(1);
  }
  int i = 0;
  while ((dir = readdir(d)) != NULL) {
    if (i++ > 0) {
      fprintf(mfile, " ");
    }
    char *name = malloc(strlen(dir->d_name) + 1);
    strcpy(name, dir->d_name);
    name[strlen(dir->d_name) - 1] = 'o';
    if (!streq(name, ".o") && !streq(name, "o")) {

      fprintf(mfile, "$(BUILD)%s", name);
    }
    free(name);
  }
  fprintf(mfile, "\n");
  closedir(d);
}

void projman_create_makefile(void) {
  char makefile[256] = {0};

  sprintf(makefile, "%s/Makefile", directory);
  FILE *mfile = fopen(makefile, "w");
  fprintf(mfile, "CC=%s\n", CC);
  fprintf(mfile, "CFLAGS=%s\n", CFLAGS);
  fprintf(mfile, "SRC=src/\n");
  fprintf(mfile, "BUILD=build/\n");
  fprintf(mfile, "BIN=bin/\n\n");
  add_deps_to_makefile(mfile);
  fprintf(mfile, "all: lines %s\n", project_name);
  fprintf(mfile, "lines:\n\t@echo \"C:\"\n\t@wc -l $$( find -wholename "
                 "\'./*.[hc]\') | tail -n 1\n");
  fprintf(mfile, "$(BUILD)%%.o: $(SRC)%%.c\n\t $(CC) $(CFLAGS) -o $@ $^ -c\n");
  fprintf(mfile, "$(BIN)%s: $(DEPS)\n\t$(CC) $(CFLAGS) -o $@ $^\n",
          project_name);
  fprintf(mfile, "%s: $(BIN)%s\n", project_name, project_name);
  fprintf(mfile, "clean:\n\trm -rf $(BIN)*\n\trm -rf $(BUILD)*\n");
  fprintf(mfile, "install:\n\tcp $(BIN)%s /bin/", project_name);
  fclose(mfile);
}

void projman_create_gitignore(void) {
  char filename[256];
  sprintf(filename, "%s/.gitignore", directory);
  FILE *f = fopen(filename, "w");
  fprintf(f, "bin/*\nbuild/*\n*.o");
  fclose(f);
}

void create_src_file(const char *name) {
  char file_path[256] = {0};
  sprintf(file_path, "%s/src/%s.c", project_name, name);
  if (!file_exists(file_path)) {
    FILE *f = fopen(file_path, "w");
    fclose(f);
  }
}

void write_main_template(FILE *mainfile) {
  write_copyright(mainfile, project_name, false);
  fprintf(mainfile, "\n\n");
  fprintf(mainfile, "#include <stdio.h>\n");
  fprintf(mainfile, "\n\n");
  fprintf(mainfile, "int main(int argc, char **argv) {\n");
  fprintf(mainfile, "    (void)argc;\n");
  fprintf(mainfile, "    (void)argv;\n");
  fprintf(mainfile, "    printf(\"Hello, World !\\n\");\n");
  fprintf(mainfile, "    return 0;\n");
  fprintf(mainfile, "}\n");
}

void create_main_file(void) {
  char file_path[256] = {0};
  sprintf(file_path, "%s/src/%s.c", directory, project_name);
  FILE *f = fopen(file_path, "w");
  write_main_template(f);
  fclose(f);
}

void make_structure(void) {
  new_dir(project_name);
  char src[256] = {0};
  char inc[256] = {0};
  char bld[256] = {0};
  char bin[256] = {0};

  sprintf(src, "%s/src", directory);
  sprintf(inc, "%s/include", directory);
  sprintf(bld, "%s/build", directory);
  sprintf(bin, "%s/bin", directory);

  new_dir(src);
  new_dir(inc);
  new_dir(bld);
  new_dir(bin);
}

void projman_init(void) {
  make_structure();
  create_main_file();
  projman_create_makefile();
  projman_create_gitignore();
}

void usage() {
  printf("Welcome to Projman, the C project creator !\n\n");
  printf("Usage:\n");
  printf("%s [options]\n\n", prg_name);
  printf("Options:\n");
  printf("\t-c <project name>: Creates a new project directory in the current "
         "working  directory\n");
  printf("\t-h Displays this help\n");
  printf("\t-m <module name> Add module (.h and .c file) and adds it to the "
         "makefile\n");
  printf("\t-mc <module name> Add module (.c file only) and adds it to the "
         "makefile\n");
  printf("\t-mh <module name> Add stb-style module (.h file only)\n");
  // printf();
}

bool is_create = false;

char actual_dir[128];

int main(int argc, char **argv) {
  prg_name = argv[0];
  getcwd(actual_dir, 128);
  strcpy(actual_dir, basename(actual_dir));
  // First pass, looking for a create
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    if (streq(arg, "-c")) {
      // Create
      is_create = true;
      // if no filename provided
      if (argc <= ++i) {
        usage();
        exit(1);
      }
      project_name = argv[i];
    } else if (streq(arg, "-h")) {
      usage();
      exit(0);
    }
  }
  if (project_name == NULL) {
    project_name = actual_dir;
  }
  if (streq(project_name, actual_dir)) {
    strcpy(directory, "./");
  } else if (project_name != NULL) {
    strcpy(directory, project_name);
  }
  if (is_create) {
    projman_init();
  }
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    if (streq(arg, "-m")) {
      while (++i < argc && argv[i][0] != '-') {
        char *module_name = argv[i];
        new_file(module_name, directory);
      }
    } else if (streq(arg, "-mh")) {
      printf("-MH\n");
      while (++i < argc && argv[i][0] != '-') {
        char *module_name = argv[i];
        new_file_h(module_name, directory);
      }
    } else if (streq(arg, "-mc")) {
      while (++i < argc && argv[i][0] != '-') {
        char *module_name = argv[i];
        new_file_c(module_name, directory);
      }
    }
  }
  projman_create_makefile();

  return 0;
}