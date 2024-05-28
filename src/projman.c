#include <linux/limits.h>
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
#define CFLAGS "-Wall -Wextra -pedantic -g"

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
char src_dir[256] = {0};

void create_init_directories(FILE *mfile, DIR *d, int depth, char *old_path) {
  if (depth <= 0)
    return;
  struct dirent *dir;
  while ((dir = readdir(d)) != NULL) {
    if (strcmp(dir->d_name, ".") == 0)
      continue;
    if (strcmp(dir->d_name, "..") == 0)
      continue;
    if (dir->d_type == DT_REG) {
      continue;
    } else {
      char build_dir[PATH_MAX] = {0};
      if (streq(directory, "./")) {
        sprintf(build_dir, "build");
      } else
        sprintf(build_dir, "%s/build", directory);
      char p[PATH_MAX] = "";
      strcpy(p, old_path);
      strcat(p, "/");
      strcat(p, dir->d_name);

      char to_create[PATH_MAX] = "";
      strcat(to_create, "build/");
      size_t to_remove = 0;
      if (streq(src_dir, "./")) {
        to_remove = strlen("src/");
      } else
        to_remove = strlen(src_dir) + 1;
      strcat(to_create, p + to_remove);
      // new_dir(to_create);
      fprintf(mfile, "mkdir %s\n\t", to_create);
      DIR *nd = opendir(p);
      create_init_directories(mfile, nd, depth - 1, p);
      closedir(nd);
    }
  }
}

void print_deps(FILE *mfile, DIR *d, int depth, char *old_path) {
  if (depth <= 0)
    return;
  struct dirent *dir;

  int i = 0;
  while ((dir = readdir(d)) != NULL) {
    if (i++ > 0) {
      fprintf(mfile, " ");
    }
    if (strcmp(dir->d_name, ".") == 0)
      continue;
    if (strcmp(dir->d_name, "..") == 0)
      continue;
    if (dir->d_type == DT_REG) {
      char *name = malloc(strlen(dir->d_name) + strlen(old_path) + 2);
      memset(name, 0, strlen(dir->d_name) + strlen(old_path) + 2);
      strcat(name, old_path);
      strcat(name, "/");
      strcat(name, dir->d_name);
      if (strlen(dir->d_name) <= 2) {
        free(name);
        continue;
      }
      if (name[strlen(name) - 2] != '.') {
        free(name);
        continue;
      }
      if (name[strlen(name) - 1] != 'c') {
        free(name);
        continue;
      }
      name[strlen(name) - 1] = 'o';
      if (!streq(name, ".o") && !streq(name, "o")) {
        bool has_point = false;
        for (size_t i = 0; i < strlen(name); i++) {
          if (name[i] == '.') {
            has_point = true;
            break;
          }
        }
        if (has_point) {
          size_t to_remove = 0;
          if (streq(src_dir, "./")) {
            to_remove = strlen("src/");
          } else
            to_remove = strlen(src_dir) + 1;
          // strcat(to_create, p + to_remove);
          fprintf(mfile, "$(BUILD)%s", name + to_remove);
        }
      }
      free(name);
    } else {
      char build_dir[PATH_MAX] = {0};
      if (streq(directory, "./")) {
        sprintf(build_dir, "build");
      } else
        sprintf(build_dir, "%s/build", directory);
      char p[PATH_MAX] = "";
      strcpy(p, old_path);
      strcat(p, "/");
      strcat(p, dir->d_name);

      char to_create[PATH_MAX] = "";
      strcat(to_create, "build/");
      size_t to_remove = 0;
      if (streq(src_dir, "./")) {
        to_remove = strlen("src/");
      } else
        to_remove = strlen(src_dir) + 1;
      strcat(to_create, p + to_remove);
      new_dir(to_create);
      DIR *nd = opendir(p);
      print_deps(mfile, nd, depth - 1, p);
      closedir(nd);
    }
  }
}

