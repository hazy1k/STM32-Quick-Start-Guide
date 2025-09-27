#include "includes.h"
extern void Fatfs_MainTask(void);
void UserAPP(void)
{
	while(1)
	{
		Fatfs_MainTask();
	}
}
