#include "./template_utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
//A$ means that it is a auxilary type that
//is to be copied as is
//
//T$ means that it is a template
//T<number>$ is a Template type

TEMPLATE_TYPEDEF(0)
TEMPLATE_TYPEDEF(1)
TEMPLATE_TYPEDEF(2)


struct auxilary_A${
  int a;
  int b;
};
union template_union_T${
  int a;
  T0$ C;
};


struct template_T${
  T0$ A;
  T1$ B;
  T2$ C;
  int a;
};

struct nested_template_T${
  struct template_T$ a;
  struct template_T$ (*b)(int a, int c, void* ptr, T0$ tt);
  int* (*z)(int a, int c, void* ptr, int tt);
};


void template_function_T$(T0$ a,T1$ b,T2$ c){
  // do stuff
  T1$ tmp = b;
}

void auxilary_function_A$(size_t* a){
  *a = 20;
}




struct list_T${
  unsigned long size;
  unsigned long capacity;
  T0$ ptr[];
};


//struct list_T$* list_T$_(unsigned long cap){
//  struct list_T$ l =  (struct list_T$){
//    .size = 0, 
//    .capacity = cap,
//  };
//
//  struct list_T$* alloc = malloc(sizeof(struct list_T$) + cap*sizeof(T0$));
//  *alloc = l;
//  return alloc;  
//}

struct list_T$ list_T$_(volatile size_t cap ,void* (*alloc)(int)){
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


