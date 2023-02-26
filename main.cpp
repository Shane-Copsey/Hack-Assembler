#include<iostream>
#include<bitset>
#include<sstream>
#include<windows.h>
#include<fstream>
#include<vector>

#include"command.hpp"
short command::amount_of_commands=0;
std::string command::current_input;
short command::current_ID;

struct symbol {
    std::string name;
    unsigned int address;
};

const char other_acceptable_chars[4]={'_','.','$',':'};
//const char ignored_characters[1]={' '};
const std::string comp[]={"0","1","-1","D","A","!D","!A","-D","-A","D+1","A+1","D-1","A-1","D+A","D-A","A-D","D&A","D|A","M","!M","-M","M+1","M-1","D+M","D-M","M-D","D&M","D|M"};
const short comp_num[]={0x2A,0x3F,0x3A,0x0C,0x30,0x0D,0x31,0x0F,0x33,0x1F,0x37,0x0E,0x32,0x02,0x13,0x07,0x00,0x15,0x30,0x31,0x33,0x37,0x32,0x02,0x13,0x07,0x00,0x15};
const std::string dest[]={"","M","D","MD","A","AM","AD","AMD"};
const std::string jump[]={"","JGT","JEQ","JGE","JLT","JNE","JLE","JMP"};

#define PREDEFINED_SYMBOLS_SIZE 23
const symbol predefined_symbols[PREDEFINED_SYMBOLS_SIZE]={
    {"SP",0},
    {"LCL",1},
    {"ARG",2},
    {"THIS",3},
    {"THAT",4},
    {"R0",0},
    {"R1",1},
    {"R2",2},
    {"R3",3},
    {"R4",4},
    {"R5",5},
    {"R6",6},
    {"R7",7},
    {"R8",8},
    {"R9",9},
    {"R10",10},
    {"R11",11},
    {"R12",12},
    {"R13",13},
    {"R14",14},
    {"R15",15},
    {"SCREEN",16384},
    {"KBD",24576}};
std::vector<symbol> symbols;
#define START_VAR_STORE 16
unsigned int allocate_here=START_VAR_STORE;
#define SYMBOL_NOT_FOUND 4294967295
std::string NumberTo8BitBinaryString(const short number)
{
    std::bitset<8> binary_constant(number);
    std::stringstream bitset_to_string;
    bitset_to_string << binary_constant;
    return bitset_to_string.str();
}
unsigned int repeated_symbol(const std::string name_check)
{
    unsigned int sym_count=0;
    for (;sym_count<=PREDEFINED_SYMBOLS_SIZE-1;sym_count++) {
        if (name_check==predefined_symbols[sym_count].name) return predefined_symbols[sym_count].address;
    }
    if (!symbols.empty()) {
        for (sym_count=0;sym_count<=symbols.size()-1;sym_count++) {
            if (name_check==symbols.at(sym_count).name) return symbols.at(sym_count).address;
        }
    }
    return SYMBOL_NOT_FOUND;
}

#define INVALID_CHAR 0
#define VALID_DIGIT_CHAR 1
#define VALID_NONDIGIT_CHAR 2

char is_valid_char(const char ascii)
{
    for (unsigned short special=0;special<=3;special++) {
        if (ascii==other_acceptable_chars[special]) return VALID_NONDIGIT_CHAR;
    }
    if (ascii>='A'&&ascii<='Z') return VALID_NONDIGIT_CHAR;
    if (ascii>='a'&&ascii<='z') return VALID_NONDIGIT_CHAR;
    if (ascii>='0'&&ascii<='9') return VALID_DIGIT_CHAR;
    return INVALID_CHAR;
}
bool good_name(const std::string name_check)
{
    if (name_check!="") {
        if (is_valid_char(name_check.at(0))==VALID_NONDIGIT_CHAR) {
            for (unsigned short check_char=1;check_char<=name_check.length()-1;check_char++) {
                if (is_valid_char(name_check.at(check_char))==INVALID_CHAR) return false;
            }
        } else {return false;}
    } else {return false;}
    return true;
}

void cls( HANDLE hConsole )
{
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    if( !GetConsoleScreenBufferInfo( hConsole, &csbi )) return;
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    if( !FillConsoleOutputCharacter( hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten )) return;
    if( !GetConsoleScreenBufferInfo( hConsole, &csbi )) return;
    if( !FillConsoleOutputAttribute( hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten )) return;
    SetConsoleCursorPosition( hConsole, coordScreen );
}
HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

