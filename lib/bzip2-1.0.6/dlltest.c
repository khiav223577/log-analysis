/*
   minibz2
      libbz2.dll test program.
      by Yoshioka Tsuneo (tsuneo@rr.iij4u.or.jp)
      This file is Public Domain.  Welcome any email to me.

   usage: minibz2 [-d] [-{1,2,..9}] [[srcfilename] destfilename]
*/

#define BZ_IMPORT
#include <stdio.h>
#include <stdlib.h>
#include "bzlib.h"


#ifdef _WIN32

#define BZ2_LIBNAME "libbz2-1.0.2.DLL"

#include <windows.h>
static int BZ2DLLLoaded = 0;
static HINSTANCE BZ2DLLhLib;

int BZ2DLLLoadLibrary(void){
   HINSTANCE hLib;

   if(BZ2DLLLoaded==1){return 0;}
   hLib=LoadLibrary(BZ2_LIBNAME);
   if(hLib == NULL){
      fprintf(stderr,"Can't load %s\n",BZ2_LIBNAME);
      return -1;
   }
   BZ2_bzlibVersion = (const char * (__stdcall *)(void)) GetProcAddress(hLib,"BZ2_bzlibVersion");
   BZ2_bzopen = (BZFILE * (__stdcall *)(const char *, const char *))GetProcAddress(hLib,"BZ2_bzopen");
   BZ2_bzdopen = (BZFILE * (__stdcall *)(int, const char *))GetProcAddress(hLib,"BZ2_bzdopen");
   BZ2_bzread = GetProcAddress(hLib,"BZ2_bzread");
   BZ2_bzwrite = GetProcAddress(hLib,"BZ2_bzwrite");
   BZ2_bzflush = GetProcAddress(hLib,"BZ2_bzflush");
   BZ2_bzclose = (void (__stdcall *)(BZFILE *))GetProcAddress(hLib,"BZ2_bzclose");
   BZ2_bzerror = (const char * (__stdcall *)(BZFILE *, int *))GetProcAddress(hLib,"BZ2_bzerror");

   if (!BZ2_bzlibVersion || !BZ2_bzopen || !BZ2_bzdopen
       || !BZ2_bzread || !BZ2_bzwrite || !BZ2_bzflush
       || !BZ2_bzclose || !BZ2_bzerror) {
      fprintf(stderr,"GetProcAddress failed.\n");
      return -1;
   }
   BZ2DLLLoaded=1;
   BZ2DLLhLib=hLib;
   return 0;

}
void BZ2DLLFreeLibrary(void){
   if(BZ2DLLLoaded==0){return;}
   FreeLibrary(BZ2DLLhLib);
   BZ2DLLLoaded=0;
}
#endif /* WIN32 */

void usage(void)
{
   puts("usage: minibz2 [-d] [-{1,2,..9}] [[srcfilename] destfilename]");
}

void decompress(const char *input_path, const char *output_path, int level){
    int len;
    char buff[0x1000];
    BZFILE *BZ2fp_r = NULL;
         FILE *fp_w = NULL;

    if(output_path){
        if((fp_w = fopen(output_path,"wb"))==NULL){
            printf("can't open [%s]\n",output_path);
            perror("reason:");
            exit(1);
        }
    }else{
        fp_w = stdout;
    }
    if((input_path == NULL && (BZ2fp_r = BZ2_bzdopen(fileno(stdin),"rb"))==NULL)
            || (input_path != NULL && (BZ2fp_r = BZ2_bzopen(input_path,"rb"))==NULL)){
        printf("can't bz2openstream\n");
        exit(1);
    }
    while((len=BZ2_bzread(BZ2fp_r,buff,0x1000))>0){
        fwrite(buff,1,len,fp_w);
    }
    BZ2_bzclose(BZ2fp_r);
    if(fp_w != stdout) fclose(fp_w);
}
void compress(const char *input_path, const char *output_path, int level){
    int len;
    char buff[0x1000];
    char mode[10];
    BZFILE *BZ2fp_w = NULL;
    FILE *fp_r = NULL;

    if(input_path){
        if((fp_r = fopen(input_path,"rb"))==NULL){
            printf("can't open [%s]\n",input_path);
            perror("reason:");
            exit(1);
        }
    }else{
        fp_r = stdin;
    }
    mode[0]='w';
    mode[1] = '0' + level;
    mode[2] = '\0';

    if((output_path == NULL && (BZ2fp_w = BZ2_bzdopen(fileno(stdout),mode))==NULL)
            || (output_path !=NULL && (BZ2fp_w = BZ2_bzopen(output_path,mode))==NULL)){
        printf("can't bz2openstream\n");
        exit(1);
    }
    while((len=fread(buff,1,0x1000,fp_r))>0){
        BZ2_bzwrite(BZ2fp_w,buff,len);
    }
    BZ2_bzclose(BZ2fp_w);
    if(fp_r!=stdin)fclose(fp_r);
}
int main(int argc,char *argv[])
{
   int decompress_flag = 0;
   int level = 9;
   char *input_path = NULL;
   char *output_path = NULL;

#ifdef _WIN32
   if(BZ2DLLLoadLibrary()<0){
      fprintf(stderr,"Loading of %s failed.  Giving up.\n", BZ2_LIBNAME);
      exit(1);
   }
   printf("Loading of %s succeeded.  Library version is %s.\n",
          BZ2_LIBNAME, BZ2_bzlibVersion() );
#endif


    compress("test/test.txt", "test/test.bz2", 9);
    decompress("test/test.bz2", NULL, 9);
    return 0;
   while(++argv,--argc){
       break;
      if(**argv =='-' || **argv=='/'){
         char *p;

         for(p=*argv+1;*p;p++){
            if(*p=='d'){
               decompress_flag = 1;
            }else if('1'<=*p && *p<='9'){
               level = *p - '0';
            }else{
               usage();
               exit(1);
            }
         }
      }else{
         break;
      }
   }
   if(argc>=1){
      input_path = *argv;
      argc--;argv++;
   }else{
      input_path = NULL;
   }
   if(argc>=1){
      output_path = *argv;
      argc--;argv++;
   }else{
      output_path = NULL;
   }
   {

      if(decompress_flag){
        decompress(input_path, output_path, level);
      }else{
        compress(input_path, output_path, level);

      }
   }
#ifdef _WIN32
   BZ2DLLFreeLibrary();
#endif
   return 0;
}
