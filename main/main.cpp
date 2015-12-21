#include "elf-parser.h"
#include "disassembler.h"
#include "module.h"
#include "pin-profile.h"

int main(int argc, const char **argv)
{
    ElfParser::init();
    ElfParser::parse_elf(argv[1]);
    Disassembler::init();
    Disassembler::disassemble_all_modules();
    Module::split_all_modules_into_bbls();
    PinProfile *profile = new PinProfile(argv[2]);
    profile->check_bbl_safe();
    Module::dump_all_indirect_jump_result();
    
    return 0;
}