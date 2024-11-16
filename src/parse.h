#include <assert.h>
#include <cstring>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////
// If return value is >0, CACTI failed on this cache configuration.
// Always check the return value!
///////////////////////////////////////////////////////////////////////////
int get_cacti_results(unsigned int SIZE, unsigned int BLOCKSIZE, unsigned int ASSOC, float *AccessTime, float *Energy, float *Area) 
{
	char command[128];
	FILE *pipe;

    char buffer[128];
	char *substring;
	float Height, Width;

	int errflag = 3;

	/////////////////////////////////////////////////////////
	// 1. Generate the cacti command.
	/////////////////////////////////////////////////////////
	if (ASSOC == (SIZE/BLOCKSIZE))
	   sprintf(command, "./cacti %d %d FA 45nm 1 2>&1", SIZE, BLOCKSIZE);	// fully-associative case
	else
	   sprintf(command, "./cacti %d %d %d 45nm 1 2>&1", SIZE, BLOCKSIZE, ASSOC);

	/////////////////////////////////////////////////////////
	// 2. Execute cacti, and create a pipe between
	//    this process and the cacti process.
	/////////////////////////////////////////////////////////
	pipe = popen(command, "r");
	assert(pipe);

	/////////////////////////////////////////////////////////
	// 3. Extract the key results from cacti.
	//
	//    Format of key outputs from CACTI 6.0 (examples given):
	//
	//    Access time (ns): 0.24435
	//    Total dynamic read energy per access (nJ):0.0064104
	//    Cache height x width (mm): 0.402309 x 0.218135
	/////////////////////////////////////////////////////////

	while (fgets(buffer, 128, pipe)) {
	  if ((substring = strstr(buffer, "Access time"))) {
	     assert(substring = strstr(buffer, ":"));
	     sscanf(substring, ": %f", AccessTime);
	     //printf("%s", buffer);
	     errflag--;
	  }
	  else if ((substring = strstr(buffer, "Total dynamic read energy per access"))) {
	     assert(substring = strstr(buffer, ":"));
	     sscanf(substring, ":%f", Energy);
	     //printf("%s", buffer);
	     errflag--;
	  }
	  else if ((substring = strstr(buffer, "Cache height x width"))) {
	     assert(substring = strstr(buffer, ":"));
	     sscanf(substring, ": %f x %f", &Height, &Width);
	     *Area = Height*Width;
	     //printf("%s", buffer);
	     errflag--;
	  }
	}

	/////////////////////////////////////////////////////////
	// 4. Close the pipe.
	/////////////////////////////////////////////////////////
	pclose(pipe);

	return(errflag);
}
