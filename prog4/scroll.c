/*Prog4: Scroll.c
 *Author: Enoch Ikunda & Zachary Ouellette 
 *Class: COS 350
 *Prof: Bob Boothe
 */
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>

int windowheight;
int windowwidth;
int thisline;
int alllines;
double scrollSpeed;
int scrollswitch;// 1 for scrolling, 0 if not

FILE * file;

struct itimerval timer;

void filedisplay();
void commandreceiver();
void showtexts(int lines);
void refreshrate();
void generatescreen();
void showfile();
void cleandisplay();
void noecho();
void echo();
void quit();
void signalhandler(int signal);
void alarmhandler();

int main(int argc, char ** argv)
{
    
    struct winsize w; //window size 
    ioctl(0, TIOCGWINSZ, &w);
    windowheight = w.ws_row-1;//set window height
    windowwidth = w.ws_col;   //set window width
    
    fputs("\033[?7l", stdout);// long lines are cut short 
    
    
    alarmhandler();//for any signal 
    
    scrollSpeed = 2000.0; //2 seconed scroll speed
    refreshrate();
    alllines = 400;
    
    if(argc == 2)//checks to see if file exists or not
    {
        printf("'%s'\n", argv[1]);
        file = fopen(argv[1], "r");
        if(file != NULL)
        {
            commandreceiver(file);
        }
        else
        {
            perror("No file found\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (argc == 1)
    {
        file = stdin; //no file found error checking
        commandreceiver(file);
    }
    else
    {
        exit(EXIT_FAILURE);
    }
    
}

void filedisplay()
{
    if(scrollswitch)
    {
        echo();
        showtexts(1);
        generatescreen();
        fflush(stdout);
        noecho();
    }
}

void commandreceiver()
{
    cleandisplay();
    while(1)//infinite loop 
    {
        generatescreen();
        noecho();
        int c;
        c = getchar();
        if(c == 'q')
        {
            quit();//exit program
        }
        else if(c == '\n')
        {
            scrollswitch = !scrollswitch;
        }
        else if (c == ' ')
        {
            showtexts(windowheight);//go to the next screen 
        }
        else if (c == 'f')
        {
            scrollSpeed *= .8;//faster by 20%
            refreshrate();
        }
        else if (c == 's')
        {
            scrollSpeed *= 1.20;//Slower by 20%
            refreshrate();
        }
        echo();
    }
}
void showtexts (int numlines)
{
    thisline += numlines;
    if(thisline > alllines-windowheight)
    {
        if(alllines - windowheight < 0)
        {
            thisline = 0;
        }
        else
        {
            thisline = alllines-windowheight;
        }
    }
}
void refreshrate()
{
    if(scrollSpeed > 1000)
    {
        timer.it_value.tv_sec = scrollSpeed/1000;
        timer.it_value.tv_usec = 0;
    }
    else
    {
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = (int)scrollSpeed*1000;
    }
    
    timer.it_interval = timer.it_value;
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer() error");
        quit();
    }
}
void alarmhandler()
{
    if (signal(SIGALRM, (void (*)(int)) filedisplay) == SIG_ERR) {
        perror("Failed SIGALRM catch");
        quit();
    }
    signal(SIGINT, signalhandler);
}

void generatescreen()
{
    cleandisplay();
    showfile();
}
void showfile()
{
    char line[256];
    int count = 1;
    int lines = 0;
     
    while (fgets(line, sizeof line, file) != NULL)
    {
        if (count >= thisline && count < thisline+windowheight)
        {
            printf("%s", line);
            lines++;
        }
        count++;
    }
    rewind(file); //reset file
    while(lines < windowheight)
    {
        printf("\n");
        lines++;
    }
    
    //program controls
    fputs("\033[7m", stdout);
    printf("SpaceBar = Page down, (\n) enter = Switch auto-scroll, f = faster scroll 20%%, s = Slower scroll 20%%, q - quit, Scroll speed = %fms, autoScroll = %d", scrollSpeed, scrollswitch);
    fputs("\033[0m", stdout);
    fflush(stdout);
}
void cleandisplay()
{
    fputs("\033[2J",stdout);
    fputs("\033[1;1H",stdout);
}
void noecho()
{
    struct termios info;
    tcgetattr(0,&info); 
    info.c_lflag &= ~(ICANON | ECHO) ; 
    tcsetattr(0,TCSANOW,&info); 
    
}
void echo()
{
    
    struct termios info;
    tcgetattr(0,&info); 
    info.c_lflag |= (ICANON | ECHO) ;   
    tcsetattr(0,TCSANOW,&info);  
}

void signalhandler(int signal)
{
    quit();
}
//close file and quit the program without leaving it in a funny state
void quit()
{
    noecho();
    echo();
    cleandisplay();
    fclose(file);   
    exit(0);
}