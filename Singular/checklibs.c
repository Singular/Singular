#include <stdio.h>
#include <string.h>
#include <ctype.h>


#define NUM_PROC 200
#define LINE_LEN 200
#define RECOMMENDED_LEN 100
VAR FILE *f;
VAR int trailing_spaces=0;
VAR int tabs=0;
VAR int verylong_lines=0;
VAR int lines=0;
VAR unsigned char buf[LINE_LEN];
VAR int proc_cnt=0;
VAR unsigned char *proc[NUM_PROC];
VAR unsigned char have_doc[NUM_PROC];
VAR unsigned char have_example[NUM_PROC];
VAR unsigned char proc_found[NUM_PROC];
VAR int non_ascii=0;
VAR int non_ascii_line=0;
VAR int star_nl=0;
VAR int footer=0;
VAR int header=0;
VAR int crlf=0;
VAR int proc_help_lines=0;
VAR int proc_help_texinfo=0;

void get_next()
{
  int i;
  memset(buf,0,LINE_LEN);
  fgets(buf,LINE_LEN,f);
  lines++;
  if (buf[0]!='\0')
  {
    int non_ascii_found=0;
    if (strchr(buf,'\r')!=NULL) crlf++;
    if ((buf[LINE_LEN-1]!='\0')||(strlen(buf)>RECOMMENDED_LEN))
    {
      if (verylong_lines==0) printf("warning: very long line (%d):\n%s\n",lines,buf);
      verylong_lines++;
    }
    if ((strstr(buf," \n")!=NULL)||(strstr(buf," \r\n")!=NULL)) trailing_spaces++;
    if (strchr(buf,'\t')!=NULL) tabs++;

    for(i=0;(i<LINE_LEN) && (buf[i]!='\0'); i++)
    {
      if (buf[i]>=127) { non_ascii_found=1;non_ascii++;non_ascii_line=lines; break; }
    }
    if (non_ascii_found) printf("non-ascii:>>%s<<\n",buf);
    if (footer==0) /* we are still in the header */
    {
      if (strstr(buf,"@*")!=NULL) star_nl++;
    }
  }
}

