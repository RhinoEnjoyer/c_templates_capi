#pragma once

#include <string>
#include <map>
#include <set>
#include <optional>

#include <clang-c/Index.h>

#include "ttype_regex.hpp"

struct type_bools{
  bool is_ptr = false;
  bool is_block = false;
  bool is_fptr = false;
  bool is_fnproto = false;

  bool is_typedef = false;

  bool is_carray = false;
  bool is_varray = false;
  bool is_iarray = false;
  
  bool is_builtin = false;
  bool is_complex = false;
  bool is_vector = false;
  bool is_auto = false;

  bool is_atomic = false;

  bool is_elaborated = false;
  bool is_record = false;
  bool is_struct = false;
  bool is_union = false;

  bool is_enum = false;
  bool is_unexposed = false;
};

void type_bools_analysis_rec(CXType ctype, type_bools &v, bool has_val = true) {
  if(!has_val) return;

  std::optional<CXType> rctype;

  if(ctype.kind == CXType_Unexposed) {
    v.is_unexposed = true;
  } else if (ctype.kind == CXType_BlockPointer) {
    v.is_ptr = false;
    v.is_fptr = false;
    v.is_block = true;
  } else if (ctype.kind == CXType_FunctionProto) {
    v.is_ptr = false;
    v.is_fptr = true;
  } else if (ctype.kind == CXType_Pointer) {
    v.is_ptr = true;
    rctype = clang_getPointeeType(ctype);
  } else if (ctype.kind == CXType_ConstantArray || 
             ctype.kind == CXType_VariableArray ||
             ctype.kind == CXType_IncompleteArray) {
    if (ctype.kind == CXType_ConstantArray) {
      v.is_carray = true;
    } else if (ctype.kind == CXType_VariableArray) {
      v.is_varray = true;
    } else if (ctype.kind == CXType_IncompleteArray) {
      v.is_iarray = true;
    }
    rctype = clang_getArrayElementType(ctype);
  } else if (ctype.kind == CXType_Enum) { 
    v.is_enum = true; 
  } else if (ctype.kind == CXType_Record) { 
    v.is_record = true;
  } else if(ctype.kind == CXType_Complex) { 
    v.is_complex = true;
  } else if(ctype.kind == CXType_FunctionNoProto) { 
    v.is_fnproto = true;
  } else if(ctype.kind == CXType_Vector) {
    v.is_vector = true;
  } else if(ctype.kind == CXType_Elaborated) {
    v.is_elaborated = true;
    rctype = clang_Type_getNamedType(ctype);
  } else if(ctype.kind == CXType_Typedef) {
    v.is_typedef = true;
    rctype = clang_getCanonicalType(ctype);
  } else if(ctype.kind >= CXType_FirstBuiltin && ctype.kind <= CXType_LastBuiltin){
    v.is_builtin = true; 
  } else if(ctype.kind == CXType_Atomic) {
    v.is_atomic = true;
  }

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
  if(vb.is_carray) std::cout << "------is_carray"         << "\n";
  if(vb.is_varray) std::cout << "------is_varray"         << "\n";
  if(vb.is_iarray) std::cout << "------is_iarray"         << "\n";
  if(vb.is_vector) std::cout << "------is_vector"         << "\n";
  if(vb.is_auto) std::cout << "------is_auto"             << "\n";
  if(vb.is_elaborated) std::cout << "------is_elaborated" << "\n";
  if(vb.is_builtin) std::cout << "------is_builtin"       << "\n";
  if(vb.is_record) std::cout << "------is_record"         << "\n";
  if(vb.is_struct) std::cout << "------is_struct"         << "\n";
  if(vb.is_union) std::cout << "------is_union"           << "\n";
  if(vb.is_enum) std::cout << "------is_enum"             << "\n";
  if(vb.is_atomic) std::cout << "------is_atomic"         << "\n";
}

struct type_data{
  std::string type;
  template_type tt;
  type_bools bools;
  std::set<std::string> nested_template_types;
};


struct source_range{
  unsigned  start = -1;
  unsigned  end = -1;

  source_range(){}

  source_range(CXCursor cursor){
    CXSourceRange srange = clang_getCursorExtent(cursor); 
    CXSourceLocation cstart = clang_getRangeStart(srange);
    CXSourceLocation cend = clang_getRangeEnd(srange);

    clang_getFileLocation(cstart, NULL, NULL,NULL, &this->start);
    clang_getFileLocation(cend, NULL, NULL, NULL, &this->end);
  }
};

struct var_data{
  type_data var_type_data;
  std::string name;
};

struct record_data{
  type_data rec_type_data;
  std::vector<var_data> fields;

  source_range loc;
};


struct function_data{
  type_data ret_type_data;
  std::string name;

  std::vector<var_data> args;

  source_range loc;
};




class state{
public:
  std::map<std::string, record_data> records;
  std::vector<std::string> record_order;

  std::map<std::string, function_data> functions;
  std::vector<std::string> function_order;

  void insert_record(type_data rec_type_data,record_data rec){
  }

  void insert_function(function_data func){
  }
};

