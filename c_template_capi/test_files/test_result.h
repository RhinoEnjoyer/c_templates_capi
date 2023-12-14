
// common.h
//////////////////////////////////
#define CONCATENATE1_(prefix, T0) prefix##_##T0
#define CONCATENATE2_(prefix, T0, T1) prefix##_##T0##_##T1
#define CONCATENATE3_(prefix, T0, T1, T2) prefix##_##T0##_##T1##_##T2
#define CONCATENATE4_(prefix, T0, T1, T2, T3) prefix##_##T0##_##T1##_##T2##_##T3
#define CONCATENATE5_(prefix, T0, T1, T2, T3, T4) prefix##_##T0##_##T1##_##T2##_##T3##_##T4
#define CONCATENATE6_(prefix, T0, T1, T2, T3, T4, T5) prefix##_##T0##_##T1##_##T2##_##T3##_##T4##_##T5
#define CONCATENATE7_(prefix, T0, T1, T2, T3, T4, T5, T6) prefix##_##T0##_##T1##_##T2##_##T3##_##T4##_##T5##_##T6
#define CONCATENATE8_(prefix, T0, T1, T2, T3, T4, T5, T6, T7) prefix##_##T0##_##T1##_##T2##_##T3##_##T4##_##T5##_##T6##_##T7
#define CONCATENATE9_(prefix, T0, T1, T2, T3, T4, T5, T6, T7, T8) prefix##_##T0##_##T1##_##T2##_##T3##_##T4##_##T5##_##T6##_##T7##_##T8
#define CONCATENATE10_(prefix, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) prefix##_##T0##_##T1##_##T2##_##T3##_##T4##_##T5##_##T6##_##T7##_##T8##_##T9
#define CONCATENATE1(prefix, T0) CONCATENATE1_(prefix, T0)
#define CONCATENATE2(prefix, T0, T1) CONCATENATE2_(prefix, T0, T1)
#define CONCATENATE3(prefix, T0, T1, T2) CONCATENATE3_(prefix, T0, T1, T2)
#define CONCATENATE4(prefix, T0, T1, T2, T3) CONCATENATE4_(prefix, T0, T1, T2, T3)
#define CONCATENATE5(prefix, T0, T1, T2, T3, T4) CONCATENATE5_(prefix, T0, T1, T2, T3, T4)
#define CONCATENATE6(prefix, T0, T1, T2, T3, T4, T5) CONCATENATE6_(prefix, T0, T1, T2, T3, T4, T5)
#define CONCATENATE7(prefix, T0, T1, T2, T3, T4, T5, T6) CONCATENATE7_(prefix, T0, T1, T2, T3, T4, T5, T6)
#define CONCATENATE8(prefix, T0, T1, T2, T3, T4, T5, T6, T7) CONCATENATE8_(prefix, T0, T1, T2, T3, T4, T5, T6, T7)
#define CONCATENATE9(prefix, T0, T1, T2, T3, T4, T5, T6, T7, T8) CONCATENATE9_(prefix, T0, T1, T2, T3, T4, T5, T6, T7, T8)
#define CONCATENATE10(prefix, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) CONCATENATE10_(prefix, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)



#define F_(pre, sub) pre##_##sub 
#define F(pre, sub) F_(pre, sub)

//////////////////////////////////

//Error chekcing
#ifndef T0$
  #define T0$ int
#endif
#ifndef T1$
  #define T1$ float
#endif
#ifndef T2$
  #define T2$ uint
#endif

//Auxilary structs
#ifndef AUXILARY_STRUCTS
#define AUXILARY_STRUCTS

  struct auxilary_A${
    int a;
    int b;
  };
  typedef struct auxilary_A$ auxilary;
#endif

#ifndef AUXILARY_FUNCTIONS
#define AUXILARY_FUNCTIONS
  
#endif 


// Template Structs

// Structual template types definitions
#define template_union_T$ CONCATENATE3(template_union, T0$, T1$, T2$)
#define template_union(T0,T1,T2) template_union_##T0##_##T1##_##T2

// Structual definitions
union template_union_T$ {
  int a;
  T0$ C;
};
typedef union template_union_T$ template_union_T$;


#define template_union_T$_push F(template_union_T$,push)
//Template Functions
void template_union_T$_push(void){
}


#undef template_union_T$
#undef AUXILARY_STRUCTS