int StringToNumber(std::string input, short& informationpointer, short first=0, short last=32767)
{
    informationpointer=1;
    int conversion[6]={0};
    if (last<first) {informationpointer=-1;return 0;}
    if (input.length()==0) {informationpointer=0;return 0;}
    bool negnum=false;
    if (input.at(0)==45)
    {
        if (first>=0) {informationpointer=0;return 0;}
        input.erase(0,1);
        negnum=true;
    }
    if (input.length()==0) {informationpointer=0;return 0;}
    while (input.length()>1) if (input.at(0)==48) input.erase(0,1); else break;
    for (short y=0; y<=input.length()-1; y++)
    {
        if (not(input.at(y)>=48 && input.at(y)<=57)) {informationpointer=0;return 0;}
        else conversion[input.length()-y]=(input.at(y)-48);
    }
    short xten=1;
    for (short y=1; y<=input.length(); y++)
    {
        short currentvalue=conversion[0];
        int tobeadded=conversion[y]*xten;
        conversion[0]+=tobeadded;
        if (conversion[0]<currentvalue || conversion[0]<tobeadded) {informationpointer=0;return 0;}
        xten*=10;
    }
    if (negnum) conversion[0] = -conversion[0];
    if (conversion[0] < first || conversion[0] > last) {informationpointer=0;return 0;}
    return conversion[0];
}
std::string NumberTo3BitBinaryString(const short number)
{
    std::bitset<3> binary_constant(number);
    std::stringstream bitset_to_string;
    bitset_to_string << binary_constant;
    return bitset_to_string.str();
}
std::string NumberTo6BitBinaryString(const short number)
{
    std::bitset<6> binary_constant(number);
    std::stringstream bitset_to_string;
    bitset_to_string << binary_constant;
    return bitset_to_string.str();
}
std::string NumberTo15BitBinaryString(const short number)
{
    std::bitset<15> binary_constant(number);
    std::stringstream bitset_to_string;
    bitset_to_string << binary_constant;
    return bitset_to_string.str();
}

void assemble_console_command();
void exit_console_command();
void cls_console_command();
void cd_console_command();
void cdup_console_command();
void dir_console_command();
void help_console_command();

command commands[]={
    command("assemble",
            "assembles a hack .asm (assembly) file into a hack .hack (binary) file",
            "assemble XXX.asm",
            assemble_console_command,
            1,
            true),
    command("exit",
            "exits this program",
            "exit",
            exit_console_command),
    command("cls",
            "clears the console's screen",
            "cls",
            cls_console_command),
    command("cd",
            "used to change the current directory to another within it",
            "cd dir",
            cd_console_command,
            1,
            true),
    command("cd..",
            "changes the current directory to the parent directory",
            "cd..",
            cdup_console_command),
    command("dir",
            "displays the current directory",
            "dir",
            dir_console_command),
    command("help",
            "lists all commands with their corresponding descriptions and usages",
            "help",
            help_console_command)
};