void add_deps_to_makefile(FILE *mfile) {
  fprintf(mfile, "DEPS=");

  DIR *d;
  if (streq(directory, "./")) {
    sprintf(src_dir, "src");
  } else
    sprintf(src_dir, "%s/src", directory);
  d = opendir(src_dir);
  if (!d) {
    fprintf(stderr, "[ERROR]: Bad directory !\n");
    exit(1);
  }
  print_deps(mfile, d, 3, src_dir);
  fprintf(mfile, "\n");
  closedir(d);
}

void projman_create_makefile(void) {
  char makefile[256] = {0};

  sprintf(makefile, "%s/Makefile", directory);
  FILE *mfile = fopen(makefile, "w");
  fprintf(mfile, "CC=%s\n", CC);
  fprintf(mfile, "CFLAGS=%s\n", CFLAGS);
  fprintf(mfile, "LIBS=");
  FILE *libs = fopen(".projman.libs", "r");
  if (libs != NULL) {
    fseek(libs, 0, SEEK_END);
    size_t l = ftell(libs);
    fseek(libs, 0, SEEK_SET);
    char *tmp = malloc(l + 1);
    fread(tmp, 1, l, libs);
    tmp[l] = 0;
    fprintf(mfile, "%s", tmp);
    free(tmp);
  }
  fprintf(mfile, "\n");
  fprintf(mfile, "SRC=src/\n");
  fprintf(mfile, "BUILD=build/\n");
  fprintf(mfile, "BIN=bin/\n\n");
  add_deps_to_makefile(mfile);
  fprintf(mfile, "all: init lines %s\n", project_name);
  fprintf(mfile, "lines:\n\t@echo \"C:\"\n\t@wc -l $$( find -wholename "
                 "\'./*.[hc]\') | tail -n 1\n");
  fprintf(mfile, "$(BUILD)%%.o: $(SRC)%%.c\n\t $(CC) $(CFLAGS) -o $@ $^ -c\n");
  fprintf(mfile, "$(BIN)%s: $(DEPS)\n\t$(CC) $(CFLAGS) -o $@ $^ $(LIBS)\n",
          project_name);
  fprintf(mfile, "%s: $(BIN)%s\n", project_name, project_name);
  fprintf(mfile, "clean:\n\trm -rf $(BIN)*\n\trm -rf $(BUILD)*\n");
  fprintf(mfile, "install:\n\tcp $(BIN)%s /bin/", project_name);
  fprintf(mfile, "\ninit:\n\t");
  DIR *d = opendir(src_dir);
  create_init_directories(mfile, d, 3, src_dir);
  closedir(d);
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
  printf("\t-r [args] Build and run the current project with args\n");
  printf("\t-f forces the makefile to recompile everything\n");
  printf("\t-i install the project (Can require sudo)\n");
  // printf();
}

bool is_create = false;
bool is_forced = false;

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
    } else if (streq(arg, "-f")) {
      is_forced = true;
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
    } else if (streq(arg, "-r")) {
      char cmd[256];
      projman_create_makefile();

      sprintf(cmd, "%s make && %sbin/%s ", is_forced ? "make clean &&" : "",
              directory, project_name);
      while (++i < argc && argv[i][0] != '-') {
        strcat(cmd, argv[i]);
        strcat(cmd, " ");
      }
      printf("[CMD] %s\n", cmd);
      system(cmd);
    } else if (streq(arg, "-i")) {
      char cmd[256];
      projman_create_makefile();

      sprintf(cmd, "%s make && make install", is_forced ? "make clean &&" : "");
      printf("[CMD] %s\n", cmd);
      system(cmd);
    } else if (streq(arg, "-b")) {
      char cmd[256];
      projman_create_makefile();
      sprintf(cmd, "%s make", is_forced ? "make clean &&" : "");
      printf("[CMD] %s\n", cmd);
      system(cmd);
    } else if (streq(arg, "-f")) {
      continue;
    } else if (streq(arg, "-c")) {
      i++;
      continue;
    } else {
      usage();
      exit(1);
    }
  }
  // projman_create_makefile();
  return 0;
}