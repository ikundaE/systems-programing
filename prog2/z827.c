/*
 * COS 350 this program will do a compression and decompression
 * Enoch Ikunda
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
unsigned int flSize;

int main (int argc, char *argv[]){
  //first check if you're getting more than one argument
        if (argc < 2)
          {
            printf("Please add a file");
            exit(1);
          }

        //get the file's size
        char *flName;
        flName = argv [1];
        //strcpy(flName, argv[1]);
        //char name [strlen(flName)];
        int fl; //a pointer to the input file descriptor
        printf("Name: %s\n", flName);
        fl = open(flName, O_RDONLY);
        int flWt;
        char outNm [64] ;
        strcpy(outNm, flName);

        ///////FILE SIZE
        //move the pointer to the end of the file
        flSize = lseek(fl,0,SEEK_END);//ftell returns a long so I had to cast it
        printf ("The size of the file is: %d\n", flSize);
        lseek(fl,0,SEEK_SET);
        //Now check for the right extension for compression or decompression

        const char ch = '.';
        char *ext;
        ext = strrchr (argv[1], ch);
        printf("The extension after <%c> is - <%s>\n", ch, ext);

        /*If the file to be compressed has any characters
        that have an eighth bit set, you can simply report
        that the file is not compressible and exit*/
        if (flSize <= 8 && strcmp(ext,".txt") == 0 )
          {
            printf("file too small");
            exit(1);
          }
        int i = 0;
        int cmprsd = 0;
        char entireTxt [flSize];
        int temp = 0;
        int buf = 0;
        int lftShft = 0;
        int numbytes = read(fl,entireTxt,flSize);

        if ((strcmp(ext,".txt") != 0) && (strcmp(ext,".z827") != 0) ){
                printf ("Sorry unknown file.\n");
                exit (1);
                }


        //**********Below is the compression codes*************
        if (strcmp(ext,".txt") == 0){
                strcat(outNm,".z827");
                //flWt = creat (outNm, O_RDWR);
                flWt = open(outNm, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                //first write the size of the file in the compressed file
                write(flWt, &flSize,4);
                //Check for the size of the before exitin the loop
                printf ("The size of the entiretxt array is: %d\n", numbytes);
                while (i < flSize){
                        if (lftShft > 0) {
                                temp = entireTxt [i];
                                temp = temp << lftShft;
                                buf |= temp;
                                write(flWt,&buf,1);
                                buf = buf >> 8;
                                lftShft = lftShft - 1;
                                i++;
                        }else {
                                buf = entireTxt [i];
                                lftShft = 7;
                                i++;
                        }
                }
                if (buf > 0){
                        write (flWt,&buf,1);
                        buf = buf >> 8;
                }

                printf("Compression complete.\n");
                close (flWt);
                close (fl);
                unlink(flName);
                printf("File : %s, has been deleted\n",flName);
                printf("File : %s, has been created\n",outNm);

        //**********Below is the Decompression codes*************
        }else if (strcmp(ext,".z827") == 0){
                printf ("Decompression initializing ... \n");
                buf = 0;//re-initialize the buffer
                int rghtShft= 7;
                int bitcnt = 0;
                int i ;
                int strngLth = strlen(flName);
                flName[strngLth - 5] = 0;
                flWt = open(flName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                char temp = 0;
                lseek(fl,0,SEEK_SET);
                int size = 0;
                read(fl, &size,4);
                char mask = 0x7F;
                int bttmBits = 0;
                printf ("The size of the decompressed file should be: %d\n", size);
                read (fl, &temp,1);
                buf = buf |= temp;
                bttmBits = buf & mask;
                write (flWt,&bttmBits,1);
                buf = buf >> rghtShft;
                bitcnt = 1;
                for( i =0 ; i < fl; i++)
                {
                  if(bitcnt  < 7)
                        {
                                read (fl, &temp,1);
                                buf = buf |= temp;
                                bttmBits = buf & mask;
                                write (flWt,&bttmBits,1);
                                buf = buf >> rghtShft;
                                bitcnt ++;
                                //rghtShft--;
                        }else
                        {
                                read (fl, &temp,1);
                                buf = buf |= temp;
                                bttmBits = buf & mask;
                                write (flWt,&bttmBits,2);                               //rghtShft = 7;
                                buf = buf >> rghtShft;
                                bitcnt = 1;
                                //rghtShft--;
                        }
                }
        }else {
                printf ("Sorry unknown file.\n");
                exit (1);
        }

return 0;
}
