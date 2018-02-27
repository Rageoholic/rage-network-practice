#ifndef RDEF_H
#define RDEF_H

#include <stdint.h>
#include <stddef.h>

/* Types and constants */

typedef const int FD;



#define local static		/* Function is local to the file */

#define ignore (void)		/* Ignore this variable. Turns off
				   warnings */

typedef int ReadFlags;		/* Set of flags for reading files. See
				   DEFINEs with ReadFlags */

#define READ_PEEK 0x01		/* ReadFlags: Don't actually advance
				   the buffer */

#define READ_NO_BLOCK 0x02 	/* ReadFlags: Don't block on reading
				   data from the descriptor */

#endif
