#define TI_TYPEDEF(name) typedef struct{int a;} name##_ti;
TI_TYPEDEF(a)
TI_TYPEDEF(b)

typedef struct {
  a_ti my_field;
  int cint;
  a_ti* (^block_func)(b_ti);
} my_entity_te;


typedef struct {
  int b;
} my_aux_ta;

my_entity_te my_func_te(a_ti i){
  a_ti a = {0};
  return (my_entity_te){};
}
