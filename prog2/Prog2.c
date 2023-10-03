/* z827 1.0                                                                     
 * z827  accepts one file as a command line argument.  If that argument is a .t\
xt file                                                                         
 * it is compressed by one bit per char into a .txt.z827 file.                  
 * This orignal file will then be deleted upon successful compression.          
 * If that arguemnt is a .txt.z827 file then it will be decompressed back into \
a .txt file.                                                                    
 * The .txt.z827 file will then be deleted upon seccessful decompression.       
 *                                                                              
 * -Taylor Wilk 02.16.2020                                                      
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int sizeOfFile(char *fileName);
void compressFile(char *fileName);
void decompressFile(char *fileName);


int main(int argc, char *argv[])
{

  if(argc != 2)
  {
    fprintf(stderr, "Incorrect number of arguments: ");
  }

  const char *ext;  //ext is used to check the extension of a file              
  ext = strrchr(argv[1], '.'); //set ext to file extension                      

  if(strcmp(ext, ".txt") == 0)
  {
    compressFile(argv[1]);
  }                                         
  else if (strcmp(ext, ".z827") == 0)
  {
    decompressFile(argv[1]);
  }
  else
  {
    fprintf(stderr, "File type not compatable\n");
  }
}

void compressFile(char *fileName)
{
  int in_fd, out_fd, i, sizeRead;
  int writer_bits = 0;
  char origFile[strlen(fileName)];
  unsigned int bitbuf = 0x00000000;
  unsigned char compressedChar;
  unsigned int tempCh;

  //make sure file can be opened                                                
  if ((in_fd=open(fileName, O_RDONLY))== -1)
    {
      fprintf(stderr, "Cannot open %s\n", fileName);
    }

  char fileHeader = sizeOfFile(fileName);

  strcpy(origFile, fileName); //keep original file name so it can be deleted later                                                                             
  strcat(fileName, ".z827"); //append extension .z827 onto fileName to create a new file                                                                       
  out_fd = open(fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

  //put file header at begging of compressed file                               
  write(out_fd, &fileHeader, 4);
  
  //initialize a buffer                                                         
  int BUFFERSIZE = 4096;
  unsigned char buf[BUFFERSIZE];
  
  //read file into buffer until whole file is read                              
    while ((sizeRead = read(in_fd, &buf, BUFFERSIZE)) > 0)
    {
      //process the data from the buffer                                          
      for (i=0; i < sizeRead; i++)
      {
        tempCh = buf[i];
        tempCh = buf[i] << writer_bits;
        bitbuf = bitbuf | tempCh;
        writer_bits += 7; //keep track of number of bits                          

        //compressedChar is assigned that last 8 bits in the bitbuf               
        compressedChar = bitbuf & 0x000000ff;

        //if there are 8 or more bits in the bitbuf then write them to newfile    
        if(writer_bits >= 8)
        {
          write(out_fd, &compressedChar, 1);
          bitbuf = bitbuf >> 8;
          writer_bits -= 8;
        }
      }
      //if there any left over bits in the bitbuf at the end of reading           
      //the file then write them to newfile                                       
      if(writer_bits > 0)
      {
        write(out_fd, &compressedChar, 1);
      }
    }
    close(out_fd);
    close(in_fd);
    unlink(origFile);// This will be uncommented once decompression is working    
  }
  
  
void decompressFile(char *fileName)
{
  int in_fd, out_fd, i, fileSize, sizeRead;
  char origFile[strlen(fileName)];
  char header[4];
  int writer_bits = 0;
  unsigned int bitbuf = 0x00000000;
  unsigned int decompressedChar = 0x00000000
  unsigned int tempCh;

  //initialize a buffer                                                         
  int BUFFERSIZE = 4096;
  unsigned char buf[BUFFERSIZE];

  //make sure file is opened successfully                                       
  if ((in_fd=open(fileName, O_RDONLY))== -1)
    {
      fprintf(stderr, "Cannot open %s\n", fileName);
    }

  strcpy(origFile, fileName); //keep original filename in order to delete later
  //remove .z827 from fileName and create a new file to write to                
  int stringLength = strlen(fileName);
  fileName[stringLength - 5] = 0;

  out_fd = open(fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);


  //find out the size the decompressed file should ber                          
  read(in_fd, &header, 4);
  fileSize = header[0];

  while((sizeRead = read(in_fd, &buf, BUFFERSIZE)) > 0)
  {
    //process the data from the buffer                                          
    for (i=0; i < sizeRead; i++)
    {
      tempCh = buf[i];
      tempCh = buf[i] << writer_bits;
      bitbuf = bitbuf | tempCh;

      writer_bits++; //keep track of number of bits                             

      //decompressedChar is assigned that last 7 bits in the bitbuf             
      decompressedChar = bitbuf & 0x0000007f;

      write(out_fd, &decompressedChar, 1);
      bitbuf = buf[i] >> 8-writer_bits;

      if (writer_bits > 7) //this doesnt work...FIX ME                          
      {
        writer_bits = 0;
        bitbuf = 0x00000000;
        i--;
      }
    }
    printf("new file size: %d", sizeOfFile(fileName));
  }
  close(in_fd);
  close(out_fd);
  // unlink(origFile);// This will be uncommented when decompression works      
}



//finds the size of a file in bytes                                             
int sizeOfFile(char *fileName)
{
  int in_fd = open(fileName, O_RDONLY);
  int fileSize =  lseek(in_fd, 0, SEEK_END);
  close(in_fd);
  return fileSize;
}		  