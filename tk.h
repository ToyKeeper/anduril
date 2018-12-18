#ifndef TK_H
#define TK_H

/////
// tk.h
// misc tricks which need to be available before other includes,
// but which don't need to be repeated in every source file
////

// create a way to include files defined at the command line,
// like with "gcc -DCONFIGFILE=foo.h"
#define incfile2(s) #s
#define incfile(s) incfile2(s)
// use it like this:
//#include incfile(CONFIGFILE)

#endif
