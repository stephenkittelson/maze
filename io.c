#include <stdio.h>
#include <allegro.h>

#include LIBDIR"std.h"
#include LIBDIR"port.h"

void get_string(B8 *str,B16 size)
{
	B16 c=0;

	while(1)
	{
		str[c]=readkey();
		switch(str[c])
		{
			case '\b':
			{
				str[c]=0;
				if(c==0)
					break;
				str[--c]=0;
				setcurpos(getcurx()-1,getcury());
				fputc(' ',stdout);
				fflush(stdout);
				setcurpos(getcurx()-1,getcury());
				break;
			}
			case '\r':
			case '\n':
			{
				str[c]=0;
				return;
			}
			default:
			{
				if(c==size)
					break;
				fputc(str[c],stdout);
				fflush(stdout);
				c++;
				break;
			}
		}
	}
}