void scan_proc(int *l)
{
  unsigned char *p;
  while(1)
  {
    get_next(); (*l)++;
    if (((p=strchr(buf,'('))!=NULL)&&(isalnum(*(--p))||(*p=='_')))
    {
      unsigned char *s=buf;
      while(*s==' ') s++;
      p++; (*p)='\0';
      if ((((int)(long)(s-buf))>10)||(strchr(s,' ')!=NULL))
      {
        printf("warning: probably not a proc ? (%s)\n",s);
      }
      else
      {
        if (strlen(s)<4)
          printf("error: minimal length of a procedure name is 4: %s\n",s);
        proc[proc_cnt]=strdup(s); proc_cnt++;
      }
    }
    else if (strstr(buf,"LIB ")!=NULL) break;
    else if (strstr(buf,"LIB\"")!=NULL) break;
    else if (strstr(buf,"proc ")!=NULL) break;
    else if (strncmp(buf,"\";",2)==0) break; /* poor mans end-of-info*/
    else if ((p=strstr(buf,":"))!=NULL)
    { /* handles all capital letters + : */
      /* SEE ALSO, KEYWORDS, NOTE, ... */
      int ch;
      unsigned char *pp=buf;
      while((*pp==' ')||(*pp=='\t')) pp++;
      ch=strspn(pp,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      if ((ch>1)||(pp+ch==p))
      {
        break;
      }
    }
  }
  if (proc_cnt==0)
    printf("warning: no proc found in the section PROCEDURES ?\n");
  printf("\n# proc mentioned in the header: %d\n",proc_cnt);
}

void scan_keywords(int *l)
{
  /* the main problem with KEYWORDS: seperator between is ;,
   * but it MUST NOT appear at the end */
  unsigned char *p;
  while(!feof(f))
  {
    p=strrchr(buf,';'); /* the last ; in the line*/
    if (p==NULL) { get_next(); (*l)++; return; }
    while (*p==' ') p++;
    if (isalpha(*p)) { get_next(); (*l)++; return; }
    if (*p=='\0') { get_next(); (*l)++; }
    else if (strstr(buf,"LIB ")!=NULL) break;
    else if (strstr(buf,"LIB\"")!=NULL) break;
    else if (strstr(buf,"proc ")!=NULL) break;
    else if (strncmp(buf,"\";",2)==0) break; /* poor mans end-of-info*/
    else if ((p=strstr(buf,":"))!=NULL)
    { /* handles all capital letters + : */
      /* SEE ALSO, KEYWORDS, NOTE, ... */
      int ch;
      unsigned char *pp=buf;
      while((*pp==' ')||(*pp=='\t')) pp++;
      ch=strspn(pp,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      if ((ch>1)||(pp+ch==p))
      {
        break;
      }
    }
  }
  printf("error: seperate keywords by ; but do not have ; after the last keyword\n");
}
void scan_proc_help(const char *s)
{
  while(!feof(f))
  {
    proc_help_lines++;
    if (strstr(buf,"\";")!=NULL) break;
    if (buf[0]=='{') break;
    if (strstr(buf,"@")!=NULL)
    {
      proc_help_texinfo++;
      buf[strlen(buf)-1]='\0';
      strcat(buf,"<<\n");
      printf("texinfo in proc help(%s): >>%s",s,buf);
    }
    get_next();
  }
}
void scan_info(int *l)
{
  int have_LIBRARY=0;
  int have_AUTHORS=0;
  int have_PROCEDURES=0;
  int have_SEEALSO=0;
  int have_KEYWORDS=0;
  int have_OVERVIEW=0;
  int have_NOTE=0;
  int have_other=0;
  int texinfo=0;
  unsigned char *p;

  while(!feof(f))
  {
    if (strstr(buf,"LIBRARY: ")!=NULL)
    {
      have_LIBRARY++;
      /* musrt be first*/
      if (have_other+have_AUTHORS+have_PROCEDURES+have_KEYWORDS+have_SEEALSO!=0)
        printf("error: LIBRARY: must be the first section in info\n");
    }
    else if (strstr(buf,"NOTE:")!=NULL)
    {
      if (have_PROCEDURES!=0)
        printf("error: only KEYWORDS/SEE ALSO may follow PROCEDURES\n");
      have_NOTE++;
    }
    else if (strstr(buf,"OVERVIEW:")!=NULL)
    {
      have_OVERVIEW++;
      if (have_PROCEDURES!=0)
        printf("error: only KEYWORDS/SEE ALSO may follow PROCEDURES\n");
    }
    else if (strstr(buf,"KEYWORDS: ")!=NULL)
    {
      have_KEYWORDS++;
    }
    else if (strstr(buf,"SEE ALSO: ")!=NULL)
    {
      have_SEEALSO++;
    }
    else if ((strstr(buf,"AUTHORS: ")!=NULL)
      ||(strstr(buf,"AUTHOR: ")!=NULL))
    {
      have_AUTHORS++;
      if (have_PROCEDURES!=0)
        printf("error: only KEYWORDS/SEE ALSO may follow PROCEDURES\n");
    }
    else if ((p=strstr(buf,"PROCEDURES"))!=NULL)
    {
      unsigned char *pp=buf;
      while (pp!=p)
      {
       if ((*pp!=' ')&&(*pp!='\t')) break;
       pp++;
      }
      if (p==pp)
      {
        have_PROCEDURES++;
        scan_proc(l);
        continue;
      }
      else
      {
        printf("error: unknown section in library header: %s",buf);
        have_other++;
      }
    }
    else if ((p=strstr(buf,":"))!=NULL)
    {
      int ch;
      unsigned char *pp=buf;
      while((*pp==' ')||(*pp=='\t')) pp++;
      ch=strspn(pp,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      if ((ch>1)||(pp+ch==p))
      {
        /* check for other allowed sections: REFERENCES*/
        if ((ch!=10)||(strncmp(pp,"REFERENCES",10)!=0))
        {
          printf("error: unknown section in library header: %s",buf);
          have_other++;
        }
        if (have_PROCEDURES!=0)
          printf("error: only KEYWORDS/SEE ALSO may follow PROCEDURES\n");
      }
    }
    else if (strncmp(buf,"\";",2)==0) goto e_o_info; /* poor mans end-of-info*/
    else
    {
      p=buf;
      if (strchr(buf,'@')!=NULL)
      { texinfo++; printf("%s",buf); }
    }
    get_next(); (*l)++;
  }
  e_o_info:
  printf("\nSUMMARY OF THE HEADER:\n");
    if (have_LIBRARY!=1)
      printf("error: missing/duplicate LIBRARY (%d lines found, should be 1)\n",have_LIBRARY);
    if (have_AUTHORS!=1)
      printf("error: missing/duplicate AUTHOR/AUTHORS (%d lines found, should be 1)\n",have_AUTHORS);
    if (have_PROCEDURES!=1)
      printf("error: missing/duplicate PROCEDURES (%d lines found, should be 1)\n",have_PROCEDURES);
    if (have_SEEALSO>1)
      printf("error: duplicate SEE ALSO (%d lines found)\n",have_SEEALSO);
    if (have_KEYWORDS>1)
      printf("error: duplicate KEYWORDS (%d lines found)\n",have_KEYWORDS);
    if (have_NOTE==1)
      printf("hint: avoid NOTE: if not used for a library requirement\n");
    else if (have_NOTE>1)
      printf("error: duplicate NOTE (%d lines found)\n",have_NOTE);
    if ((have_OVERVIEW==1)&&(proc_cnt<3))
      printf("hint: avoid OVERVIEW: for small libraries\n");
    else if (have_OVERVIEW>1)
      printf("error: duplicate OVERVIEW (%d lines found)\n",have_OVERVIEW);

    if (have_other!=0)
      printf("error: other header entries found (illegal ?) :%d lines found, should be 0\n",have_other);
    if ((star_nl>0)&&(star_nl*10>=header))
    {
      printf("warning: %d forced line breaks in %d header lines: @* should be used very rarely!\n",star_nl,header);
    }
    if (texinfo>0)
    {
      printf("warning: %d texinfo commands in %d header lines: should be used very rarely!\n",texinfo,header);
    }
}

int main(int argc, char** argv)
{
  int have_version=0;
  int have_category=0;
  int have_info=0;
  unsigned char *p;

  memset(proc,0,NUM_PROC*sizeof(char*));
  memset(have_doc,0,NUM_PROC);
  memset(have_example,0,NUM_PROC);
  memset(proc_found,0,NUM_PROC);
  if (argc!=2) { printf("usage: %s lib-file\n",argv[0]); return 1;}

  printf("\n          CHECKING LIBRARY %s\n\n",argv[1]);

  f=fopen(argv[1],"r");
  if(f==NULL) { printf("cannot read %s\n",argv[1]); return 2; }
  buf[0]='\0';
  get_next(); header++;
  if (strncmp(buf,"//",2)!=0) { printf("error: lib must start with //\n"); }
  else { get_next(); header++; }
  /* pass 1: check header */
  while(1)
  {
    if ((p=strstr(buf,"version="))!=NULL)
    {
      unsigned char *pp=buf;
      while (pp!=p)
      {
       if ((*pp!=' ')&&(*pp!='\t')) break;
       pp++;
      }
      if (p=pp)
      {
        have_version++;
        pp=p+8;
        while((*pp)==' ') pp++;
	/* syntax of version string: "version <filename> <version> <date> "
        if (*pp)!='"')
          printf("error: version string should ....");
	*/
      }
    }
    if ((p=strstr(buf,"category="))!=NULL)
    {
      unsigned char *pp=buf;
      while (pp!=p)
      {
       if ((*pp!=' ')&&(*pp!='\t')) break;
       pp++;
      }
      if (p=pp) have_category++;
    }
    if ((p=strstr(buf,"info="))!=NULL)
    {
      unsigned char *pp=buf;
      while (pp!=p)
      {
       if ((*pp!=' ')&&(*pp!='\t')) break;
       pp++;
      }
      if (p=pp) { have_info++; scan_info(&header); }
    }
    if ((p=strstr(buf,"LIB\""))!=NULL)
    {
      printf("error: use a space between LIB and \"\n");
      if (p!=buf)
      { printf("end of header ? LIB should be in col. 1:>>%s<<\n",buf); }
      break; /* end of header */
    }
    if ((p=strstr(buf,"LIB \""))!=NULL)
    {
      if (p!=buf)
      { printf("end of header ? LIB should be in col. 1:>>%s<<\n",buf); }
      break; /* end of header */
    }
    if ((p=strstr(buf,"proc "))!=NULL)
    {
      if ((p!=buf)&&(strncmp(buf,"static proc ",12)!=0))
      { printf("end of header ? proc should be in col. 1:>>%s<<\n",buf); }
      break; /* end of header */
    }
    get_next(); header++;
    if(feof(f)) break;
  }
  printf("header parsed: %d lines of %s\n\n",header,argv[1]);
  /* part 2: procs */
  while(!feof(f))
  {
    if ((strstr(buf,"static")==(char*)buf) && (strstr(buf,"proc")==NULL))
    {
      printf("error: 'static' without 'proc' found\n");
      get_next();
    }
    if(((p=strstr(buf,"proc "))!=NULL)
    &&(strncmp(buf,"static proc ",12)!=0))
    {
      unsigned char *pp=buf;
      int i;
      while(*pp==' ') pp++;
      if ((pp!=buf)&&(pp==p))
      {
        printf("warning: proc should be in col. 1: line %d:%s",lines,buf);
      }
      else if (pp!=p)
      {
        footer++; get_next(); continue; /* this is not a proc start*/
      }
      p+=5; /* skip proc+blank*/
      while(*p==' ') p++;
      pp=p;
      while(isalnum(*p)||(*p=='_')) p++;
      *p='\0';
      for(i=proc_cnt-1;i>=0;i--)
      {
        if(strcmp(proc[i],pp)==0) break;
      }
      if (i<0)
      {
        printf("hint: global proc %s not found in header\n",pp);
        footer++; get_next();
      }
      else
      {
        proc_found[i]=1;
        footer++; get_next(); /* doc should start at next line */
        p=buf;
        while(*p==' ') p++;
        if (*p == '"') have_doc[i]=1;
	/* scan proc help*/
	scan_proc_help(proc[i]);
        /* serach for example */
        while(!feof(f))
        {
           if(strncmp(buf,"proc ",5)==0) break;
           if(strncmp(buf,"static proc ",12)==0) break;
           if(strncmp(buf,"example",7)==0)
           {
             have_example[i]=1;
             break;
           }
           footer++; get_next();
        }
      }
    }
    else {get_next();footer++;}
  }
  {
    int i;
    for(i=proc_cnt-1; i>=0;i--)
    {
      if(proc_found[i]==0) printf("proc %s not found\n",proc[i]);
      else
      {
        if(have_doc[i]==0) printf("proc %s has no documentation\n",proc[i]);
        if(have_example[i]==0) printf("proc %s has no example (or it does not start in col. 1)\n",proc[i]);
      }
    }
  }
  /* part 3: summary*/
  printf("\nproc part parsed: %d lines of %s\n",footer,argv[1]);
  if (have_version!=1) printf("version missing/duplicate (%d)\n",have_version);
  if (have_category!=1) printf("category missing/duplicate (%d)\n",have_category);
  if (have_info!=1) printf("info missing/duplicate (%d)\n",have_info);

  printf("\nGENERAL SUMMARY:\n");
  if(tabs!=0) printf("warning: lib should not contain tabs, >=%d found\n",tabs);
  if(trailing_spaces!=0) printf("hint: lib should not contain trailing_spaces, >=%d found\n",trailing_spaces);
  if(verylong_lines!=0) printf("hint: lib should not contain very long lines, >=%d found\n",verylong_lines);
  if(non_ascii>0)
  printf("error: lib should not contain non-ascii characters, %d found, last in line %d\n",non_ascii, non_ascii_line);
  if (crlf>=lines-1)
  {
    printf("warning: DOS format (%d)\n",crlf);
  }
  else if (crlf>0)
  {
    printf("error: some lines are in DOS format, some not (%d/%d)\n",crlf,lines);
  }
  printf("%d lines parsed\n",lines);
  printf("%d proc found in header\n",proc_cnt);
  printf("%d lines found in proc help\n",proc_help_lines);
  printf("%d lines found in proc help with texinfo commands (should be very small)\n",proc_help_texinfo);
  fclose(f);
  return 0;
}
