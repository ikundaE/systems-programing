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

//Declare variables to be used in multiple functions
int windowheight;
int windowwidth;
int thisline;
int alllines;
double scrollSpeed;
int scrollswitch;


//The file to read from
FILE * file;

struct itimerval timer;

//Declare all the functions for the compiler to not get confused
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

//Quit the program

/*kill signal handler*/

int main(int argc, char ** argv)
{
    //Get the terminal height and width
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    windowheight = w.ws_row-1;
    windowwidth = w.ws_col;
    
    //Set up the terminal so that long lines truncate as opposed to wrapping
    fputs("\033[?7l", stdout);
    
    //Catch the all signals
    alarmhandler();
    
    //Set the default scroll speed to 2 seconds (2000ms) per line
    scrollSpeed = 2000.0;
    refreshrate();
    alllines = 400;
    
    //Handle the error that the file does not exist
    if(argc == 2)
    {
        printf("Opening File '%s'\n", argv[1]);
        //Open the file and handle the error that the file does not exist
        file = fopen(argv[1], "r");
        if(file != NULL)
        {
            commandreceiver(file);
        }
        else
        {
            perror("File does not exist!\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (argc == 1)
    {
        //if we just type the command, assume we're reading from stdin
        printf("Reading from standard file\n");
        //Open the file and handle the error that the file does not exist
        file = stdin;
        commandreceiver(file);
    }
    else
    {
        perror("Too many/too few arguments!\n");
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
    //First, clear the screen
    cleandisplay();
    //Main program logic
    while(1)
    {
        generatescreen();
        noecho();
        int c;
        c = getchar();
        if(c == 'q')
        {
            //Quit the program
            quit();
        }
        else if (c == ' ')
        {
            //Skip to the next screenful
            //windowheight for screenful
            showtexts(windowheight);
        }
        else if(c == '\n')
        {
            //toggle auto scrolling
            scrollswitch = !scrollswitch;
        }
        else if (c == 's')
        {
            //Slower by 20%
            scrollSpeed *= 1.25;
            refreshrate();
        }
        else if (c == 'f')
        {
            scrollSpeed *= .8;
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
    //Set the timer to the scrollTime
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
    /* Start a virtual timer. It counts down whenever this process is
     executing. */
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("error calling setitimer()");
        quit();
    }
}
void alarmhandler()
{
    
    /* Install timer_handler as the signal handler for SIGALRM. */
    if (signal(SIGALRM, (void (*)(int)) filedisplay) == SIG_ERR) {
        perror("Attempt to catch SIGALRM unsuccessful");
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
     
    while (fgets(line, sizeof line, file) != NULL) /* read a line */
    {
        if (count >= thisline && count < thisline+windowheight)
        {
            //print the line
            printf("%s", line);
            lines++;
        }
        count++;
    }
    //reset the file
    rewind(file);
    while(lines < windowheight)
    {
        printf("\n");
        lines++;
    }
    
    //And now, print out the program controls
    //Reverse video for pres formatting
    fputs("\033[7m", stdout);
    printf("Commands: SPACE - Page down, ENTER (\n) - Switch auto-scroll, f - Faster 20%%, s - Slower 20%%, q - Quit, Scroll speed = %fms, autoScroll = %d", scrollSpeed, scrollswitch);
    fputs("\033[0m", stdout);
    fflush(stdout);
}
void cleandisplay()
{
    //Clear the screen
    fputs("\033[2J",stdout);
    //Move the cursor to the top left
    fputs("\033[1;1H",stdout);
}
void noecho()
{
    struct termios info;
    tcgetattr(0,&info);    //get attribs
    info.c_lflag &= ~(ICANON | ECHO) ;    //turn off echo bit
    tcsetattr(0,TCSANOW,&info);   //set attribs
    
}
void echo()
{
    
    struct termios info;
    tcgetattr(0,&info);    //get attribs
    info.c_lflag |= (ICANON | ECHO) ;    //turn off echo bit
    tcsetattr(0,TCSANOW,&info);   //set attribs
}

void signalhandler(int signal)
{
    quit();
}

void quit()
{
    noecho();
    echo();
    cleandisplay();
    //close the file
    fclose(file);
    exit(0);
}
