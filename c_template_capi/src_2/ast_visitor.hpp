#pragma once
#include <optional>

#include "cclang_utils.hpp"
#include "state.hpp"
#include "ttype_regex.hpp"
#include "macro_utils.hpp"

#define child_visitor_func(kind)                                               \
  CXChildVisitResult visit_##kind##_children(CXCursor cursor, CXCursor parent, \
                                             CXClientData data_void)

#define decl_visit_branch(Kind, function)                                      \
  if (cursor.kind == Kind)                                                     \
  function
#define stmt_visit_branch(Kind, function)                                      \
  if (cursor.kind == Kind)                                                     \
  function



struct state_void{
  state* sptr;
  void* ptr;
};


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

[[gnu::always_inline]] inline


std::vector<var_data> analyse_function_params(CXType c,state* data){
  std::vector<var_data> args;
  const CXType ftype = (clang_is_ptr_type(c))? clang_getPointeeType(c) : c;

  // arg info
  int arg_count = clang_getNumArgTypes(ftype);
  if (arg_count == -1) return args;

  // arg processing
  for (int i = 0; i < arg_count; ++i) {
    CXType catype = clang_getArgType(ftype, i);
    std::string atype = clang_get_type_str(catype);


    std::set<std::string> nested_types;
    auto td = type_data_(atype, is_ttype(atype), type_bools_analysis(catype), nested_types);
    
    const auto vd = var_data_(td, "");
    //var_data_bools_print(vd);
    
    args.push_back(vd);
    //std::cout << "--------arg: " << atype << "\n";
  }

  return args;
}




child_visitor_func(record) {
  CXType cctype = clang_getCursorType(cursor);

  const std::string ctype = clang_get_type_str(cursor);
  const std::string cname = clang_get_name_str(cursor);

  state_void* sv = (state_void*)(data_void);

  state* data = sv->sptr;
  std::vector<var_data>* vvd = (std::vector<var_data>*)sv->ptr;
  
  
  std::cout << "----Record child: " << ctype << " " << cname << "\n";

  std::string type = ctype;
  std::string name = cname;

  template_type tt = is_ttype(ctype);
  type_bools tb = type_bools_analysis(cctype);

  std::set<std::string> nested_types;

  if(tb.is_fptr || tb.is_block){
    CXType rctype = ptr_type(cctype);
    std::cout << "return type: " << clang_get_type_str(rctype) << std::endl;

    std::vector<var_data> args;
    args = analyse_function_params(cctype, data); 
    std::string arg_str;

    type = clang_get_type_str(rctype);
    name = str_fptr_name(tb.is_block, cname, arg_str);
    tt = is_ttype(clang_get_type_str(ptr_type(rctype)));
  }

  type_data td = type_data_(type, tt, tb, nested_types);

  auto vd = var_data_(td,name);
  //var_data_bools_print(vd);

  vvd->push_back(vd);
  return CXChildVisit_Continue;
}




void analyse_record(CXCursor cursor, const bool is_union, state* const data) {
  if (is_typedef(cursor)) return;

  const std::string name = clang_get_name_str(cursor);
  if (data->records.find(name) != data->records.end()) return;

  template_type ttype = is_ttype(name);
  if (ttype == NOT_TEMPLATE) return;

  type_bools tb;
  tb.et = (is_union)? entity_type::UNION : entity_type::STRUCT;
 
  print_cursor(cursor);
  std::vector<var_data> fvd;

  // visit the children
  // need to analyse the children
  // and handle functions
  // and function pointers/blocks
  state_void sv = {data,&fvd};
  clang_visitChildren(cursor, visit_record_children, &sv);

  // analyse the children
  const auto nt = data->get_nested_types(fvd);
  for(const auto& c: nt){
    std::cout << "Template inputs for this record: " << c << std::endl; 
  }

  type_data ri = type_data_(name,ttype,tb,nt);

  record_data r = record_data_(ri, fvd, "body");
  data->insert_record(r);
}

void analyse_struct(CXCursor cursor, state *data) {
  analyse_record(cursor, false, data);
}

void analyse_union(CXCursor cursor, state *data) {
  analyse_record(cursor, true, data);
}

void analyse_enum(CXCursor cursor, state* data){
  std::cout << __PRETTY_FUNCTION__ << " is to be filled" << std::endl;
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








void analyse_function(CXCursor cursor, state *data) {
  if(is_func_decl(cursor)) return;

  std::string fname = clang_get_name_str(cursor);
  
  CXType ctype = clang_getCursorType(cursor);
  template_type ttype = is_ttype(fname);
  if(ttype == NOT_TEMPLATE) return;
  
  print_cursor(cursor);
  function_data f;

  CXType rctype = func_get_rtype(ctype);
  //std::cout << "----return type: " << clang_get_type_str(rctype) << "\n";
  //std::cout << "func type: " << clang_get_type_str(ctype) << "\n";
  f.body = GetFunctionBody(cursor);
  //ANALISE THE BODY OF THE FUNCTION


  std::vector<var_data> args = analyse_function_params(ctype);
  data->insert_function(f);
}

CXChildVisitResult visit_cursor(CXCursor cursor, CXCursor parent, CXClientData data_void) {
  state *data = (state *)(data_void);

  decl_visit_branch(CXCursor_StructDecl, analyse_struct(cursor, data));
  else decl_visit_branch(CXCursor_UnionDecl, analyse_union(cursor, data));
  else decl_visit_branch(CXCursor_FunctionDecl, analyse_function(cursor, data));

  return CXChildVisit_Recurse;
}
