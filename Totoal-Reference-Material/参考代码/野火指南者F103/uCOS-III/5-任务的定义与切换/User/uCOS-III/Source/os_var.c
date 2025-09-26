#define   OS_GLOBALS
/*
* os.h中定义了很多的全局变量，但是os.h又需要被很多的c文件调用，如果不特殊处理的话，会出现全局变量多次定义的错误，
* 为了实现在os.h中定义全局变量，在os.h的开头加入以下宏定义，且在全局变量前面都加上“OS_EXT”
* #ifdef     OS_GLOBALS
* #define    OS_EXT
* #else
* #define    OS_EXT  extern
* #endif
* 然后单独定义一个os_var.c文件，然后包含os.h，且只在这个文件里面定义宏“OS_GLOBALS”，其他包含os.h的文件中
* 都不定义宏“OS_GLOBALS”，那么在编译的时候，只有os_var.c里面的os.h里面的全局变量前面的OS_EXT才展开为空，其他
* 文件中包含的os.h中的全局变量前面的OS_EXT就展开为extern，即声明，而不是定义，这样就实现了在os.h中定义全局变量。
*/


#include "os.h"


