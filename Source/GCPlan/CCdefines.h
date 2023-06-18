/* 
	Define statements to switch between MAC and Windows compilers
	
	to compile for Windows use the following 2 lines
//#undef WINDOWSNOTMAC // MAC not Windows
#define WINDOWSNOTMAC // Windows not MAC

	to compile for MAC use the following 2 lines
#undef WINDOWSNOTMAC // Windows not MAC
//#define WINDOWSNOTMAC // Windows not MAC

*/
#ifndef CCDEFINES_H
#define CCDEFINES_H

#define WINDOWSNOTMAC // Windows not MAC
#undef WINDOWSNOTMAC // MAC not Windows

#ifdef WINDOWSNOTMAC // Windows not MAC
#undef USELIBRARY // Windows
#else // WINDOWSNOTMAC // Windows not MAC
#define USELIBRARY // MAC
#endif // WINDOWSNOTMAC // Windows not MAC

#endif /*CCDEFINES_H inclusion guard*/


