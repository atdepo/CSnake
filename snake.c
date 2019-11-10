#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <stropts.h>
#include <termios.h>
#include <fcntl.h>

#define ROWS 23
#define COLUMNS 80
#define B  L"\u2501"
#define S  L"\u2800"
#define UL L"\u250F"
#define UR L"\u2513"
#define BL L"\u2517"
#define BR L"\u251B"
#define BP L"\u2503"
#define PR L"\u227B"
#define PL L"\u227A"
#define PU L"\u2227"
#define PD L"\u2228"
#define PH L"\u0040"
//#define FOOD L"\u25AA"
#define FOOD L"\u2605"
#define T 100000
#define game_t int*

struct termios saved_attributes;

void reset_input_mode ();
void set_input_mode();
void printArena(game_t arena[][COLUMNS]);
void spawnPlayer(game_t arena[][COLUMNS]);
int isPlayer(game_t arena[][COLUMNS]);
void move_up(game_t arena[][COLUMNS]);
void move_down(game_t arena[][COLUMNS]);
void move_left(game_t arena[][COLUMNS]);
void move_right(game_t arena[][COLUMNS]);
void helloScreen();
void spawnFood(game_t arena[][COLUMNS]);
void removeTail(game_t arena[][COLUMNS]);

int x,y; //Position of the head of snake
int tailx,taily; //position to the tail of the snake
int flag=0; // 0=no food spawned, 1=food already spawned
int size=1;

int main(){

setlocale(LC_ALL, "");
//setlocale(LC_CTYPE,"C-UTF-8");
//helloScreen();
fcntl(0, F_GETFL, 0);
fcntl(0, F_SETFL,O_NONBLOCK);

game_t ciccio[ROWS][COLUMNS]={
{UL,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,UR},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BP,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,BP},
{BL,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,BR}
};

spawnPlayer(ciccio);
printArena(ciccio);
char mv,c;
int n;
set_input_mode();

while(1){
spawnFood(ciccio);

  read(0, &c, 1);
  if(c=='w'||c=='a'||c=='s'||c=='d')
    mv=c;


system("clear");

  switch (mv) {
    case 'w':
    move_up(ciccio);

    break;
    case 'a':
    move_left(ciccio);
    break;

    case 'd':
    move_right(ciccio);
    break;

    case 's':
    move_down(ciccio);
    break;

  }
printArena(ciccio);
//printf("move%c\n",mv);
  usleep(T);
}
reset_input_mode();
}

void printArena(game_t arena[][COLUMNS]){
  for(int i=0;i<ROWS;i++){
    for(int j=0;j<COLUMNS;j++)
    printf("%ls", arena[i][j]);
  printf("\n");
  }
//printf("tail:%d-%d/head:%d-%d",tailx,taily,x,y);
  }

void spawnPlayer(game_t arena[][COLUMNS]){
x=10,y=40;
tailx=x,taily=y;
arena[x][y]=PH;
}

int isPlayer(game_t arena[][COLUMNS]){

if(arena[x][y]==PL||arena[x][y]==PR ||arena[x][y]==PU||arena[x][y]==PD||arena[x][y]==PH)
return 1;
else
return 0;

}

void move_up(game_t arena[][COLUMNS]){
x--;
int tmp=x;

  if(isPlayer(arena)||arena[x][y]==B ||arena[x][y]==BP) //if player hits himself or the wall
    exit(0); //exit from the game

  if(arena[x][y]==FOOD){//if the player eats food

      flag=0;           // set flag=0 to respawn a new food
      arena[x][y]=PH;
      arena[x+1][y]=PU;    //advance head and not the tail
    }
  else{
    arena[x][y]=PH;
    arena[x+1][y]=PU;
    removeTail(arena);
  }

}

void move_down(game_t arena[][COLUMNS]){
x++;
int tmp=x;

  if(isPlayer(arena)||arena[x][y]==B ||arena[x][y]==BP)//if player hits himself
    exit(0);

    if(arena[x][y]==FOOD){
        flag=0;
        arena[x][y]=PH;
        arena[x-1][y]=PD;
    }
    else{
      arena[x][y]=PH;
      arena[x-1][y]=PD;
      removeTail(arena);
    }
}

void move_left(game_t arena[][COLUMNS]){
y--;
  if(isPlayer(arena)||arena[x][y]==B ||arena[x][y]==BP)//if player hits himself
    exit(0);
    if(arena[x][y]==FOOD){

        flag=0;
        arena[x][y]=PH;
        arena[x][y+1]=PL;
    }
    else{
      arena[x][y]=PH;
      arena[x][y+1]=PL;
      removeTail(arena);
    }
}
void move_right(game_t arena[][COLUMNS]){
y++;
  if(isPlayer(arena)||arena[x][y]==B ||arena[x][y]==BP)//if player hits himself
    exit(0);
    if(arena[x][y]==FOOD){

      flag=0;
      arena[x][y]=PH;
        arena[x][y-1]=PR;
    }
    else{

      arena[x][y]=PH;
      arena[x][y-1]=PR;
      removeTail(arena);
      }

}



void set_input_mode (){
struct termios tattr;
char *name;

/* Make sure stdin is a terminal. */
if (!isatty (STDIN_FILENO))
  {
    fprintf (stderr, "Not a terminal.\n");
    exit (EXIT_FAILURE);
  }

/* Save the terminal attributes so we can restore them later. */
tcgetattr (STDIN_FILENO, &saved_attributes);
atexit (reset_input_mode);

/* Set the funny terminal modes. */
tcgetattr (STDIN_FILENO, &tattr);
tattr.c_lflag &= ~(ICANON|ECHO|O_NONBLOCK); /* Clear ICANON and ECHO. */
tattr.c_cc[VMIN] = 1;
tattr.c_cc[VTIME] = 0;
tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

void reset_input_mode () {
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void helloScreen(){

initscr();
noecho();
curs_set(FALSE);

mvprintw(5,18,"Hello and welcome to the Snake (C-Version)!");
mvprintw(10,20,"Use the W-A-S-D-keys to move the player\n");
refresh();
sleep(3);
mvprintw(15,25,"Press any key to continue");
refresh();
if(getchar());
endwin();
}

void spawnFood(game_t arena[][COLUMNS]){

if(flag==0){
  srand(time(0));
  int nx=rand()%ROWS+1;
  int ny=rand()%COLUMNS+1;
  arena[nx][ny]=FOOD;
  flag=1;
}
}

void removeTail(game_t arena[][COLUMNS]){

game_t t=arena[tailx][taily];
//printf("tail:--%ls--\n",arena[tailx][taily]);
  if(arena[tailx][taily]==PU){
    //printf("tail=up");
    arena[tailx][taily]=S;
    tailx-=1;
  }
    else
      if(arena[tailx][taily]==PD){
        //printf("tail=down");
        arena[tailx][taily]=S;
        tailx+=1;
      }
      else
        if(arena[tailx][taily]==PL){
        //  printf("tail=left");
          arena[tailx][taily]=S;
          taily-=1;
        }
        else
          if(arena[tailx][taily]==PR){
            //printf("tail=right");
            arena[tailx][taily]=S;
            taily+=1;
          }
}
