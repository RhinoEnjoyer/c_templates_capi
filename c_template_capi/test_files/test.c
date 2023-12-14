#include <malloc.h>
#include <stdio.h>
#include "./template_utils.h"
//A$ means that it is a auxilary type that
//is to be copied as is
//
//T$ means that it is a template
//T<number>$ is a Template type

TEMPLATE_TYPEDEF(0)
TEMPLATE_TYPEDEF(1)
TEMPLATE_TYPEDEF(2)


struct list_T${
  unsigned long size;
  unsigned long capacity;
  T0$ ptr[];
};


struct aux_A${
  unsigned int i;
  unsigned int c;
  float b;
};

struct list_T$ list_T$_(unsigned long* cap){
  struct list_T$ result;
  result = (struct list_T$){2,2};
  return result;
}


