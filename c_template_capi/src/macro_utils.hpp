#pragma once

#include "./utils.hpp"
#include <string>
#include <vector>

const std::string str_error(std::string message){
  return "  #error \"" + message +  "\"\n"; 
}

const std::string str_warn(std::string message){
  return "  #warning \"" + message +  "\"\n"; 
}

const std::string str_ifndef(const std::string def){
  return "\n#ifndef " + def + "\n";
}

const std::string str_ifdef(const std::string def){
  return "\n#ifdef " + def + "\n";
}

const std::string str_endif(){
  return "#endif\n";
}

const std::string str_ifndef_error(const std::string def,const std::string message) {
  return str_ifndef(def) + str_error(message); 
}

const std::string str_ifndef_warn(const std::string def,const std::string message) {
  return str_ifndef(def) + str_warn(message);
}

const std::string str_ifdef_error(const std::string def,const std::string message){
  return str_ifdef(def) + str_error(message); 
}

const std::string str_ifdef_warn(const std::string def,const std::string message){
  return str_ifdef(def) + str_warn(message);
}

const std::string str_typedef(const std::string name,const std::string tdef_name) {
  return "typedef " + name + " " + tdef_name + ";";
}

const std::string str_include(const std::string file) {
  return "#include \"" + file + "\"";
}

const std::string str_concat_macro(std::string prefix, std::vector<std::string> types) {
  auto r = std::string("CONCATENATE" + std::to_string(types.size()) + "(" + prefix + ",");
  const std::string args = condence_string_with_pattern(types, "", "", ",");
  r += args + ")";
  return r;
}

const std::string str_record_begin(std::string name, bool is_union) {
  return ((is_union) ? "union " : "struct ") + name + "{\n";
}

const std::string str_def(std::string def_name){
  return "#define " + def_name + " ";
}

const std::string str_undef(std::string def_name){
  return "#undef " + def_name + "\n";
}

const std::string str_record_end(){ 
  return "};"; 
}

const std::string str_comment(std::string comment) {
  return "/* " + comment + " */\n";
}

const std::string str_func_macro(std::string name, std::vector<std::string>& args) {
  return str_def(name) + "(" + condence_string_with_pattern(args, "", "", ",") + ")";
}

const std::string str_remove_pointers(std::string type){
  return str_erase_all(type,"*");
}

const std::string str_remove_brackets(std::string type){
  // make this a function
  type = str_erase_all(type,"[");
  type = str_erase_all(type,"]");
  return type;
}

const std::string str_remove_const(std::string type){
  return str_erase(type, "const");
}

const std::string str_remove_struct(std::string type){
  return str_erase(type, "struct");
}

const std::string str_fptr_name(bool is_block, std::string name, std::string arguments){
  std::string c = (is_block)? "^" : "*";
  return "(" + std::string(c) + name  + ")" + "(" + arguments + ")"; 
}
