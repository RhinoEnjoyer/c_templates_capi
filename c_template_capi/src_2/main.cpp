#include "ast_visitor.hpp"
#include <filesystem>


int main(int argc, char** argv){

  const char* const filename = "./src/test2.c";
  const char* const args[] = {"-xc","-std=c2x"};

  // Info print
  {
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string cpp_args;
    for(uint64_t i = 1; i < argc; i++)
      cpp_args = std::string(argv[i]) + " ";

    std::cout << "Working directory: \"" << cwd.string()  << "\"" << 
    "\n----" << "arguments: \"" << cpp_args << "\"" << "\n\n";


    std::string clang_args;
    for(uint64_t i = 0; i < sizeof args/sizeof *args; i++)
      clang_args += std::string(args[i]) + " "; 
    
    std::cout << "Analysing file: \"" << filename << "\"" <<
    "\n----" << "clang arguments: \"" << clang_args << "\"" <<
    "\n\n" << std::endl;
  }

  // Init
  CXTranslationUnit translation_unit;
  CXIndex index;  
  {
    index = clang_createIndex(1, 0);
    translation_unit = clang_parseTranslationUnit(
      index,
      filename,
      args,
      sizeof args / sizeof *args,
      nullptr,
      0,
      CXTranslationUnit_SingleFileParse | CXTranslationUnit_LimitSkipFunctionBodiesToPreamble
    );

    if(!translation_unit){
      fprintf(stderr, "(error) \"%s\"  Failed to parse the translation unit.\n",filename);
      return 1;
    }
  }

  // Analysis
  state data;
  {
    CXCursor cursor = clang_getTranslationUnitCursor(translation_unit);
    clang_visitChildren(cursor, visit_cursor, &data);
  }

  // Processing
  std::vector<record_data> tinputs;

  std::vector<record_data> rtentities;
  std::vector<record_data> rtauxiliaries;

  std::vector<function_data> ftentities;
  std::vector<function_data> ftauxiliaries;
  {
    putchar('\n'); 
    for(const auto& ro: data.record_order){
      const record_data& r = data.records[ro];

      if(r.info.ttype == TEMPLATE_INPUT) tinputs.push_back(r);
      else if(r.info.ttype == TEMPLATE_ENTITY) rtentities.push_back(r);
      else if(r.info.ttype == TEMPLATE_AUXILIARY) rtauxiliaries.push_back(r);
           
 
      std::cout << "Record: "  << r.info.name << std::endl;
      record_data_bools_print(r); 
      for(const auto& f: r.fields){
        std::cout << "Field: " << f.type.name<< " " << f.name << std::endl;
        var_data_bools_print(f); 
      }
      putchar('\n');
    }

    for (const auto& ti: tinputs){
      std::cout << "Template input: " << ti.info.name << std::endl;
    }
    for (const auto& rte: rtentities){
      std::cout << "Record Template Entity: " << rte.info.name << std::endl;
    }
    for (const auto& rta: rtauxiliaries){
      std::cout << "Record Template Auxiliary: " << rta.info.name << std::endl;
    }

    for (const auto& fte: ftentities){
      std::cout << "Function Template Entity: " << fte.name << std::endl;
    }
    for (const auto& fta: ftauxiliaries){
      std::cout << "Function Template Auxiliary: " << fta.name << std::endl;
    }


  }

  // Output
  {

  }

  // Clean up
  clang_disposeTranslationUnit(translation_unit);
  clang_disposeIndex(index);

  return 0;
}
