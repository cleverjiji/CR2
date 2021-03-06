#pragma once
#include <string>
#include "type.h"
#include "utility.h"

class Options{
public:
	static BOOL _static_analysis;
	static BOOL _dynamic_shuffle;
	static BOOL _need_check_static_analysis;
	static BOOL _has_elf_path;
	static BOOL _has_input_db_file;
	static BOOL _has_output_db_file;
	static BOOL _need_randomize_rbbl;
	static BOOL _need_randomize_rbbu;
	static INT64 _rbbu_range;
	static INT64 _rbbu_padding;
	static std::string _check_file;
	static std::string _elf_path;
	static std::string _input_db_file_path;
	static std::string _output_db_file_path;
	static void check(char *cr2);
	static void parse(int argc, char** argv);
	static void show_system();
	static void print_usage(char *cr2);
};

