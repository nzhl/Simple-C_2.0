//  file related

//  C header
#include <stdio.h>
#include <assert.h>

//  third-party


//  project related

#include "error.h"

//---------------------------------------------

static char *infile;
static char *outfile;


int main(int argc, char* argv[]) {
    // set no buffer, useful for debug.
    setbuf(stdout, NULL);

    // todo parse option ...

    error("123");
    warning("123");
}
