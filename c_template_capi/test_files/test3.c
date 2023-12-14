
#include "./result/common.h"
#include "./result/types.h"
#include <stdint.h>
#include <stdlib.h>

#define T0$ int
#define T1$ char
#define T2$ float


#ifndef T0$
  #error "Template type: T0$ is required but not defined"
#endif

#ifndef T1$
  #error "Template type: T1$ is required but not defined"
#endif

#ifndef T2$
  #error "Template type: T2$ is required but not defined"
#endif
/* Auxilary Section */


#define user_given_name_TEMPLATE_FUNCTION_DEF_SECTION

#ifndef user_given_name_here_AUXILARY_SECTION
#define user_given_name_here_AUXILARY_SECTION 

void auxilary_function(size_t *);

#ifndef user_given_name_here_AUXILARY_FUNCTION_SECTION
#define user_given_name_here_AUXILARY_FUNCTION_DEFINITION_SECTION 
void auxilary_function(size_t * a){
  *a = 20;
}

#endif
#endif




#define list_T$_  F(list_T$,_)
/* Template Section */



#define template_function_T$  CONCATENATE3(template_function,T0$,T1$,T2$)
struct list_T$ list_T$_(volatile size_t);


#ifdef user_given_name_here_TEMPLATE_FUNCTION_DEF_SECTION
  
void template_function_T$ (T0$ a,T1$ b,T2$ c){
  T1$ tmp = b;
}

struct list_T$ list_T$_(volatile size_t cap){
  struct list_T$ result = ((struct list_T$){0,cap});
  if(cap > 20){
    printf("Hallow world %li", cap);
    result = ((struct list_T$){0,cap});
  } else if(cap < 10){
    result = ((struct list_T$){1,cap});
  } else{
    result = ((struct list_T$){2,cap});
  }
  return result;
}


#endif







#ifdef T0$
  #undef T0$
#endif

#ifdef T1$
  #undef T1$
#endif

#ifdef T2$
  #undef T2$
#endif

#ifdef template_function_T$
  #undef template_function_T$
#endif

#ifdef list_T$_
  #undef list_T$_
#endif

#ifdef user_given_name_here_AUXILARY_FUNCTION_DEFINITION_SECTION
  #undef user_given_name_here_AUXILARY_FUNCTION_DEFINITION_SECTION
#endif

#ifdef user_given_name_here_TEMPLATE_FUNCTION_DEFINITION_SECTION
  #undef user_given_name_here_TEMPLATE_FUNCTION_DEFINITION_SECTION
#endif


int main(){
}
