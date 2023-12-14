#pragma once
#include <optional>

#include "cclang_utils.hpp"
#include "state.hpp"
#include "ttype_regex.hpp"

#define child_visitor_(kind)                                               \
  CXChildVisitResult visit_##kind##_children(CXCursor cursor, CXCursor parent, \
                                             void* data_void)

#define decl_visit_branch(Kind, function)                                      \
  if (cursor.kind == Kind)                                                     \
  function
#define stmt_visit_branch(Kind, function)                                      \
  if (cursor.kind == Kind)                                                     \
  function

bool is_typedef(CXCursor cursor) {
  CXCursorKind kind = clang_getCursorKind(cursor);
  return kind == CXCursor_TypedefDecl;
}

CXType func_get_rtype(CXType ctype){
  if(ctype.kind == CXType_FunctionProto){
    // returns no matter what the result type of the function is
    return clang_getResultType(ctype); 
  }

  return CXType{};
}

CXType ptr_type(CXType ctype, bool has_val = true){
  if(!has_val) return(ctype);

  std::optional<CXType> rctype;
  if (ctype.kind == CXType_BlockPointer || ctype.kind == CXType_Pointer) {
    // block pointers always degrade to function pointers
    // and normal pointers will just give a type
    rctype = clang_getPointeeType(ctype);
  } else if(ctype.kind == CXType_FunctionProto){
    // returns no matter what the result type of the function is
    return clang_getResultType(ctype); 
  }

  has_val = rctype.has_value();
  return ptr_type(rctype.value_or(ctype),  has_val);
}

// recursion will not happen for blocks and function pointers
// because if we do we get info about the return type not about the function it self
// but it will for pointers

[[gnu::always_inline]] inline
bool clang_is_ptr_type(CXType c){
  if(c.kind == CXType_Pointer) return true;
  else if(c.kind == CXType_BlockPointer) return true;
  return false;
}

bool is_func_def(CXCursor cursor) {
    CXCursor definition = clang_getCursorDefinition(cursor);
    return !clang_Cursor_isNull(definition);
}

bool is_func_decl(CXCursor cursor) {
    return clang_getCursorKind(cursor) == CXCursor_FunctionDecl &&
           clang_Cursor_isNull(clang_getCursorDefinition(cursor));
}

// sucks dick
// have to use a formater to fix the output from this one since we can not get the orginal text
// replace it with just looking through the file
// and getting what we want
// for now tho this will do 
std::string GetSourceTextFromRange(CXTranslationUnit tu, CXSourceRange range) {
    CXToken* tokens;
    unsigned int numTokens;
    clang_tokenize(tu, range, &tokens, &numTokens);

    std::string sourceText;
    for (unsigned int i = 0; i < numTokens; ++i) {
        CXString tokenString = clang_getTokenSpelling(tu, tokens[i]);
        sourceText += std::string(clang_getCString(tokenString)) + " ";
        clang_disposeString(tokenString);
    }

    clang_disposeTokens(tu, tokens, numTokens);
    return sourceText;
}

// so does this by extension
std::string GetFunctionBody(CXCursor cursor) {
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);

    return GetSourceTextFromRange(tu, range);
}

typedef CXChildVisitResult (*child_visit_fptr)(CXCursor, CXCursor,CXClientData);

CXChildVisitResult visit_cursor(CXCursor cursor, CXCursor parent, CXClientData data_void);
CXChildVisitResult visit_record_children(CXCursor cursor, CXCursor parent, void* data_void);

class ast_visitor{
  public:
  state& st;

  // "funny" side effect of working with a C API via a C++ class
  // DO NOT FORGET TO MAKE IT NULL AGAIN
  void* client_data = nullptr;

  ast_visitor(state& s): st{s}{} 

  
  std::set<std::string> visit_func_params_types(CXType fctype){
    std::set<std::string> nested_types;
    const CXType ftype = (clang_is_ptr_type(fctype))? clang_getPointeeType(fctype) : fctype;

    // arg info
    int arg_count = clang_getNumArgTypes(ftype);
    if (arg_count == -1) return nested_types;

    for (int i = 0; i < arg_count; ++i) {
      CXType actype = clang_getArgType(ftype, i);
      std::string aname = clang_get_type_str(actype);
      type_bools bools = type_bools_analysis(actype);
      template_type tt = is_ttype(aname);
      const auto a_nested_types = visit_type(actype, aname, tt, bools);

      nested_types.insert(a_nested_types.begin(), a_nested_types.end());
    }

    return nested_types;
  }
  
