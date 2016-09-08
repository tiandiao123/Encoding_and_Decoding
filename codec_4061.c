#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codec.c"
#include "codec.h"
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <time.h>
#define pathmax 255


void encodefile(char *dirname,char *dpath,char *outputdirname,char *outfile,FILE *file,char *parentpath,char *enter_outputpath);
void decodefile(char *dirname,char *dpath,char *outputdirname,char *outfile,FILE *file,char *parentpath,char *enter_outputpath);
int isdirectory(char *path);
int isregularfile(const char *path);
void addintonewfile(char a[],char *pathfrom,char *outputdirname,char *filename,size_t size,char *enter_outputpath);
void addintonewfile2(char a[],char *pathfrom,char *outputdirname,char *filename,size_t size,char *reportfilename,FILE *file,char *enter_outputpath);

void decodeintonewfile(uint8_t a[],char *parentpath,char *outputdirname,char *filename,size_t size, char *enter_outputpath,int check,FILE *fp_r);

int compare(char *a,char *b)
{
    return strcmp(a,b);
}


int main(int argc,char *argv[]){
       
  char r_filename[100];
                
              if ((mkdir (argv[3], 0740)) == 0) 
               {
                     printf ("New directory created successfully.\n");
                     char *currentpath=malloc(pathmax);
                     getcwd(currentpath,pathmax);
                     
                     char enter_outputpath[pathmax];
                     getcwd(enter_outputpath,pathmax);
                     char a[40]="/";
                     strcat(a,argv[3]);
                     strcat(enter_outputpath,a);
                     
                     chdir(argv[3]);
                     FILE * fp;
                     char *namea=malloc(50);
                     strcpy(namea,"<");
                     strcat(namea,argv[2]);
                     strcat(namea,">");
                     strcat(namea,"_unsortedreport.txt");
                     
                     char *reportfilename=malloc(40);
                     strcpy(reportfilename,"<");
                     strcat(reportfilename,argv[2]);
                     strcat(reportfilename,">");
                     strcat(reportfilename,"_report.txt");
                     //strcpy(r_filename,namea);
                     fp = fopen (namea, "a");
                     chdir(currentpath);
                     printf("the current path:%s\n",currentpath);
                   if(strcmp(argv[1],"-e")==0){
                        encodefile(argv[2],currentpath,argv[3],namea,fp,currentpath,enter_outputpath);
                    }
                   if (strcmp(argv[1],"-d")==0){
                       decodefile(argv[2],currentpath,argv[3],namea,fp,currentpath,enter_outputpath);
                    }
                  chdir(enter_outputpath);
                  fclose(fp); 
                  
                  int fd;
                  fd = open(reportfilename, O_WRONLY | O_CREAT, 0777 );
                  if(fd==-1){
                      perror("open reportfile failed");
                       exit(1);
                   }
                  dup2(fd,STDOUT_FILENO);
                  close(fd);
                  
                  char *argv1[]={"sort",namea,NULL};
                  pid_t childpid=fork();
                 if(childpid==0){
                     execvp(argv1[0],argv1);}
                  else {  
                        wait(NULL);
                       char *argv2[]={"rm",namea,NULL};
                        execvp(argv2[0],argv2);
                   }
               }
              else
                {
                       perror ("Error in directory creation");
                       exit(1);
                }
             
            
              
           return 0;
 }




