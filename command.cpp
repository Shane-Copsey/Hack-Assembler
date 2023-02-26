#include"command.hpp"

command::command(std::string _keyword, std::string _description, std::string _usage, function_ _execute, short _arguments, bool _last_arg_special):
    keyword(_keyword),description(_description),usage(_usage),execute(_execute),arguments(_arguments),last_arg_special(_last_arg_special)
    {
        amount_of_commands++;
        if (arguments==0) last_arg_special=false;
    }
std::string command::get_keyword() {return keyword;}
std::string command::get_description() {return description;}
std::string command::get_usage() {return usage;}
short command::get_arguments() {return arguments;}
bool command::get_last_arg_special() {return last_arg_special;}
short command::get_amount_of_commands() {return amount_of_commands;}
