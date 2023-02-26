#include<string>

typedef void (*function_)();
class command {
    private:
        std::string keyword;
        std::string description;
        std::string usage;
        short arguments;
        bool last_arg_special;
    public:
        command(std::string _keyword, std::string _description, std::string _usage, function_ _execute, short _arguments=0, bool _last_arg_special=false);
        std::string get_keyword();
        std::string get_description();
        std::string get_usage();
        short get_arguments();
        bool get_last_arg_special();
        function_ execute;
        static short amount_of_commands;
        static short get_amount_of_commands();
        static std::string current_input;
        static short current_ID;
};
