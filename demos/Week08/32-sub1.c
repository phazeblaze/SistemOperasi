/*
 * (C) 2018 Rahmat M. Samik-Ibrahim
 * You are free to SHARE and to ADAPT,
 * but WITHOUT ANY WARRANTY.
 * REV03 Mon Dec 10 18:54:10 WIB 2018
 * REV02 Wed Nov 21 20:49:00 WIB 2018
 * START Wed Nov 14 20:30:05 WIB 2018
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MYFLAGS      O_CREAT | O_RDWR
#define MYPROTECTION PROT_READ|PROT_WRITE
#define MYVISIBILITY MAP_SHARED 

typedef  struct {
   sem_t  sync[3];
   int    share;
   int    loop;
   pid_t  relative;
} myshare;

myshare* mymap;
char*    sfile="demo-file.bin";

void flushprintf(char* tag1, char* tag2){
   printf("%s[%s] loop%d relative(%d)\n", 
      tag1, tag2, mymap->loop, 
      getpid()  + mymap->relative);
   fflush(NULL);
}

void main(int argc, char* argv[]) {
   int fd  =open(sfile,MYFLAGS,S_IRWXU);
   int ssize=sizeof(myshare);
   mymap=mmap(NULL, ssize, MYPROTECTION, 
              MYVISIBILITY, fd, 0);
   sem_post (&(mymap->sync[1]));
   sem_wait (&(mymap->sync[2]));
   sem_wait (&(mymap->sync[2]));
   mymap->share=2000;
   flushprintf(argv[0], "PASS");
   while (mymap->loop) {
      for(int ii=0; ii<1000000; ii++);
      mymap->share--;
   }
   sem_post (&(mymap->sync[1]));
   sem_wait (&(mymap->sync[2]));
   sem_post (&(mymap->sync[1]));
   flushprintf(argv[0], "EXIT");
   sem_post (&(mymap->sync[0]));
   close(fd);
}

//       1         2         3         4
// 4567890123456789012345678901234567890


