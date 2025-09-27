#include "includes.h"
extern void Touch_MainTask(void);
void UserAPP(void)
{
	while(1)
	{
		Touch_MainTask();
	}
}