void encodefile(char *targetdirname,char *start_path,char *outputdirname,char *reportfilename, FILE* fp_r,char *parentpath,char *enter_outputpath){
     struct dirent *direntp;
     DIR *dirp;
     dirp=opendir(targetdirname);
     if (dirp==NULL){
     fprintf(stderr,"open dir failed");
       exit(1);
       }   


     char change_path[pathmax];
      strcpy(change_path,start_path);
       char temp[40]="/";
       strcat(temp,targetdirname);
       strcat(change_path,temp);    
                               
     while ((direntp=readdir(dirp))!=NULL)
       {    chdir(change_path);
              
          if(isdirectory(direntp->d_name)>0)
             {  char *subname=direntp->d_name;
                fprintf(stderr,"this is %s\n",subname);
                if (strlen(subname)>2){
                  struct stat statbuf;
                  char filepath[pathmax];
                   strcpy(filepath,change_path);
                   strcpy(temp,"/");
                   strcat(temp,direntp->d_name);
                   strcat(filepath,temp);                 
               
                  stat(filepath,&statbuf);
                  long originalsize=(long)statbuf.st_size; 
                  
                 // fprintf(fp_r, "%s %s %ld\n", direntp->d_name,"directory",(long)statbuf.st_size);
                     
                       chdir(enter_outputpath);
                       char new_outputpath[pathmax];
                       strcpy(new_outputpath,enter_outputpath);
                       char a[30]="/";
                       strcat(a,direntp->d_name);
                       strcat(new_outputpath,a);
                       if ((mkdir (direntp->d_name, 0740)) == 0) {
                           printf("subdirectory was created sucessfully\n");}
                       else {
                                   fprintf(stderr,"sundirectory didn't create\n");
                                    exit(1);
                          }
                         chdir(change_path);
           
                  encodefile(direntp->d_name,change_path,outputdirname,reportfilename,fp_r,parentpath,new_outputpath);
                       long currentsize=(long)statbuf.st_size;
                       fprintf(fp_r, "%s %s %ld %ld\n", direntp->d_name,"directory",originalsize,currentsize);
                       
                 }
             }
          else if(isregularfile(direntp->d_name)>0){
                 printf("this is %s\n",direntp->d_name);
                  
                  struct stat statbuf;
                  char filepath[pathmax];
                      strcpy(filepath,change_path);
                   strcpy(temp,"/");
                    strcat(temp,direntp->d_name);
                     strcat(filepath,temp);                 
               
                  stat(filepath,&statbuf);
                  long orginalsize=(long)statbuf.st_size;
                  fprintf(fp_r, "%s %s %ld", direntp->d_name,"regular file",(long)statbuf.st_size);
                  
                  FILE *file;
                  
                  file=fopen(direntp->d_name,"r"); 
                  int check=0;
                  int i;
                  char c;
                   
                  do
                     {  
                        char *buf1=malloc(3*sizeof(uint8_t));
                        char *buf2=malloc(4*sizeof(uint8_t));
                        for(i=0;i<3;){                         
                         c=fgetc(file);
                         buf1[i]=c;
                         i++;
                         if( c==EOF )
                          {   check=1;
                              break ;
                           }                         
                                                  
                         //fprintf(stderr,"%c",c);
                         }  
                       
                         if((check==0)&&(i==3))
                            { size_t num=encode_block(buf1, buf2, 3);
                              addintonewfile(buf2,parentpath,outputdirname,direntp->d_name,num/sizeof(uint8_t),enter_outputpath);}
                         else if ((check==1)&&(i>1))
                             { size_t num=encode_block(buf1,buf2,i-1);
                                addintonewfile2(buf2,parentpath,outputdirname,direntp->d_name,num/sizeof(uint8_t),reportfilename,fp_r,enter_outputpath);}
                         else if((check==1)&&(i==1))
                             {    size_t num=encode_block(buf1,buf2,i-1);
                                   addintonewfile2(buf2,parentpath,outputdirname,direntp->d_name,0,reportfilename,fp_r,enter_outputpath);}
                      
                         free(buf1);
                         free(buf2);
                     }while(check==0);
                
                 fclose(file);
                  
                  
             }
          chdir(start_path);
       }


}


void addintonewfile(char a[],char *pathfrom,char *outputdirname,char *filename,size_t size,char *enter_outputpath){//pass elements into a file in the outputdir
     char previouspath[pathmax];
     getcwd(previouspath,pathmax);
     char path[pathmax];
      strcpy(path,enter_outputpath);
    
     chdir(path);
     FILE *fp;
     fp=fopen(filename,"a");
      int i=0;
     for(;i<size;i++)
     {
        fputc(a[i],fp);
     }
     fclose(fp);
    chdir(previouspath);
}


void addintonewfile2(char a[],char *pathfrom,char *outputdirname,char *filename,size_t size,char *reportfilename,FILE *file,char *enter_outputpath){
//pass elements into a file in the outputdir
     char previouspath[pathmax];
     getcwd(previouspath,pathmax);
     
     chdir(enter_outputpath);
      
     FILE *fp;
     fp=fopen(filename,"a");
      int i=0;
     for(;i<size;i++)
     {
        fputc(a[i],fp);
     }
     
     //fputc('\n',fp);
     
     fclose(fp);

     struct stat statbuf;
     char filepath[pathmax];                
     getcwd(filepath,pathmax);
     
     char b[50]="/";
     strcat(b,filename);
     strcat(filepath,b);
     stat(filepath,&statbuf);
     fprintf(stderr,"can we get here?\n");
     long currentsize=(long)statbuf.st_size;
    
     if(currentsize==0)
         fprintf(file, " %ld\n", currentsize);
     else
        {    FILE *fp1;
             fp1=fopen(filename,"a");
             fputc('\n',fp1);
             fclose(fp1);
             struct stat statbuf1;
             stat(filepath,&statbuf1);
             
             fprintf(file," %ld\n",(long)statbuf1.st_size);
         }
     chdir(previouspath);
}


