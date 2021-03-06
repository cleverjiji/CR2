#include <unistd.h>
#include<stdlib.h>
#include "option.h"

BOOL  Options::_static_analysis = false;
BOOL  Options::_dynamic_shuffle = false;
BOOL  Options::_need_check_static_analysis = false;
BOOL  Options::_has_elf_path = false;
BOOL  Options::_has_input_db_file = false;
BOOL  Options::_has_output_db_file = false;
BOOL  Options::_need_randomize_rbbl = false;
BOOL  Options::_need_randomize_rbbu = false;
INT64 Options::_rbbu_range = 1;
INT64 Options::_rbbu_padding = 0;

std::string Options::_check_file;
std::string Options::_elf_path;
std::string Options::_input_db_file_path;
std::string Options::_output_db_file_path;

void Options::show_system()
{
    const char* lines[] = {
    "    ________    ________      _______      ", 
    "   |\\   ____\\  |\\   __  \\    /  ___  \\     ",
    "   \\ \\  \\___|  \\ \\  \\|\\  \\  /__/|_/  /|    ",
    "    \\ \\  \\      \\ \\   _  _\\ |__|//  / /    ",
    "     \\ \\  \\___   \\ \\  \\\\  \\|    /  /_/__   ",
    "      \\ \\_______\\ \\ \\__\\\\ _\\   |\\________\\ ",
    "       \\|_______|  \\|__|\\|__|   \\|_______| ",
    "                                                    ",
    };         
            
    for (unsigned i=0; i<sizeof(lines)/sizeof(char*); ++i)
       BLUE("%s\n", lines[i]);
}

void Options::print_usage(char *cr2)
{
    PRINT("Usage: %s, Copyright@WangZhe | ICT\n", cr2);
    PRINT("Option list (alphabetical order):\n");
    PRINT(" -A                             Static Analysis and Dynamic Shuffle code.\n");
    PRINT(" -C /path/*.cr2.indirect.log    Input indirect log file to check static analysis.\n");
    PRINT(" -D                             Dynamic Shuffle (Generate the shuffle code variants).\n");
    PRINT(" -h                             Display help information.\n");
    PRINT(" -i /rela.db.path               Input the db file of relocation block.\n");
    PRINT(" -I /path/elf                   Handle elf binary file and its all dependence library.\n");
    PRINT(" -o /rela.db.path               Output relocation block to db file used for shuffle code at runtime.\n");
    PRINT(" -R                             All relocation block should be randomized in code variant!\n");
    PRINT(" -r range_num padding_num       Reorder Basic Block Unit!\n");
    PRINT(" -S                             Static Analysis (Disassemble/Recognize IndirectJump Targets/Split BBLs/Classify BBLs).\n");
    PRINT(" -v                             Display version information.\n");
}

void Options::check(char *cr2)
{
    if(_static_analysis){
        if(!_has_elf_path){
            PRINT("%s: invalid option -- when using static analysis, you should specified a binary (Forget -I)\n", cr2);
            exit(-1);
        }
    }
    if(_dynamic_shuffle){
        if(!_static_analysis){
            if(!_has_input_db_file){
                PRINT("%s: invalid option -- when using dynamic shuffle without static analysis, you should specifed a static analysis db file (Forget -i)\n", cr2);
                exit(-1);
            }
        }
        if(_need_randomize_rbbl){
            if(_need_randomize_rbbu){
                PRINT("%s: invalid option -- -R and -r cannot used simultaneously!\n", cr2);
                exit(-1);
            }
        }else{
            if(!_need_randomize_rbbu){
                PRINT("%s: invalid option -- -R or -r should be set at least!\n", cr2);
                exit(-1);
            }
        }
        
    }
}

inline INT64 convert_str_to_num(const char* str, char** stopstring)
{
    INT64 offset = strtol(str, stopstring, 0);
    return offset;
}

void Options::parse(int argc, char** argv)
{
    //1. process cr2 options
    const char *opt_string = "AC:Dhi:I:o:Rr::Sv";
    INT32 ret;
    while((ret = getopt(argc, argv, opt_string))!=-1){
        switch (ret){
            case 'A':
                _static_analysis = true;
                _dynamic_shuffle = true;
                break;
            case 'C':
                _need_check_static_analysis = true;
                _check_file = std::string(optarg);
                break;
            case 'D':
                _dynamic_shuffle = true;
                break;
            case 'h' :
                show_system();
                print_usage(argv[0]);
                exit(0);
                break;
            case 'i':
                _has_input_db_file = true;
                _input_db_file_path = std::string(optarg);
                break;
            case 'I':
                _has_elf_path = true;
                _elf_path = std::string(optarg);
                break;
            case 'o':
                _has_output_db_file = true;
                _output_db_file_path = std::string(optarg);
                break;
            case 'R':
                _need_randomize_rbbl = true;
                break;
            case 'r':
                _rbbu_range = convert_str_to_num(argv[optind++], NULL);
                _rbbu_padding = convert_str_to_num(argv[optind++], NULL);
                _need_randomize_rbbu = true;
                break;
            case 'S':
                _static_analysis = true;
                break;
            case 'v' :
                show_system();
                exit(0);
                break;
            default:
                print_usage(argv[0]);
                exit(-1);
        }
    }
    check(argv[0]);
    return ;
}
