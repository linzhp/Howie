%module jalice

%{
#include <string>
%}

%include std_string.i
extern void bootstrap(void);
extern std::string respond(const std::string sInput, const std::string sId);