std::vector<std::string> console_argument_parser(std::string input, short max_args=1, bool last_arg_special=false)
{
    short args_found=0;
    short last_word_pos=0;
    bool space=false;
    short last_char_before_space_pos=-1;
    std::vector<std::string> retval;
    if (input.length()==0 || input.length()>32767) return retval;
    for (short find_space=0;find_space<=input.length()-1;find_space++) {
        if (!space) last_char_before_space_pos++;
        if (input.at(find_space)==' ') {space=true;}
        else if (space) {
            args_found++;
            if (max_args<args_found) {
                retval.clear();
                return retval;
            } else if (max_args==args_found&&last_arg_special) {
                retval.push_back(input.substr(last_word_pos,(last_char_before_space_pos-last_word_pos)+1));
                retval.push_back(input.substr(find_space,input.npos));
                return retval;
            }
            space=false;
            retval.push_back(input.substr(last_word_pos,(last_char_before_space_pos-last_word_pos)+1));
            last_word_pos=find_space;
            last_char_before_space_pos=find_space;
        }
    }
    retval.push_back(input.substr(last_word_pos,(last_char_before_space_pos-last_word_pos)+1));
    return retval;
}
void cd_console_command()
{
    std::vector<std::string> parsed_spaces= console_argument_parser(command::current_input,commands[command::current_ID].get_arguments(),commands[command::current_ID].get_last_arg_special());
    if (parsed_spaces.empty()) {
        std::cout << "too many arguments or something else went wrong" << std::endl;
        return;
    }
    parsed_spaces.erase(parsed_spaces.begin());
    if (parsed_spaces.empty()) {
        std::cout << "too little arguments" << std::endl;
        return;
    }
    SetCurrentDirectory(parsed_spaces.at(0).c_str());
    system("cd");
}
void cdup_console_command()
{
    std::vector<std::string> parsed_spaces=
        console_argument_parser(command::current_input,commands[command::current_ID].get_arguments(),commands[command::current_ID].get_last_arg_special());
    if (parsed_spaces.empty()) {
        std::cout << "too many arguments or something else went wrong" << std::endl;
        return;
    }
    SetCurrentDirectory("..");
    system("cd");
}
void dir_console_command()
{
    std::vector<std::string> parsed_spaces=
        console_argument_parser(command::current_input,commands[command::current_ID].get_arguments(),commands[command::current_ID].get_last_arg_special());
    if (parsed_spaces.empty()) {
        std::cout << "too many arguments or something else went wrong" << std::endl;
        return;
    }
    system("dir");
}
void assemble_console_command()
{
    std::vector<std::string> parsed_spaces=
        console_argument_parser(command::current_input,commands[command::current_ID].get_arguments(),commands[command::current_ID].get_last_arg_special());
    if (parsed_spaces.empty()) {
        std::cout << "too many arguments or something else went wrong" << std::endl;
        return;
    }
    parsed_spaces.erase(parsed_spaces.begin());
    if (parsed_spaces.empty()) {
        std::cout << "too little arguments" << std::endl;
        return;
    }
    if (parsed_spaces.at(0).substr(parsed_spaces.at(0).length()-4,4)!=".asm") {
        std::cout << "invalid file type" << std::endl;
        return;
    }
    std::fstream asmfile(parsed_spaces.at(0).c_str());
    if (asmfile.fail()) {
        std::cout << "file does not exist or another issue occurred; try again" << std::endl;
        asmfile.close();
        return;
    } else if (!asmfile.is_open()) {
        std::cout << "error opening the file; try again" << std::endl;
        asmfile.close();
        return;
    }
    unsigned int line_count=0;
    unsigned int real_line_count=1;
    std::vector<std::string> assembled_code;
    bool error=true;
    do {
        if (line_count==4294967295) {
            std::cout << "too many lines in the .asm file (max lines accepted is 4294967295)" << std::endl;
            break;
        }
        line_count++;
        std::string current_line;
        if (!asmfile.eof()) {
            if (asmfile.good()) {
                std::getline(asmfile,current_line);
            } else {
                std::cout << "something went wrong while reading the file" << std::endl;
                std::cout << "a data race most likely occurred" << std::endl;
                std::cout << "line " << line_count << " was the last line loaded" << std::endl;
                break;
            }
        } else {
            error=false;
            std::cout << "file mostly parsed; allocation of non-label symbols left" << std::endl;
            break;
        }
        if (current_line=="") continue;
        if (current_line.length()>65535) {
            std::cout << "line " << line_count << ": too many characters" << std::endl;
            break;
        }
        bool complete_line=false;
        char instruction_type=0;
        bool one_slash=false;
        bool one_parenthesis=false;
        bool line_success=true;
        bool eq_found=false;
        char check_jmp=0;
        char num_count=0;
        bool A_instruction_var=false;
        std::string parsed_asm_line="";
        for (unsigned short parse=0;parse<=current_line.length()-1;parse++) {
            if (current_line.at(parse)==' ') continue;
            if ((instruction_type==0||complete_line)&&!one_parenthesis&&one_slash&&current_line.at(parse)=='/') break;
            if ((instruction_type==0||complete_line)&&!one_parenthesis&&current_line.at(parse)=='/') {
                one_slash=true;
                continue;
            }
            if ((instruction_type==3)&&!one_slash&&one_parenthesis&&current_line.at(parse)==')') continue;
            if ((instruction_type==0)&&!one_slash&&current_line.at(parse)=='(') {
                instruction_type=3;
                one_parenthesis=true;
                continue;
            }
            if (instruction_type==0&&!one_slash&&!one_parenthesis) {
                if (current_line.at(parse)=='@') {
                    instruction_type=1;
                    continue;
                }
                if (current_line.at(parse)=='M'||current_line.at(parse)=='D'||current_line.at(parse)=='A'||current_line.at(parse)=='-'||current_line.at(parse)=='!'||current_line.at(parse)=='0'||current_line.at(parse)=='1') {
                    instruction_type=2;
                    parsed_asm_line.append(current_line.substr(parse,1));
                    continue;
                }
            }
            if (!complete_line) {
                if (instruction_type==1) {
                    if ((A_instruction_var||(!(current_line.at(parse)>='0'&&current_line.at(parse)<='9')))&&num_count==0) {
                        A_instruction_var=true;
                        parsed_asm_line.append(current_line.substr(parse,1));
                        continue;
                    } else if ((current_line.at(parse)>='0'&&current_line.at(parse)<='9')&&num_count<5) {
                        num_count++;
                        parsed_asm_line.append(current_line.substr(parse,1));
                        continue;
                    }
                    parse--;
                    complete_line=true;
                    continue;
                }
                if (instruction_type==2) {
                    if (check_jmp==0&&(current_line.at(parse)=='M'||current_line.at(parse)=='D'||current_line.at(parse)=='A'||current_line.at(parse)=='-'||current_line.at(parse)=='!'||current_line.at(parse)=='+'||current_line.at(parse)=='0'||current_line.at(parse)=='1'||current_line.at(parse)=='&'||current_line.at(parse)=='|')) {
                        parsed_asm_line.append(current_line.substr(parse,1));
                        continue;
                    }
                    if (!eq_found&&current_line.at(parse)=='=') {
                        eq_found=true;
                        parsed_asm_line.append(current_line.substr(parse,1));
                        continue;
                    }
                    if (current_line.at(parse)==';') {
                        check_jmp=1;
                        parsed_asm_line.append(current_line.substr(parse,1));
                        continue;
                    }
                    if (check_jmp==1&&current_line.at(parse)=='J') {
                        check_jmp=2;
                        parsed_asm_line.append(current_line.substr(parse,1));
                        continue;
                    }
                    if (check_jmp==2&&(current_line.at(parse)=='M'||current_line.at(parse)=='G'||current_line.at(parse)=='L'||current_line.at(parse)=='E'||current_line.at(parse)=='N')) {
                        check_jmp=3;
                        parsed_asm_line.append(current_line.substr(parse,1));
                        continue;
                    }
                    if (check_jmp==3&&(current_line.at(parse)=='P'||current_line.at(parse)=='T'||current_line.at(parse)=='Q'||current_line.at(parse)=='E')) {
                        check_jmp=4;
                        complete_line=true;
                        parsed_asm_line.append(current_line.substr(parse,1));
                        continue;
                    }
                    parse--;
                    complete_line=true;
                    continue;
                }
                if (instruction_type==3) {parsed_asm_line.append(current_line.substr(parse,1));continue;}
            } else {line_success=false;break;}
            line_success=false;
            break;
        }
        if (!line_success) {
            std::cout << "line " << line_count << ": syntax error" << std::endl;
            break;
        }
        if (parsed_asm_line==""||instruction_type==0) continue;
        std::string translated_binary_line="";
        if (instruction_type==1) {
            translated_binary_line.append("0");
            if (A_instruction_var) {
                if (parsed_asm_line!="") {
                    if (good_name(parsed_asm_line)) {
                        try {assembled_code.push_back(parsed_asm_line);}
                        catch (...) {
                            std::cout << "error while reading from the file" << std::endl;
                            std::cout << "file almost certainly too large; try again" << std::endl;
                            break;
                        }
                        real_line_count++;
                        continue;
                    } else {
                        std::cout << "line " << line_count << ": syntax error, bad label" << std::endl;
                        break;
                    }
                } else {
                    std::cout << "line " << line_count << ": syntax error, empty label" << std::endl;
                    break;
                }
            } else {
                short information;
                const short constant=StringToNumber(parsed_asm_line,information);
                if (information==1) {
                    translated_binary_line.append(NumberTo15BitBinaryString(constant));
                } else {
                    std::cout << "line " << line_count << ": syntax error" << std::endl;
                    break;
                }
            }
        }
        if (instruction_type==2) {
            translated_binary_line.append("111");
            if (check_jmp==1) parsed_asm_line.erase(parsed_asm_line.find(';'),1);
            if (eq_found) {
                if (parsed_asm_line.at(0)=='=') {
                    parsed_asm_line.erase(0,1);
                    eq_found=false;
                }
            }
            if (eq_found) {
                for (short comps=0;comps<=27;comps++) {
                    if (check_jmp!=4) {
                        if (parsed_asm_line.substr(parsed_asm_line.find('=')+1,parsed_asm_line.npos)==comp[comps]) {
                            if (comps<=17) translated_binary_line.append("0");
                            else translated_binary_line.append("1");
                            translated_binary_line.append(NumberTo6BitBinaryString(comp_num[comps]));
                            break;
                        }
                    } else {
                        if (parsed_asm_line.substr(parsed_asm_line.find('=')+1,parsed_asm_line.find(';')-parsed_asm_line.find('=')-1)==comp[comps]) {
                            if (comps<=17) translated_binary_line.append("0");
                            else translated_binary_line.append("1");
                            translated_binary_line.append(NumberTo6BitBinaryString(comp_num[comps]));
                            break;
                        }
                    }
                }
                for (short dests=1;dests<=7;dests++) {
                    if (parsed_asm_line.substr(0,parsed_asm_line.find('='))==dest[dests]) {
                        translated_binary_line.append(NumberTo3BitBinaryString(dests));
                        break;
                    }
                }
            } else {
                for (short comps=0;comps<=27;comps++) {
                    if (check_jmp!=4) {
                        if (parsed_asm_line.substr(0,parsed_asm_line.npos)==comp[comps]) {
                            if (comps<=17) translated_binary_line.append("0");
                            else translated_binary_line.append("1");
                            translated_binary_line.append(NumberTo6BitBinaryString(comp_num[comps]));
                            break;
                        }
                    } else {
                        if (parsed_asm_line.substr(0,parsed_asm_line.find(';'))==comp[comps]) {
                            if (comps<=17) translated_binary_line.append("0");
                            else translated_binary_line.append("1");
                            translated_binary_line.append(NumberTo6BitBinaryString(comp_num[comps]));
                            break;
                        }
                    }
                }
                translated_binary_line.append("000");
            }
            if (check_jmp==4) {
                for (short jmps=1;jmps<=7;jmps++) {
                    if (parsed_asm_line.substr(parsed_asm_line.find(';')+1,parsed_asm_line.npos)==jump[jmps]) {
                        translated_binary_line.append(NumberTo3BitBinaryString(jmps));
                        break;
                    }
                }
            } else {translated_binary_line.append("000");}
        }
        if (instruction_type==3) {
            if (parsed_asm_line!="") {
                if (good_name(parsed_asm_line)) {
                    if (repeated_symbol(parsed_asm_line)==SYMBOL_NOT_FOUND) {
                        try {symbols.push_back({parsed_asm_line,real_line_count-1});}
                        catch (...) {
                            std::cout << "error while reading from the file" << std::endl;
                            std::cout << "file almost certainly too large; try again" << std::endl;
                            break;
                        }
                        continue;
                    } else {
                        std::cout << "line " << line_count << ": syntax error, label already declared" << std::endl;
                        break;
                    }
                } else {
                    std::cout << "line " << line_count << ": syntax error, bad label" << std::endl;
                    break;
                }
            } else {
                std::cout << "line " << line_count << ": syntax error, empty label" << std::endl;
                break;
            }
        }
        if (translated_binary_line.length()==16) {
            try {assembled_code.push_back(translated_binary_line);}
            catch (...) {
                std::cout << "error while reading from the file" << std::endl;
                std::cout << "file almost certainly too large; try again" << std::endl;
                break;
            }
        }
        else {
            std::cout << "line " << line_count << ": syntax error" << std::endl;
            break;
        }
        real_line_count++;
    } while(true);
    asmfile.close();
    if (!error) {
        if (!assembled_code.empty()) {
            for (unsigned int line=0;line<=assembled_code.size()-1;line++) {
                if (!(assembled_code.at(line).at(0)>='0'&&assembled_code.at(line).at(0)<='9')) {
                    std::string translated_binary_line="0";
                    unsigned int found;
                    if ((found=repeated_symbol(assembled_code.at(line)))!=SYMBOL_NOT_FOUND) {
                        translated_binary_line.append(NumberTo15BitBinaryString(found));
                        assembled_code.at(line)=translated_binary_line;
                    } else {
                        try {symbols.push_back({assembled_code.at(line),allocate_here});}
                        catch (...) {
                            std::cout << "error while reading from the file" << std::endl;
                            std::cout << "file almost certainly too large; try again" << std::endl;
                            error=true;
                            break;
                        }
                        translated_binary_line.append(NumberTo15BitBinaryString(allocate_here));
                        assembled_code.at(line)=translated_binary_line;
                        allocate_here++;
                    }
                }
            }
        }
    }
    if (error) {
        std::cout << "ERROR" << std::endl;
        assembled_code.clear();
        symbols.clear();
        allocate_here=START_VAR_STORE;
    } else {
        std::cout << "allocation of non-label symbols finished; writing assembled code to " << parsed_spaces.at(0).substr(0,parsed_spaces.at(0).length()-4) << ".hack" << std::endl;
        std::string hackfile_name=parsed_spaces.at(0).substr(0,parsed_spaces.at(0).length()-4);
        hackfile_name.append(".hack");
        std::ofstream hackfile(hackfile_name.c_str());
        if (!assembled_code.empty()) {
            for (unsigned int line=0;line<=assembled_code.size()-1;line++) hackfile << assembled_code.at(line) << "\n";
        }
        std::cout << "file assembled (no errors)" << std::endl;
        assembled_code.clear();
        symbols.clear();
        allocate_here=START_VAR_STORE;
        hackfile.close();
    }
}
void exit_console_command()
{
    std::vector<std::string> parsed_spaces=
        console_argument_parser(command::current_input,commands[command::current_ID].get_arguments(),commands[command::current_ID].get_last_arg_special());
    if (parsed_spaces.empty()) {
        std::cout << "too many arguments or something else went wrong" << std::endl;
        return;
    }
    exit(0);
}
void cls_console_command()
{
    std::vector<std::string> parsed_spaces=
        console_argument_parser(command::current_input,commands[command::current_ID].get_arguments(),commands[command::current_ID].get_last_arg_special());
    if (parsed_spaces.empty()) {
        std::cout << "too many arguments or something else went wrong" << std::endl;
        return;
    }
    cls(hStdout);
}
void help_console_command()
{
    std::vector<std::string> parsed_spaces=
        console_argument_parser(command::current_input,commands[command::current_ID].get_arguments(),commands[command::current_ID].get_last_arg_special());
    if (parsed_spaces.empty()) {
        std::cout << "too many arguments or something else went wrong" << std::endl;
        return;
    }
    for (short c=0;c<=command::get_amount_of_commands()-1;c++) {
        std::cout << std::endl << commands[c].get_keyword() << " - " << commands[c].get_description();
        if (commands[c].get_description().length()!=69) std::cout << std::endl;
        std::cout << "usage: " << commands[c].get_usage() << std::endl;
    }
    std::cout << std::endl << std::endl;
}
std::string getinputfromuser()
{
    std::string input;
    std::getline(std::cin, input);
    return input;
}
int main() {
    std::cout << "type the command help to see all of the commands" << std::endl;
    do {
        std::cout << "> ";
        command::current_input=getinputfromuser();
        std::string keyword=" ";
        if (command::current_input.length()>0) {
            bool goto_top=false;
            do {
                if (command::current_input.at(0)==' ') {
                    if (command::current_input.length()==1) {
                        goto_top=true;
                        break;
                    }
                    command::current_input.erase(0,1);
                }
                else break;
            } while (true);
            if (goto_top) continue;
            short ichar=0;
            for (ichar=0;ichar<=command::current_input.length()-1;ichar++) {
                if (command::current_input.at(ichar)==' ') {
                    keyword=command::current_input.substr(0,ichar);
                    break;
                }
            }
            if (ichar==command::current_input.length()) keyword=command::current_input;
        } else continue;
        bool found_command=false;
        for (short icommands=0;icommands<=command::get_amount_of_commands()-1;icommands++) {
            if (keyword==commands[icommands].get_keyword()) {
                found_command=true;
                command::current_ID=icommands;
                commands[icommands].execute();
                break;
            }
        }
        if (!found_command) std::cout << keyword << " is not a valid command" << std::endl;
    } while (true);
    return 0;
}
