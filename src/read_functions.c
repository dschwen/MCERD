/*
    Copyright 2010 Kai Arstila, Jouni Heiskanen

    This file is part of MCERD.

    MCERD is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MCERD is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MCERD. If not, see <http://www.gnu.org/licenses/>.
*/



#include "general.h"
#include "prototypes.h"
#include "read_input.h"
#include "protot.h"

/* All functions needed for reading a file collected from the original mcerd source files
 * (read_input.c, read_detector.c and read_target.c) */
 

Fvalue *read_file(char *fname,int cols,int *n)
{
   FILE *fp;
   Fvalue *f;
   char buf[LINE];
   int i=0;
      
   f = (Fvalue *) malloc(sizeof(Fvalue));
   
   fp = fopen(fname,"r");
   
   if(fp==NULL){
      /* fprintf(stderr,"Could not open file %s\n",fname);
      exit(10); */
      error_dialog_open(fname);
      free(f);
      longjmp(err_buf,1);
   }

   while(fgets(buf,LINE,fp)!=NULL){
      switch(cols){
         case 1:
            sscanf(buf,"%lf",&(f->a[i]));
            break;
         case 2:
            sscanf(buf,"%lf %lf",&(f->a[i]),&(f->b[i]));
            break;      
         case 3:
            sscanf(buf,"%lf %lf %lf",&(f->a[i]),&(f->b[i]),&(f->c[i]));
            break;      
      }
      i++;
   }
   fclose(fp);
   
   *n = i;
   
   return(f);
   
}
void get_atom(char *c,double *z)
{
   FILE *fp;
   char S[3],*symbol;
   int Z,n,found=FALSE;

   symbol = get_word(c,&n);

   fp = fopen(FILE_MASSES,"r");
   
   if(fp == NULL){
      /* fprintf(stderr,"Could not open file %s\n",FILE_MASSES);
      exit(10); */
      error_dialog_open(FILE_MASSES);
      free(symbol);
      longjmp(err_buf,1);
   }

   while(!found && fscanf(fp,"%*i %i %*i %s %*f %*f",&Z,S)==2)
      if(strcmp(S,symbol) == 0)
         found = TRUE;

   fclose(fp);
   free(symbol);

   if(!found) {
      /* fprintf(stderr,"atom data not found from database\n");
      exit(12); */
      error_dialog("atom data not found from database");
      longjmp(err_buf,1);
   } else{
      *z = (double) Z; 
   }

}
void get_ion(char *c,double *z,double *m,Isotopes *I)
{
   FILE *fp;
   char S[3],*symbol;
   double U,number,C,Amax=-1.0;
   int N,Z,A,found = FALSE,mass=-1.0,n,Z1=0;

   if(isdigit(*c)){
      c=get_number(c,&number);
      mass = (int) (number+0.5);
   }
   symbol = get_word(c,&n);

   fp = fopen(FILE_MASSES,"r");
   
   if(fp == NULL){
      /* fprintf(stderr,"Could not open file %s\n",FILE_MASSES);
      exit(10); */
      error_dialog_open(FILE_MASSES);
      free(symbol);
      longjmp(err_buf,1);
   }

   n = 0;
   if(mass > 0.0){
      while(!found && fscanf(fp,"%i %i %i %s %lf %lf",&N,&Z,&A,S,&U,&C)==6)
         if((A==mass) && (strcmp(S,symbol) == 0))
            found = TRUE;
   } else {
      while(fscanf(fp,"%i %i %i %s %lf %lf",&N,&Z,&A,S,&U,&C)==6)
         if(strcmp(S,symbol) == 0 && C > 0.0){
            found = TRUE;
            Z1 = Z;
            I->A[n] = U*C_U/1.0e6;
            if(C > Amax){
               Amax = C;
               I->Am = U*C_U/1.0e6;
            }
            I->c[n] = C/100.0;
            n++;
         }
   }   

   fclose(fp);
   free(symbol);

   I->n = n;

   if(!found) {
      /* fprintf(stderr,"atom data not found from database\n");
      exit(12); */
      error_dialog("atom data not found from database");
      longjmp(err_buf,1);
   } else if(n > 0) {
         *m = I->Am;
         *z = Z1;
      } else {
         *m = U*C_U/1e6;
         *z = Z; 
      }

}
char *get_string(char *c)
{

/* 
   String can contain white spaces, thus there can be only one string per line
*/
   char *p;
   size_t i=0;

   while(*(c+i) != '\0')
      i++; 
   
   i--;
   
   while(i > 0 && isspace(*(c+i)))
      i--;

   i++;

   if(*(c+i) == '\0')
      return(NULL);

   p = (char *) malloc(sizeof(char)*(i+1));
   
   p = strncpy(p,c,i);
   
   p[i] = '\0';

   return(p);
  
}
char *get_word(char *c,int *n)
{
   char *p;
   size_t i=0;

   while(*(c+i) != '\0' && !isspace(*(c+i)))
      i++; 
   
   i--;
   
   while(i > 0 && isspace(*(c+i)))
      i--;

   i++;

   if(i == 0)
      return(NULL);

   p = (char *) malloc(sizeof(char)*(i+1));
   
   p = strncpy(p,c,i);
   
   p[i] = '\0';

   *n = i;
   return(p);
  
}
char *get_number(char *c,double *number)
{
   int t;
   char *s;
   size_t i=0;
   
   while(isnumber(*(c+i)))
      i++;
   if((*(c+i) == 'e' || *(c+i) == 'E') && i != 0 && *(c+i) != '\0' 
      && *(c+i+1) != '\0' && 
      (isnumber(*(c+i+1)) || *(c+i+1) == '-' || *(c+i+1) == '+')){
      t = i;
      i++;
      while(isnumber(*(c+i)))
         i++;
      if(t == (i+1))
         i = t;
   }
   
   if(i == 0)
      return(NULL);
   if(*(c+i) == '\0'){
      *number = atof(c);
   } else {
      s = (char *) malloc((i+1)*sizeof(char));
      strncpy(s,c,i);
      s[i] = '\0';
      *number = atof(s);
/*
      free(s);
*/
   }

   c = trim_space(c+i);     
   return(c);
}
char *get_unit_value(char *c,double *unit,double def)
{
   int j= -1,v;
   char *u;
   size_t i=0;
/*   
   while(*(c+i) != '\0' && !isnumber(*(c+i)) && !isspace(*(c+i)))
      i++;
*/
   while(*(c+i) != '\0' && !isspace(*(c+i)))
      i++;

   if(i == 0){
      *unit = def;
      return(c);
   }
   
   u = (char *) malloc(sizeof(char)*(i+1));
   strncpy(u,c,i);
   u[i] = '\0';
   
   do {
      j++;
      v = strcmp(u,units[j].unit);
   } while(units[j].value > 0.0 && v != 0);

   if(units[j].value < 0.0){
      /* fatal_error("Could not find value for given unit"); */
      error_dialog("Could not find value for given unit");
      free(u);
      longjmp(err_buf,1);
   }

   *unit = units[j].value;
   
   c = trim_space(c+i);   

   return(c);
}
char *trim_space(char *c)
{
   while(*c != '\0' && isspace(*c))
      c++;
      
   return(c);
}
int isnumber(char c)
{
   if(isdigit(c) || c == '-' || c == '.' || c == ',' || c == '+') /* added ',' for windows */
      return(TRUE);
   else
      return(FALSE);
      
}
TextLine *addline(TextLine *line,char *buf)
{
   int len;
   TextLine *tmp;

   len = strlen(buf) + 1;

   tmp = (TextLine *) malloc(sizeof(TextLine));
   tmp->text = (char *) malloc(len*sizeof(char));
   tmp->next = line;
   tmp->read = FALSE;
   strcpy(tmp->text,buf);

   line = tmp;

   return(line);
}

int line_not_empty(char *buf)
{
   int empty = TRUE,i;

   for(i=0;(i<LINE) && (buf[i] != '\0') && (empty == TRUE);i++)
      if(!isspace(buf[i]))
         empty = FALSE;

   return(!empty);
}

