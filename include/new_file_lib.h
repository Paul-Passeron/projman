#ifndef NEW_FILE_LIB_H
#define NEW_FILE_LIB_H

char *get_upper_filename(const char *fn);
int folder_exists(const char *path);
int file_exists(const char *path);
void new_file(const char *file);
void new_dir(const char *path);

#ifdef NEW_FILE_IMPL

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void new_dir(const char *path) { mkdir(path, 0775); }
// Warning, the result of this function is to be freed
char *get_upper_filename(const char *fn) {
  size_t l = strlen(fn);
  char *buffer = malloc(l + 1);

  for (size_t i = 0; i < l; ++i) {
    buffer[i] = toupper(fn[i]);
  }
  buffer[l] = 0; // null-terminated string
  return buffer;
}

int folder_exists(const char *path) {
  struct stat sb;
  return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

int file_exists(const char *path) { return access(path, F_OK) == 0; }

void new_file(const char *file) {
  char filename_in[128] = {0};
  sprintf(filename_in, "./include/%s.h", file);

  char filename_src[128] = {0};
  sprintf(filename_src, "./src/%s.c", file);

  if (!folder_exists("include") || !folder_exists("src")) {
    new_dir("include");
    new_dir("src");
  }

  if (file_exists(filename_in) || file_exists(filename_src)) {
    return;
  }

  FILE *inc = fopen(filename_in, "w");
  if (inc == NULL) {
    return;
  }
  char *upper_file = get_upper_filename(file);
  fprintf(inc, "// %s HEADER FILE\n", upper_file);
  fprintf(inc, "// Paul Passeron\n\n");
  fprintf(inc, "// File created by the new_file tool !\n\n");
  fprintf(inc, "#ifndef %s_H\n", upper_file);
  fprintf(inc, "#define %s_H\n\n", upper_file);
  fprintf(inc, "#endif // %s_H\n", upper_file);
  fclose(inc);

  FILE *src = fopen(filename_src, "w");
  if (inc == NULL) {
    free(upper_file);
    return;
  }

  fprintf(src, "// %s SOURCE FILE\n", upper_file);
  fprintf(src, "// Paul Passeron\n\n");
  fprintf(src, "#include \"../include/%s.h\"\n\n", file);
  fprintf(src, "// File created by the new_file tool !\n");

  fclose(src);
  free(upper_file);
}
#endif
#endif // NEW_FILE_LIB_H