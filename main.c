#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <conio.h>
#include <allegro.h>

#include LIBDIR"std.h"
#include LIBDIR"djgpp.h"

#include "io.h"

#define OPEN_FAILED 1

#define CALC_OFFSET(_Mx,_My) ((80*_My)+_Mx)

struct PLAYER
{
	B8 x,y;
	B16 offset;
};

void show_visible_walls(B8,B8);
void clean_up(void);

void set_text_color(B8 x,B8 y,B16 length,B8 color)
{
	B16 offset,c=0;

	offset=(CALC_OFFSET(x,y)*2)+1;
	for(;c<length && offset<8000;c++,offset+=2)
	{
		movedata(_my_ds(),(int)&color,_dos_ds,0xB8000+offset,1);
	}
}

void write_ch(B16 offset,B8 ch)
{
	movedata(_my_ds(),(int)&ch,_dos_ds,0xB8000+(offset*2),1);
	//
	//
}

void clear_scr(void)
{
	B16 empty=0x0720;
	B16 c;

	for(c=0;c<8000;c+=2)
		movedata(_my_ds(),(int)&empty,_dos_ds,0xB8000+c,2);
}

static B8 vscr[4000];

int main(void)
{
	B8 file_num[4]={0};
	B8 file_name[13]={0};
	B16 offset;//,c;
	B8 ch;
	struct PLAYER p;
	B8 end_x,end_y;
	FILE *maze_file;

	allegro_init();
	install_keyboard();
	#ifdef DEBUG
		init_log();
	#endif

	printf("Enter a number: ");
	get_string(file_num,4);
	sprintf(file_name,"maze%s.dat",file_num);
	//sprintf(file_name,"maze1.dat");
	if(!(maze_file=fopen(file_name,"r")))
	{
		printf("\nCannot open '%s'!\n",file_name);
		return OPEN_FAILED;
	}

	_set_screen_lines(50);
	set_text_color(0,0,4000,0x07);
	hidecur();
	atexit(clean_up);

	p.x=fgetc(maze_file);
	p.y=fgetc(maze_file);
	end_x=fgetc(maze_file);
	end_y=fgetc(maze_file);
	p.offset=CALC_OFFSET(p.x,p.y);
	for(offset=0;offset<4000;offset++)
	{
		ch=fgetc(maze_file);
		if(ch=='\n' || ch=='\r')
			ch=fgetc(maze_file);
		vscr[offset]=ch;
	}
	fclose(maze_file);

	clear_scr();
	clear_keybuf();
	show_visible_walls(p.x,p.y);
	write_ch(p.offset,'');
	while(1)
	{
		if(p.x==end_x && p.y==end_y)
		{
			clean_up();
			setcurpos(0,0);
			printf("You won!\n");
			exit(0);
		}
		if(key[KEY_UP])
		{
			if(vscr[p.offset-80]!=' ')
				continue;
			p.y--;
			p.offset-=80;
			key[KEY_UP]=FALSE;
		}
		else if(key[KEY_DOWN])
		{
			if(vscr[p.offset+80]!=' ')
				continue;
			p.y++;
			p.offset+=80;
			key[KEY_DOWN]=FALSE;
		}
		else if(key[KEY_LEFT])
		{
			if(vscr[p.offset-1]!=' ')
				continue;
			p.x--;
			p.offset--;
			key[KEY_LEFT]=FALSE;
		}
		else if(key[KEY_RIGHT])
		{
			if(vscr[p.offset+1]!=' ')
				continue;
			p.x++;
			p.offset++;
			key[KEY_RIGHT]=FALSE;
		}
		else if(key[KEY_ESC])
		{
			exit(0);
		}
		else
		{
			continue;
		}
		clear_scr();
		show_visible_walls(p.x,p.y);
		write_ch(p.offset,'');
	}
	return 0;
}

