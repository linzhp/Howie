%module jalice

%{
#include <string>
%}

%include std_string.i
extern void bootstrap(void);
extern std::string respond(std::string sInput, std::string sId);
