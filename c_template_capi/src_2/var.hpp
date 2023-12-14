#pragma once
#include "ttype_regex.hpp"

#include <clang-c/Index.h>
#include <string>
#include <optional>
#include <iostream>
#include <set>

enum entity_type{
  STRUCT,
  UNION,
  ENUM,
  FUNCTION,
};

enum array_type{
  CARRAY,
  VARRAY,
  IARRAY,
};

struct type_bools{
  bool is_ptr = false;
  bool is_block = false;
  bool is_fptr = false;
  bool is_fnproto = false;

  bool is_typedef = false;
  array_type at;
  
  bool is_builtin = false;
  bool is_complex = false;
  bool is_vector = false;
  bool is_auto = false;

  bool is_atomic = false;

  bool is_elaborated = false;

  entity_type et;
  bool is_unexposed = false;
};

void type_bools_analysis_rec(CXType ctype, type_bools &v, bool has_val = true) {
  if(!has_val) return;

  std::optional<CXType> rctype;

  if(ctype.kind == CXType_Unexposed) v.is_unexposed = true;
  else if (ctype.kind == CXType_BlockPointer) v.is_block = true;
  else if (ctype.kind == CXType_FunctionProto) {
    v.is_ptr = false;
    v.is_fptr = true;
  }
  else if (ctype.kind == CXType_Pointer){
    v.is_ptr = true;
    rctype = clang_getPointeeType(ctype);
  }
  else if (ctype.kind == CXType_ConstantArray || 
           ctype.kind == CXType_VariableArray ||
           ctype.kind == CXType_IncompleteArray){
    if (ctype.kind == CXType_ConstantArray) v.at = array_type::CARRAY; 
    else if (ctype.kind == CXType_VariableArray) v.at= array_type::VARRAY;
    else if (ctype.kind == CXType_IncompleteArray) v.at = array_type::IARRAY;
    rctype = clang_getArrayElementType(ctype);
  } 
  else if (ctype.kind == CXType_Enum) v.et= entity_type::ENUM; 
  else if (ctype.kind == CXType_Record) v.et = entity_type::STRUCT;
  else if(ctype.kind == CXType_Complex) v.is_complex = true;
  else if(ctype.kind == CXType_FunctionNoProto) v.is_fnproto = true;
  else if(ctype.kind == CXType_Vector) v.is_vector = true;
  else if(ctype.kind == CXType_Elaborated){
    v.is_elaborated = true;
    rctype = clang_Type_getNamedType(ctype);
  } 
  else if(ctype.kind == CXType_Typedef){
    v.is_typedef = true;
    rctype = clang_getCanonicalType(ctype);
  } 
  else if(ctype.kind >= CXType_FirstBuiltin && ctype.kind <= CXType_LastBuiltin) v.is_builtin = true; 
  else if(ctype.kind == CXType_Atomic) v.is_atomic = true;
  

  has_val = rctype.has_value();
  type_bools_analysis_rec(rctype.value_or(CXType()), v, has_val);
}

inline
type_bools type_bools_analysis(CXType ctype){
  type_bools vb;
  type_bools_analysis_rec(ctype, vb); 
  return vb;
}

void type_bools_print(const type_bools& vb){
  if(vb.is_complex) std::cout << "------is_complex"       << "\n";
  if(vb.is_ptr) std::cout << "------is_ptr"               << "\n";
  if(vb.is_block) std::cout << "------is_block"           << "\n";
  if(vb.is_fptr) std::cout << "------is_fptr"             << "\n";
  if(vb.is_fnproto) std::cout << "------is_fnproto"       << "\n";
  if(vb.is_typedef) std::cout << "------is_typedef"       << "\n";
  if(vb.at == array_type::CARRAY) std::cout << "------is_carray"         << "\n";
  if(vb.at == array_type::VARRAY) std::cout << "------is_varray"         << "\n";
  if(vb.at == array_type::IARRAY) std::cout << "------is_iarray"         << "\n";
  if(vb.is_vector) std::cout << "------is_vector"         << "\n";
  if(vb.is_auto) std::cout << "------is_auto"             << "\n";
  if(vb.is_elaborated) std::cout << "------is_elaborated" << "\n";
  if(vb.is_builtin) std::cout << "------is_builtin"       << "\n";
  if(vb.et == entity_type::STRUCT) std::cout << "------is_struct"         << "\n";
  if(vb.et == entity_type::UNION) std::cout << "------is_union"           << "\n";
  if(vb.et == entity_type::ENUM) std::cout << "------is_enum"             << "\n";
  if(vb.is_atomic) std::cout << "------is_atomic"         << "\n";
}


struct type_data{
  std::string name;
  template_type ttype = NOT_TEMPLATE;
  type_bools bools;
  std::set<std::string> nested_types;
};

type_data type_data_(std::string type, template_type ttype, type_bools tbools, std::set<std::string> nt){
  return {type, ttype, tbools, nt};
}

struct var_data{
  type_data type;
  std::string name;
};

var_data var_data_(const type_data& type, std::string name){
  return {type, name};
}


void var_data_bools_print(const var_data& v){
  ctem_bools_print(v.type.ttype);
  type_bools_print(v.type.bools); 
}


