#include <stdio.h>
#include <string.h>
#include <malloc.h>

THREAD_VAR FILE *f;
#define NUM_FILES 200

THREAD_VAR char* file_names[NUM_FILES];
THREAD_VAR int*   line_buf[NUM_FILES];
THREAD_VAR char buf[200];
THREAD_VAR int fn_cnt=0;

void add_line(int fn,int l)
{
  if ((l<32000)||(l<0))
  {
    int i=l/32;
    int j=l%32;
    line_buf[fn][i] |= (1<<j);
  }
  else printf("overflow: %d\n",l);
}
void add_fn(int fn,char *b)
{
  file_names[fn]=strdup(b);
  line_buf[fn]=(int*)malloc(1000*sizeof(int));
  memset(line_buf[fn],0,1000*sizeof(int));
}

void print_line(int l)
{
  FILE *fi=fopen(file_names[l],"r");
  FILE *fo;
  int i;
  int ln;
  if (fi==NULL)
  {
    printf("%s not found\n",file_names[l]);
    return;
  }
  else
  {
    char b[200];
    sprintf(b,"%s.prof",file_names[l]);
    fo=fopen(b,"w");
  }
  ln=0;
  while(!feof(fi))
  {
    char line[500];
    char *s;
    s=fgets(line,500,fi);ln++;
    if (s==NULL) break;
    if((line_buf[l][ln/32] & (1<<(ln %32))))
      fprintf(fo,"%4d + %s",ln,line);
    else
      fprintf(fo,"%4d   %s",ln,line);
  }
  fclose(fi);
  fclose(fo);
  for(i=0;i<1000;i++)
  {
    if (line_buf[l][i]!=0)
    {
      int j;
      for(j=0;j<32;j++)
        if ((1<<j) & line_buf[l][i]) printf("%d,",i*32+j);
    }
  }
  printf("\n");
}

int main(int argc, char** argv)
{
  memset(file_names,0,NUM_FILES*sizeof(char*));
  f=fopen("smon.out","r");
  if(f==NULL) { printf("cannot read smon.out\n"); return 2; }
  while(!feof(f))
  {
    if (fgets(buf,200,f)==NULL) break;
    if ((strncmp(buf,"STDIN",5)!=0)
    && (strncmp(buf,"(none)",6)!=0)
    && (strncmp(buf,"::",2)!=0))
    {
      /* get fn */
      int i=0;
      char c;
      int line_no;
      while((buf[i]!=':')&&(i<200)) i++;
      buf[i]='\0';
      if (i>=200) continue;
      while ((buf[i]!=' ')&&(i<200)) i++;
      sscanf(buf+i,"%d",&line_no);
      for(i=0;i<fn_cnt;i++)
      {
        if (strcmp(file_names[i],buf)==0) { add_line(i,line_no); break; }
      }
      if ((i==fn_cnt)&&(i<NUM_FILES))
      {
        printf("new file:%s\n",buf);
        add_fn(i,buf);
        add_line(i,line_no);
	fn_cnt++;
      }
    }
  }
  fclose(f);
  printf("----- all read\n");
  {
    int i;
    for(i=0;i<fn_cnt;i++)
    {
      printf("File %s =============================\n",file_names[i]);
      print_line(i);
    }
  }
  return(0);
}
