#ifndef THREADSTRUCTH
#define THREADSTRUCTH

#include <JobQueue.h>

/////////////////////////////////////////////////////////////
// thread info structure
// use the GET_SAFE / SET_SAFE functions with anything shared between threads!
typedef struct ThreadInfo_tag
{
   CString	csFile;           // file to process
   OPT_STRUCT opts;
   BOOL     bDone;            // set to TRUE when done 
	BOOL     bStop;            // stop if this becomes TRUE
   int      iPercentDone;     // set by thread as work progresses
   int      iErr;             // set by thread
} ThreadInfo;

#endif
