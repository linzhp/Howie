/**
 * JavaScriptProcessor - Experimental
 *
 * Uses Mozilla's SpiderMonkey JavaScript Engine. Hopefully, it will
 * work okay.
 *
 * @author	Jonathan Roewen
 */
#ifndef JAVA_SCRIPT_PROCESSOR_H
#define JAVA_SCRIPT_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>
#include <iostream>

//	Prolly need something for MacOS X later too...
#if defined(__BEOS__)
 #define XP_BEOS
#elif defined(WIN32)
 #define XP_PC
#else
 #define XP_UNIX
#endif

#include <jsapi.h>

using namespace std;

static void
my_ErrorReporter(JSContext *, const char *message, JSErrorReport *report)
{
//    int i, j, k, n;
    string prefix, tmp;

    if (!report) {
        cerr << message << endl;
        return;
    }

    /* Conditionally ignore reported warnings. */
    if (JSREPORT_IS_WARNING(report->flags))
        return;

    prefix = "";
    if (report->filename)
        prefix = string(report->filename) + ":";
    if (report->lineno) {
        tmp = prefix;
        prefix = tmp;
        prefix += report->lineno;
    }
    
    /* If there were no filename or lineno, the prefix might be empty */
    if (prefix.length() > 0)
        cerr << prefix;
    cerr << message << endl;

    if (!report->linebuf) {
        return;
    }

    /* report->linebuf usually ends with a newline. */
    cerr << prefix << report->linebuf << endl;
}

JSClass globalClass = {
	"global", 0,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
};

class JavaScriptProcessor : public AimlProcessor
{
public:
	~JavaScriptProcessor() {
		if (initialised) {
			JS_DestroyContext(context);
			JS_DestroyRuntime(runtime);
		}
	}
	JavaScriptProcessor() {
		initialised = false;
		//	Initialise the JavaScript Runtime (give it 8MB)
		runtime = JS_NewRuntime(8L * 1024L * 1024L);
		if (!runtime) {
			//	Creation failed...
			cerr << "Failed to initialise JavaScript Runtime" << endl;
			return;
		}
		context = JS_NewContext(runtime, 8192);
		if (context == NULL) {
			cerr << "Failed to create context for JavaScript engine" << endl;
			return;
		}
		JS_SetErrorReporter(context, my_ErrorReporter);
		globalObject = JS_NewObject(context, &globalClass, NULL, NULL);
		if (!(builtinObjects = JS_InitStandardClasses(context, globalObject))) {
			cerr << "Failed to initialise global objects" << endl;
			return;
		}
		initialised = true;
	}
	
	string getName() const {
		return "javascript";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		if (!initialised) {
			return "javascript not initialised";
		}
		string input = Kernel::process(m, e, r, id);
		//...
		jsval rval;
		JSString *str;
		char *filename = NULL;
		int lineNo = 0;
		
		if (JS_EvaluateScript(context, globalObject, input.c_str(), input.length(), filename, lineNo, &rval)) {
			str = JS_ValueToString(context, rval);
			string result = JS_GetStringBytes(str);
			return result;
		}
		return "failure";
	}
private:
	JSRuntime *runtime;
	JSContext *context;
	JSObject *globalObject;
	JSBool builtinObjects;
	
	bool initialised;
};

#endif