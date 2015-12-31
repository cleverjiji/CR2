#include "elf-parser.h"
#include "disassembler.h"
#include "module.h"
#include "pin-profile.h"
#include "option.h"
#include "code_variant_manager.h"

int main(int argc, char **argv)
{
    Options::parse(argc, argv);

    if(Options::_static_analysis){
        // 1. parse elf
        ElfParser::init();
        ElfParser::parse_elf(Options::_elf_path.c_str());
        // 2. disassemble all modules into instructions
        Disassembler::init();
        Disassembler::disassemble_all_modules();
        // 3. split bbls
        Module::split_all_modules_into_bbls();
        // 4. classified bbls
        Module::separate_movable_bbls_from_all_modules();
        // 5. check the static analysis's result
        if(Options::_need_check_static_analysis){
            PinProfile *profile = new PinProfile(Options::_check_file.c_str());
            profile->check_bbl_safe();
            profile->check_func_safe();
        }
        // 6. generate bbl template
        // 7. output static analysis dbs 
        
    }

    if(Options::_dynamic_shuffle){
        if(!Options::_static_analysis){
            //read input static dbs
            NOT_IMPLEMENTED(wangzhe);
        }else{
            //generate code variant
            Module::init_cvm_from_modules();
            CodeVariantManager::init_code_variant_image(Options::_shuffle_img_path);
            CodeVariantManager::init_protected_proc_info(Options::_cc_offset, Options::_ss_offset);
            Module::generate_all_relocation_block();
        }
    }
    
    
    return 0;
}
