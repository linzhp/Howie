/**
 * SystemProcessor - Launches shell commands and 
 * captures their output
 *
 * @author	Taras Glek
 */
#ifndef SYSTEM_PROCESSOR_H
#define SYSTEM_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>
#include <stdio.h>
#ifdef WIN32
  #include <windows.h>
  #include <shellapi.h>
  
  #define BUFSIZE 4096
#endif

using namespace std;

class SystemProcessor : public AimlProcessor
{
public:
	/*pipes are goof good for fun and profit*/
	string process(Match *m, PElement e, Responder *r, const string &id)
	{
		string str = "";
		string cmd = "";
		if (e->hasChildren()) {
			cmd = Kernel::process(m, e, r, id);
		} else {
			cmd = m->getInputStar(1);
		}
		string mode = e->getAttribute("mode");
#if defined(WIN32)
		if (mode.empty() || mode == "async") {
			string prog = cmd;
			string args = "";
			string::size_type ix = prog.find(" ");
			if (ix != string::npos) {
				prog = prog.substr(0, ix);
				args = cmd.substr(ix + 1);
			}
			HINSTANCE h = ShellExecute(
				NULL,
				"open",
				prog.c_str(),
				args.c_str(),
				NULL,
				SW_SHOWNORMAL
			);
		} else {
			HANDLE cOutR, cOutW, cOutRD;
			HANDLE outSave;
			
			SECURITY_ATTRIBUTES sa;
			BOOL fSuccess;
			
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = NULL;
			
			outSave = GetStdHandle(STD_OUTPUT_HANDLE);
			
			if (!CreatePipe(&cOutR, &cOutW, &sa, 0))
				return "Couldn't create pipe";
			
			if (!SetStdHandle(STD_OUTPUT_HANDLE, cOutW))
				return "Couldn't redirect stdout";
			
			fSuccess = DuplicateHandle(GetCurrentProcess(), cOutR,
				GetCurrentProcess(), &cOutRD, 0, FALSE, DUPLICATE_SAME_ACCESS);
			if (!fSuccess)
				return "Couldn't duplicate handle";
			CloseHandle(cOutR);
			
			PROCESS_INFORMATION procInfo;
			STARTUPINFO supInfo;
			
			ZeroMemory(&supInfo, sizeof(STARTUPINFO));
			supInfo.cb = sizeof(STARTUPINFO);
			
			//	Need to check win version here!
			cmd = "cmd /c " + cmd;
			if (!CreateProcess(NULL,
				//yes is know this is bad..but
				//so is the windos api
				(char *)cmd.c_str(),
				NULL,
				NULL,
				TRUE,
				0,
				NULL,
				NULL,
				&supInfo,
				&procInfo)
			)
				return "Couldn't spawn process";
			
			if (!SetStdHandle(STD_OUTPUT_HANDLE, outSave))
				return "Couldn't restore stdout";
			
			DWORD dwRead;
			CHAR chBuf[4096];
			
			if (!CloseHandle(cOutW))
				return "Couldn't close handle";
				
			memset(chBuf, 0, 4096);
			
			while (true) {
				if (!ReadFile(cOutRD, chBuf, 4096, &dwRead, NULL) || dwRead == 0)
					break;
				str += chBuf;
				memset(chBuf, 0, 4096);
			}

			CloseHandle(cOutRD);
			// cort: this line is throwing an exception when j-alice is embedded in
            // a Python environment.  We need to catch & ignore that exception, though
            // I wish there were a way to avoid making the call under those circumstances.
            // Also, it appears to be throwing some sort of of bizarre instruction that
            // the following code doesn't properly ignore.  Whatever, I'm commenting it
            // out.
            //try {
			//    CloseHandle(outSave);
            //} catch (...) {}
		}
#else
		if (mode.empty() || mode == "sync") {
			FILE *f = popen(cmd.c_str(),"r");
			char s[500];
			int read;
			while(read=(fread(s, 1, sizeof(s)/sizeof(s[0])-1,f)))
			{
				s[read] = 0;
				str+=s;
			}
		} else {
			//	Find a way to do something asynchronously like Win32
		}
#endif

        // cort: "chomp" off a trailing newline, if one exists.
        string::size_type eolpos = str.find_first_of("\r\n");
        if (eolpos != string::npos)
        {
            str.resize(eolpos);
        }
		return str;
	}
//	~SystemProcessor() { }
};

#endif
