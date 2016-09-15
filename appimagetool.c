#include <stdio.h>
#include <argp.h>

#include <stdlib.h>
#include <fcntl.h>
#include "squashfuse.h"

#include <sys/types.h>
#include <sys/stat.h>

#include "binreloc.h"
#ifndef NULL
    #define NULL ((void *) 0)
#endif

#include <libgen.h>

const char *argp_program_version =
  "appimagetool 0.1";
  
const char *argp_program_bug_address =
  "<probono@puredarwin.org>";
  
static char doc[] =
  "appimagetool -- Generate, extract, and inspect AppImages";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  char *args[2];            /* SOURCE and DESTINATION */
  int verbose;              /* The -v flag */
  int list;                 /* The -l flag */
  char *dumpfile;            /* Argument for -d */
};


static struct argp_option options[] =
{
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {"list",   'l', 0, 0,
   "List files in SOURCE AppImage"},
  {"dump", 'd', "FILE", 0,
   "Dump FILE from SOURCE AppImage to stdout"},
  {0}
};


static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'v':
      arguments->verbose = 1;
      break;
    case 'l':
      arguments->list = 1;
      break;
    case 'd':
      arguments->dumpfile = arg;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 3)
	{
	  argp_usage(state);
	}
      arguments->args[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1)
	{
	  argp_usage (state);
	}
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}


static char args_doc[] = "SOURCE {DESTINATION}";


static struct argp argp = {options, parse_opt, args_doc, doc};


// #####################################################################


static void die(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int is_directory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

/* Function that prints the contents of a squashfs file
 * using libsquashfuse (#include "squashfuse.h")
 */
int sfs_ls(char* image) {
	sqfs_err err = SQFS_OK;
	sqfs_traverse trv;
	sqfs fs;

	if ((err = sqfs_open_image(&fs, image, 0)))
		die("sqfs_open_image error");
	
	if ((err = sqfs_traverse_open(&trv, &fs, sqfs_inode_root(&fs))))
		die("sqfs_traverse_open error");
	while (sqfs_traverse_next(&trv, &err)) {
		if (!trv.dir_end) {
			printf("%s\n", trv.path);
		}
	}
	if (err)
		die("sqfs_traverse_next error");
	sqfs_traverse_close(&trv);
	
	sqfs_fd_close(fs.fd);
	return 0;
}

// #####################################################################

int main (int argc, char **argv)
{
  struct arguments arguments;

  /* Set argument defaults */
  arguments.list = 0;
  arguments.verbose = 0;
  arguments.dumpfile = NULL;
  
  /* Where the magic happens */
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  /* If in verbose mode */
  if (arguments.verbose)
    fprintf (stdout, "Verbose mode, to be implemented");
  
  /* If in list mode */
  if (arguments.list){
    sfs_ls(arguments.args[0]);
    exit(0);
  }
    
  /* If in dumpfile mode */
  if (arguments.dumpfile){
    fprintf (stdout, "%s from the AppImage %s should be dumped to stdout\n", arguments.dumpfile, arguments.args[0]);
    die("To be implemented");
  }
  
  /* Print argument values */
  fprintf (stdout, "SOURCE = %s\nDESTINATION = %s\n\n",
	   arguments.args[0],
	   arguments.args[1]);

  /* If the first argument is a directory, then we assume that we should package it */
  if(is_directory(arguments.args[0])){
      char *destination;
      if (arguments.args[1]) {
          destination = arguments.args[1];
      } else {
          char resolved_path[PATH_MAX];
          realpath(arguments.args[0], resolved_path);
          destination = basename(br_strcat (resolved_path, ".AppImage"));
          fprintf (stdout, "DESTINATION not specified, so assuming %s\n", destination);
      }

      die("To be implemented");
      fprintf (stderr, "Marking the AppImage as executable...\n");
      if (chmod (destination, 0755) < 0) {
          printf("Could not set executable bit, aborting\n");
          exit(1);
      }
      fprintf (stderr, "Success\n");
}

  /* If the first argument is a regular file, then we assume that we should unpack it */
  if(is_regular_file(arguments.args[0])){
      fprintf (stdout, "%s is a file, assuming it is an AppImage and should be unpacked\n", arguments.args[0]);
      die("To be implemented");
  }

  return 0;
}
