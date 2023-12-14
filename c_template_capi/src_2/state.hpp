#pragma once

#include <string>
#include <map>
#include <set>
#include <optional>

#include <clang-c/Index.h>

#include "ttype_regex.hpp"
#include "var.hpp"


struct record_data{
  type_data info;
  std::vector<var_data> fields;

  // replace this with a range
  std::string body;
};

record_data record_data_(
  type_data ri,
  std::vector<var_data>& f,
  std::string b
){
  record_data r;
  r.info   = ri;
  r.fields = f;
  r.body   = b;
  return r;
}

void record_data_bools_print(const record_data& r){
  ctem_bools_print(r.info.ttype);
}




struct function_data{
  type_data rtype;
  std::string name;
  std::vector<var_data> args;

  // change the name maybe
  std::vector<var_data> body_decls;


  // replace this with a range
  // find a way to get the body of a function
  std::string body;
};




class state{
public:
  std::map<std::string, record_data> records;
  std::vector<std::string> record_order;

  std::map<std::string, function_data> functions;
  std::vector<std::string> function_order;

  void insert_record(record_data rec){
    records[rec.info.name] = rec;
    record_order.push_back(rec.info.name);
  }

  void insert_function(function_data func){
    functions[func.name] = func;
    function_order.push_back(func.name);
  }


  std::set<std::string> get_nested_types(const std::vector<type_data> td){
    std::set<std::string> set;
    for(const auto& t: td){
      if(t.ttype == TEMPLATE_INPUT) {
        set.insert(t.name);
      } else if(t.ttype == TEMPLATE_ENTITY) {
        if(records.find(t.name) != records.end()) {
          set.insert(t.nested_types.begin(), t.nested_types.end());
        } else {
          std::cerr << "(error)" << "type: " << t.name << " is tempalte entity but not found in the record map" << std::endl;
        }
      }
    }

    return set;
  }
  std::set<std::string> get_nested_types(const std::vector<var_data> vd){
    std::vector<type_data> td;
    td.resize(vd.size());
    for(const auto& v: vd){
      td.push_back(v.type);
    }

    return get_nested_types(td);
  }
};