void decodefile(char *targetdirname,char *start_path,char *outputdirname,char *reportfilename,FILE *fp_r,char *parentpath,char *enter_outputpath){
     struct dirent *direntp;
     DIR *dirp;
     dirp=opendir(targetdirname);
     if (dirp==NULL){
     fprintf(stderr,"open dir failed");
       exit(1);
       }             
      
       char change_path[pathmax];
       strcpy(change_path,start_path);
       char temp[40]="/";
       strcat(temp,targetdirname);
       strcat(change_path,temp);    
       
       while((direntp=readdir(dirp))!=NULL){
                 chdir(change_path);
           if(isdirectory(direntp->d_name)>0){
                 char *subname=direntp->d_name;
                fprintf(stderr,"this is %s\n",subname);
                if (strlen(subname)>2){
                  struct stat statbuf;
                  char filepath[pathmax];
                   strcpy(filepath,change_path);
                   strcpy(temp,"/");
                   strcat(temp,direntp->d_name);
                   strcat(filepath,temp);                 
               
                  stat(filepath,&statbuf);
                  long originalsize=(long)statbuf.st_size;
                  //fprintf(fp_r, "%s %s %ld\n", direntp->d_name,"directory",(long)statbuf.st_size);
                   
                       chdir(enter_outputpath);
                       char new_outputpath[pathmax];
                       strcpy(new_outputpath,enter_outputpath);
                       char a[30]="/";
                       strcat(a,direntp->d_name);
                       strcat(new_outputpath,a);
                       if ((mkdir (direntp->d_name, 0740)) == 0) {
                           printf("subdirectory was created sucessfully\n");}
                       else {
                                   fprintf(stderr,"sundirectory didn't create\n");
                                    exit(1);
                          }
                         chdir(change_path);                                                                                     
                  decodefile(direntp->d_name,change_path,outputdirname,reportfilename,fp_r,parentpath,new_outputpath);

                   fprintf(fp_r, "%s %s %ld %ld\n", direntp->d_name,"directory",originalsize,(long)statbuf.st_size);
                 }

           }
          else if(isregularfile(direntp->d_name)>0){
                  printf("this is %s\n",direntp->d_name);
                  
                  struct stat statbuf;
                  char filepath[pathmax];
                   strcpy(filepath,change_path);
                   strcpy(temp,"/");
                   strcat(temp,direntp->d_name);
                   strcat(filepath,temp);                 
               
                  stat(filepath,&statbuf);
                  long orginalsize=(long)statbuf.st_size;
                  fprintf(fp_r, "%s %s %ld", direntp->d_name,"regular file",(long)statbuf.st_size);

                  FILE *file;
                  
                  file=fopen(direntp->d_name,"r"); 
                  int check=0;
                  int i;
                  char c;
                  do{
                       uint8_t *buf1=malloc(4*sizeof(uint8_t));
                       uint8_t *buf2=malloc(3*sizeof(uint8_t));   
                          for(i=0;i<4;){                         
                               c=fgetc(file);
                               buf1[i]=c;
                               i++;
                               if( c==EOF )
                                 {   check=1;
                                    break;
                                 }                         
                                                  
                               //fprintf(stderr,"%c",c);
                           }  
                        if(check==0)
                           {
                               size_t num=decode_block(buf1, buf2);
                               decodeintonewfile(buf2,parentpath,outputdirname,direntp->d_name,num,enter_outputpath,check,fp_r);
                            }
                        else if(check==1){
                                size_t num=decode_block(buf1, buf2);
                                decodeintonewfile(buf2,parentpath,outputdirname,direntp->d_name,0,enter_outputpath,check,fp_r);
                        }

                  }while(check==0);
                      
           }
            chdir(start_path);
       }
     
     
}

void decodeintonewfile(uint8_t a[],char *parentpath,char *outputdirname,char *filename,size_t size, char *enter_outputpath,int check,FILE *fp_r){
     char currentpath[pathmax];
     getcwd(currentpath,pathmax);
     
     chdir(enter_outputpath);
     FILE *fp;
     fp=fopen(filename,"a");
     int i;
     for(i=0;i<size;i++)
     {
         fputc(a[i],fp);
     }
     fclose(fp);
     if(check==1)
      {   struct stat statbuf;
          char filepath[pathmax];
          strcpy(filepath,enter_outputpath);
          char b[40]="/";   
         strcat(b,filename);
         strcat(filepath,b);
         stat(filepath,&statbuf);      
          fprintf(fp_r," %ld\n",(long)statbuf.st_size);
      }
    chdir(currentpath); 
           

}



int isdirectory(char *path){//this function is referenced from the book in page 157;

    struct stat statbuf;
    stat(path,&statbuf);
    return S_ISDIR(statbuf.st_mode);
   
 }

int isregularfile(const char *path)
{
    struct stat pstat;
    stat(path, &pstat);
    return S_ISREG(pstat.st_mode);
}