void show_visible_walls(B8 px,B8 py)
{
	B8 sight_line[19][10][3]=
	{
		// sight line number 
			// (quadrant 1) x offset, y offset, sight line number to jump to if blocked
		// 1
		{
			{00,01, 2},//05},
			{00,02, 2},//04},
			{00,03, 2},//03},
			{00,04, 2},//03},
			{00,05, 2},//03},
			{00,06, 2},//02},
			{00,07, 2},//02},
			{00, 8, 2},//02},
			{00, 9, 2},//02},
			{00,10, 2}//02}
		},
		// 2
		{
			{00,01, 3},//05},
			{00,02, 3},//04},
			{00,03, 3},//03},
			{00,04, 3},//03},
			{00,05, 3},//03},
			{01,06, 3},//04},
			{01,07, 3},//04},
			{01, 8, 3},//03},
			{01, 9, 3},//03},
			{01,10, 3}//03}
		},
		// 3
		{
			{00,01, 4},//05},
			{00,02, 4},//04},
			{01,03, 4},//06},
			{01,04, 4},//05},
			{01,05, 4},//04},
			{01,06, 4},//04},
			{01,07, 4},//04},
			{02, 8, 4},//05},
			{02, 9, 4},//04},
			{02,10, 4}//04}
		},
		// 4
		{
			{00,01, 5},//05},
			{01,02, 5},// 8},
			{01,03, 5},//06},
			{01,04, 5},//05},
			{02,05, 5},//06},
			{02,06, 5},//05},
			{02,07, 5},//05},
			{02, 8, 5},//05},
			{03, 9, 5},//05},
			{03,10, 5}//05}
		},
		// 5
		{
			{01,01, 6},//14},
			{01,02, 6},// 8},
			{01,03, 6},//06},
			{02,04, 6},//07},
			{02,05, 6},//06},
			{03,06, 6},//07},
			{03,07, 6},//06},
			{03, 8, 6},//06},
			{04, 9, 6},//06},
			{99,99, 6}//99}
		},
		// 6
		{
			{01,01, 7},//14},
			{01,02, 7},// 8},
			{02,03, 7},// 9},
			{02,04, 7},//07},
			{03,05, 7},// 8},
			{03,06, 7},//07},
			{04,07, 7},// 8},
			{04, 8, 7},//07},
			{05, 9, 7},//07},
			{99,99, 7}//99}
		},
		// 7
		{
			{01,01, 8},//14},
			{01,02, 8},// 8},
			{02,03, 8},// 9},
			{03,04, 8},//10},
			{03,05, 8},// 8},
			{04,06, 8},// 9},
			{04,07, 8},// 8},
			{05, 8, 8},// 8},
			{99,99, 8},//99},
			{99,99, 8}//99}
		},
		// 8
		{
			{01,01, 9},//14},
			{02,02, 9},//12},
			{02,03, 9},// 9},
			{03,04, 9},//10},
			{04,05, 9},//10},
			{04,06, 9},// 9},
			{05,07, 9},// 9},
			{06, 8, 9},// 9},
			{99,99, 9},//99},
			{99,99, 9}//99}
		},
		// 9
		{
			{01,01,10},//14},
			{02,02,10},//12},
			{03,03,10},//11},
			{03,04,10},//10},
			{04,05,10},//10},
			{05,06,10},//10},
			{06,07,10},//10},
			{07, 8,10},//10},
			{99,99,10},//99},
			{99,99,10}//99}
		},
		// 10
		{
			{01,01,11},//14},
			{02,02,11},//12},
			{03,03,11},//11},
			{04,04,11},//11},
			{05,05,11},//11},
			{06,06,11},//11},
			{07,07,11},//11},
			{99,99,11},//99},
			{99,99,11},//99},
			{99,99,11}//99}
		},
		// 11
		{
			{01,01,12},//14},
			{02,02,12},//12},
			{03,02,12},//14},
			{04,03,12},//13},
			{05,04,12},//12},
			{06,05,12},//12},
			{07,06,12},//12},
			{ 8,07,12},//12},
			{99,99,12},//99},
			{99,99,12}//99}
		},
		// 12
		{
			{01,01,13},//14},
			{02,01,13},//17},
			{03,02,13},//14},
			{04,03,13},//13},
			{05,03,13},//14},
			{06,04,13},//14},
			{07,05,13},//13},
			{ 8,06,13},//13},
			{99,99,13},//99},
			{99,99,13}//99}
		},
		// 13
		{
			{01,01,14},//14},
			{02,01,14},//17},
			{03,02,14},//14},
			{04,02,14},//16},
			{05,03,14},//14},
			{06,04,14},//14},
			{07,04,14},//15},
			{ 8,05,14},//14},
			{99,99,14},//99},
			{99,99,14}//99}
		},
		// 14
		{
			{01,00,15},//99},
			{02,01,15},//17},
			{03,01,15},//18},
			{04,02,15},//16},
			{05,02,15},//17},
			{06,03,15},//15},
			{07,04,15},//15},
			{ 8,04,15},//15},
			{ 9,05,15},//15},
			{99,99,15}//99}
		},
		// 15
		{
			{01,00,16},//99},
			{02,01,16},//17},
			{03,01,16},//18},
			{04,02,16},//16},
			{05,02,16},//17},
			{06,02,16},//18},
			{07,03,16},//16},
			{ 8,03,16},//17},
			{ 9,04,16},//16},
			{99,99,16}//99}
		},
		// 16
		{
			{01,00,17},//99},
			{02,01,17},//17},
			{03,01,17},//18},
			{04,01,17},//19},
			{05,02,17},//17},
			{06,02,17},//18},
			{07,02,17},//18},
			{ 8,03,17},//17},
			{ 9,03,17},//17},
			{99,99,17}//99}
		},
		// 17
		{
			{01,00,18},//99},
			{02,00,18},//99},
			{03,01,18},//18},
			{04,01,18},//19},
			{05,01,18},//19},
			{06,02,18},//18},
			{07,02,18},//18},
			{ 8,02,18},//18},
			{ 9,02,18},//19},
			{10,03,18}//18}
		},
		// 18
		{
			{01,00,19},//99},
			{02,00,19},//99},
			{03,00,19},//99},
			{04,01,19},//19},
			{05,01,19},//19},
			{06,01,19},//99},
			{07,01,19},//99},
			{ 8,01,19},//99},
			{ 9,02,19},//19},
			{10,02,19}//19}
		},
		// 19 (last line)
		{
			{01,00,99},
			{02,00,99},
			{03,00,99},
			{04,00,99},
			{05,00,99},
			{06,01,99},
			{07,01,99},
			{ 8,01,99},
			{ 9,01,99},
			{10,01,99}
		}
	};
	B8 quadrant;
	B8 sl_num;
	B8 sl_num_index;
	SB8 x,y;
	B16 offset;

	for(quadrant=1;quadrant<=4;quadrant++)
	{
		for(sl_num=0;sl_num<19;sl_num++)
		{
			for(sl_num_index=0;sl_num_index<10;sl_num_index++)
			{
				// have we have already processed them all for this line?
				if(sight_line[sl_num][sl_num_index][0]==99)
					break;
				switch(quadrant)
				{
					case 1:
					{
						x=sight_line[sl_num][sl_num_index][0];
						y=sight_line[sl_num][sl_num_index][1];
						break;
					}
					case 2:
					{
						x=-1*sight_line[sl_num][sl_num_index][1];
						y=sight_line[sl_num][sl_num_index][0];
						break;
					}
					case 3:
					{
						x=-1*sight_line[sl_num][sl_num_index][0];
						y=-1*sight_line[sl_num][sl_num_index][1];
						break;
					}
					case 4:
					{
						x=sight_line[sl_num][sl_num_index][1];
						y=-1*sight_line[sl_num][sl_num_index][0];
						break;
					}
					default:
					{
						printf("show_visible_walls: quadrant switch default hit!\n");
						exit(1);
					}
				}
				x+=px;
				y+=py;
				if(x<0 || x>=80)
					break;
				if(y<0 || y>=50)
					break;
				offset=(80*y)+x;
				#ifdef DEBUG
					set_text_color(x,y,1,0x17);
				#endif
				// is there a wall?
				if(vscr[offset]!=' ')
				{
					// yes!
					// do we need to draw the wall differently?
					// defines that help me think faster :-)
						#define ISABOVE y>py
						#define ISDABOVE (x==px && y>py)
						#define ISBELOW y<py
						#define ISDBELOW (x==px && y<py)
						#define ISLEFT x>px
						#define ISDLEFT (y==py && x>px)
						#define ISRIGHT x<px
						#define ISDRIGHT (y==py && x<px)
					switch(vscr[offset])
					{
						case ((B8)'¿'):
						case ((B8)'Ù'):
						case ((B8)'À'):
						case ((B8)'Ú'):
						case ((B8)'³'):
						case ((B8)'Ä'):
						{
							write_ch(offset,vscr[offset]);
							break;
						}
						case ((B8)'Á'):
						{
							if(ISABOVE)
							{
								if(ISRIGHT)
									write_ch(offset,'À');
								else if(ISLEFT)
									write_ch(offset,'Ù');
								else //directly above
									write_ch(offset,'Á');
							}
							else //below
							{
								write_ch(offset,'Ä');
							}
							break;
						}
						case ((B8)'Â'):
						{
							if(ISABOVE)
							{
								write_ch(offset,'Ä');
							}
							else //below
							{
								if(ISRIGHT)
									write_ch(offset,'Ú');
								else if(ISLEFT)
									write_ch(offset,'¿');
								else //directly below
									write_ch(offset,'Â');
							}
							break;
						}
						case ((B8)'Ã'):
						{
							if(ISLEFT)
							{
								write_ch(offset,'³');
							}
							else //right
							{
								if(ISABOVE)
									write_ch(offset,'À');
								else if(ISBELOW)
									write_ch(offset,'Ú');
								else //directly below
									write_ch(offset,'Ã');
							}
							break;
						}
						case ((B8)'´'):
						{
							if(ISRIGHT)
							{
								write_ch(offset,'³');
							}
							else //left
							{
								if(ISABOVE)
									write_ch(offset,'Ù');
								else if(ISBELOW)
									write_ch(offset,'¿');
								else //directly left
									write_ch(offset,'´');
							}
							break;
						}
						case ((B8)'Å'):
						{
							if(ISBELOW && ISRIGHT)
							{
								write_ch(offset,'Ú');
							}
							else if(ISABOVE && ISRIGHT)
							{
								write_ch(offset,'À');
							}
							else if(ISBELOW && ISLEFT)
							{
								write_ch(offset,'¿');
							}
							else if(ISABOVE && ISLEFT)
							{
								write_ch(offset,'Ù');
							}
							else if(ISDABOVE)
							{
								write_ch(offset,'Á');
							}
							else if(ISDBELOW)
							{
								write_ch(offset,'Â');
							}
							else if(ISDLEFT)
							{
								write_ch(offset,'´');
							}
							else if(ISDRIGHT)
							{
								write_ch(offset,'Ã');
							}
							else
							{
								write_ch(offset,'Å');
							}
							break;
						}
						default:
						{
							// hmmm, not a wall but it was blockage...
							write_ch(offset,vscr[offset]);
							break;
						}
					}
					#ifdef DEBUG
						set_text_color(x,y,1,0x27);
					#endif
					sl_num=sight_line[sl_num][sl_num_index][2]-1;
					break;
				}
			}
		}
	}
}

void clean_up(void)
{
	static B8 cleaned=FALSE;

	if(!cleaned)
	{
		cleaned=TRUE;
		clear_scr();
		normcur();
		normvideo();
	}
}
