#include "ast_visitor.hpp"
#include <filesystem>

#include "utils.hpp"


std::string substr(std::string& file, unsigned long long start, unsigned long long end){
  return file.substr(start, end - start);
}

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
  ast_visitor visitor = ast_visitor(data);
  visitor.run(translation_unit);

  std::string file = read_file_into_string(filename);

  // Analysis 2
  std::vector<const record_data*> tinputs;
  std::vector<const record_data*> rtentities;
  std::vector<const record_data*> rtauxiliaries;

  std::vector<const function_data*> ftentities;
  std::vector<const function_data*> ftauxiliaries;
  {
    for(const auto& ro: data.record_order){
      const auto& r = data.records[ro];
      if(r.rec_type_data.tt == TEMPLATE_INPUT) tinputs.push_back(&r);
      else if(r.rec_type_data.tt == TEMPLATE_ENTITY) rtentities.push_back(&r);
      else if(r.rec_type_data.tt == TEMPLATE_AUXILIARY) rtauxiliaries.push_back(&r);
    }

    for(const auto& fo: data.function_order){
      const auto& f = data.functions[fo];
      if(f.ret_type_data.tt == TEMPLATE_ENTITY) ftentities.push_back(&f);
      else if(f.ret_type_data.tt == TEMPLATE_AUXILIARY) ftentities.push_back(&f);
    }

    {
      putchar('\n');
      std::cout << "Template inputs: " << std::endl;
      for(const auto& i: tinputs){
        std::cout << "------------" << i->rec_type_data.type << std::endl;
      }

      putchar('\n');
      std::cout << "Template recrod entities: " << std::endl;
      for(const auto& i: rtentities){
        std::cout << "------------" << i->rec_type_data.type << std::endl;
        std::cout << substr(file,i->loc.start, i->loc.end) << std::endl;
      }

      putchar('\n');
      std::cout << "Template record auxiliaries: " << std::endl;
      for(const auto& i: rtauxiliaries){
        std::cout << "------------" << i->rec_type_data.type << std::endl;
        std::cout << substr(file,i->loc.start, i->loc.end) << std::endl;
      }
      putchar('\n');
      std::cout << "Template function entities: " << std::endl;
      for(const auto& i: ftentities){
        std::cout << "------------" << i->ret_type_data.type << std::endl;
        std::cout << substr(file,i->loc.start, i->loc.end) << std::endl;
      }
      putchar('\n');
      std::cout << "Template function auxiliaries: " << std::endl;
      for(const auto& i: ftauxiliaries){
        std::cout << "------------" << i->ret_type_data.type << std::endl;
        std::cout << substr(file,i->loc.start, i->loc.end) << std::endl;
      }
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
