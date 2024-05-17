#ifndef NEW_FILE_LIB_H
#define NEW_FILE_LIB_H

#include <stdbool.h>
#include <stdio.h>

char *get_upper_filename(const char *fn);
int folder_exists(const char *path);
int file_exists(const char *path);
void new_file(const char *file, const char *dir);
void new_file_h(const char *file, const char *dir);
void new_file_c(const char *file, const char *dir);
void new_dir(const char *path);
void write_copyright(FILE *f, const char *name, bool header);

#ifdef NEW_FILE_IMPL

#include <ctype.h>
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

void write_copyright(FILE *f, const char *name, bool header) {
  char *upper_fn = get_upper_filename(name);
  fprintf(f, "/**\n");
  fprintf(f, " * %s.%c\n", name, header ? 'h' : 'c');
  fprintf(f, " * Copyright (C) 2024 Paul Passeron\n");
  fprintf(f, " * %s %s file\n", upper_fn, header ? "header" : "source");
  fprintf(f, " * Paul Passeron <paul.passeron2@gmail.com>\n");
  fprintf(f, " */\n");
  free(upper_fn);
}

void new_file_c(const char *file, const char *dir) {
  char filename_src[128] = {0};
  char source_name[64] = {0};
  sprintf(source_name, "%s/src", dir);
  sprintf(filename_src, "%s/%s.c", source_name, file);

  if (!folder_exists(source_name)) {
    new_dir(source_name);
  }
  if (file_exists(filename_src)) {
    return;
  }

  FILE *src = fopen(filename_src, "w");
  if (src == NULL) {
    return;
  }

  write_copyright(src, file, false);
  fprintf(src, "\n\n// File created by the new_file tool !\n");

  fclose(src);
}

void new_file_h(const char *file, const char *dir) {
  printf("Here !!!\n");
  char filename_in[128] = {0};
  char include_name[64] = {0};
  sprintf(include_name, "%s/include", dir);
  sprintf(filename_in, "%s/%s.h", include_name, file);

  if (!folder_exists(include_name)) {
    new_dir(include_name);
  }

  if (file_exists(filename_in)) {
    return;
  }

  FILE *inc = fopen(filename_in, "w");
  if (inc == NULL) {
    return;
  }
  char *upper_file = get_upper_filename(file);

  write_copyright(inc, file, true);
  fprintf(inc, "\n// File created by the new_file tool !\n\n");
  fprintf(inc, "#ifndef %s_H\n", upper_file);
  fprintf(inc, "#define %s_H\n\n", upper_file);
  fprintf(inc, "#ifdef %s_IMPL\n", upper_file);
  fprintf(inc, "\n// Implementation...\n\n");
  fprintf(inc, "#endif // %s_IMPL\n\n", upper_file);
  fprintf(inc, "#endif // %s_H\n", upper_file);
  fclose(inc);
  free(upper_file);
}

void new_file(const char *file, const char *dir) {
  char filename_in[128] = {0};
  char include_name[64] = {0};
  char source_name[64] = {0};
  sprintf(include_name, "%s/include", dir);
  sprintf(source_name, "%s/src", dir);
  sprintf(filename_in, "%s/%s.h", include_name, file);

  char filename_src[128] = {0};
  sprintf(filename_src, "%s/%s.c", source_name, file);

  if (!folder_exists(include_name)) {
    new_dir(include_name);
  }
  if (!folder_exists(source_name)) {
    new_dir(source_name);
  }

  if (file_exists(filename_in) || file_exists(filename_src)) {
    return;
  }

  FILE *inc = fopen(filename_in, "w");
  if (inc == NULL) {
    return;
  }
  char *upper_file = get_upper_filename(file);
  write_copyright(inc, file, true);
  fprintf(inc, "\n// File created by the new_file tool !\n\n");
  fprintf(inc, "#ifndef %s_H\n", upper_file);
  fprintf(inc, "#define %s_H\n\n", upper_file);
  fprintf(inc, "#endif // %s_H\n", upper_file);
  fclose(inc);

  FILE *src = fopen(filename_src, "w");
  if (src == NULL) {
    free(upper_file);
    return;
  }

  write_copyright(src, file, false);
  fprintf(src, "\n\n#include \"../include/%s.h\"\n\n", file);
  fprintf(src, "// File created by the new_file tool !\n");

  fclose(src);
  free(upper_file);
}
#endif
#endif // NEW_FILE_LIB_H