  std::set<std::string> visit_type(CXType ctype, std::string name, template_type tt, type_bools bools){
    std::set<std::string> nested_types;

    if(bools.is_builtin) {
      return nested_types;
    } else if(bools.is_ptr || bools.is_fptr || bools.is_block) {
      CXType ret_ctype = ptr_type(ctype);
      std::string ret_name = clang_get_type_str(ret_ctype);
      type_bools ret_bools = type_bools_analysis(ret_ctype);
      template_type ret_tt = is_ttype(ret_name);
      const auto ret_nested_types = visit_type(ret_ctype, ret_name, ret_tt, ret_bools);

      nested_types.insert(ret_nested_types.begin(), ret_nested_types.end());

      // do the arguments for the function ptrs and blocks here
      if(bools.is_fptr || bools.is_block) {
        const auto fparms_nested_types = visit_func_params_types(ctype);
        nested_types.insert(fparms_nested_types.begin(), fparms_nested_types.end());
      }
    } else if(tt == TEMPLATE_INPUT) {
      nested_types.insert(name);
    } else if(tt == TEMPLATE_ENTITY) {
      if(st.records.find(name) != st.records.end()){
        const auto& nt = st.records[name].rec_type_data.nested_template_types;
        nested_types.insert(nt.begin(), nt.end());
      } else { 
        std::cerr << "(error)" << "Template enity: " << name  << " was not found on recrods" << "\n";
      }
    }

    return nested_types;
  }
  
  child_visitor_(record_internal) {
    CXType cctype = clang_getCursorType(cursor);

    const std::string type = clang_get_type_str(cursor);
    const template_type tt = is_ttype(type);
    const type_bools bools = type_bools_analysis(cctype);
    const std::set<std::string> nested_types = visit_type(cctype, type, tt, bools);
      
    const std::string name = clang_get_name_str(cursor);

    var_data vd = {{type, tt, bools, nested_types}, name};

    std::vector<var_data>* const parent_fields = (std::vector<var_data>*)data_void;
    parent_fields->push_back(vd);

    return CXChildVisit_Continue;
  }

  void analyse_record(CXCursor cursor, const bool is_union) {
    const std::string cname = clang_get_name_str(cursor);
    if(st.records.find(cname) != st.records.end()) return;

    std::string name = cname;

    template_type tt = is_ttype(cname);
    type_bools bools = {
      .is_record = true, 
      .is_struct = (bool)(1 - is_union), 
      .is_union = is_union
    };

    std::vector<var_data> fields;

    //VERY IMPORTANT
    client_data = &fields;
    clang_visitChildren(cursor, visit_record_children, this);
    client_data = nullptr;
    //VERY IMPORTANT

    std::set<std::string> nested_types;
    
    for(const auto& f: fields){
      nested_types.insert(
        f.var_type_data.nested_template_types.begin(), 
        f.var_type_data.nested_template_types.end()
      );
    }
    std::cout << "Record: " << name << "\n";
    const auto range = source_range(cursor);
    
    const record_data rd = {{name, tt,bools,nested_types}, fields, range};
    st.records[name] = rd;
    st.record_order.push_back(name);
  }
  
  void analyse_struct(CXCursor cursor) {analyse_record(cursor, false);}
  void analyse_union(CXCursor cursor)  {analyse_record(cursor, true);}
  
  void analyse_enum(CXCursor cursor){
    std::cout << __PRETTY_FUNCTION__ << " is to be filled" << std::endl;
  }
  void analyse_function(CXCursor cursor){
    std::cout << "(warning)"  <<  __PRETTY_FUNCTION__ << " is to be filled" << std::endl;
  }

  CXChildVisitResult visit_cursor_internal(CXCursor cursor, CXCursor parent, CXClientData data_void) {
     decl_visit_branch(CXCursor_StructDecl, analyse_struct(cursor));
    else decl_visit_branch(CXCursor_UnionDecl, analyse_union(cursor));
    else decl_visit_branch(CXCursor_FunctionDecl, analyse_function(cursor));
  
    return CXChildVisit_Recurse;
  }

  void run(CXTranslationUnit tu){
    CXCursor cursor = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(cursor, visit_cursor, this);
  }
};



CXChildVisitResult visit_cursor(CXCursor cursor, CXCursor parent, void* data_void) {
  ast_visitor* v = (ast_visitor*)data_void;
  return v->visit_cursor_internal(cursor,parent, v->client_data);
}


CXChildVisitResult visit_record_children(CXCursor cursor, CXCursor parent, void* data_void){
  ast_visitor* v  = (ast_visitor*)data_void;

  if(v->client_data == nullptr) std::cerr << "(error)" << "[" << __PRETTY_FUNCTION__  << "]" << "client data was null" << "\n"; 

  return v->visit_record_internal_children(cursor, parent, v->client_data) ;
}
