#include "Kernel.h"
#include "Main.h"
#include "Memory.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;
#include <list>
#include <string>
using std::string;
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

// change to a new directory, saving the old directory to return to later.
typedef std::list<string> StrList;
typedef StrList::iterator StrListItor;
static StrList _dirStack;
static bool _pushd(const string& sNewDir)
{
  // Gotta save the old directory first.
  char cwd[256];
  if (getcwd(cwd, 256) == NULL)
    {
      cerr << "_pushd: current dir is too long.\n";
      return false;
    }
  _dirStack.push_back(cwd);

  // Change to new directory.
  int retval = chdir(sNewDir.c_str());
  if (retval != 0)
    {
      cerr << "_pushd: " << sNewDir << " not found.\n";
      return false;
    }

  // Success!
  return true;
}
static bool _popd(void)
{
  if (_dirStack.empty())
    return false;
  
  // Retrieve new directory from top of stack.
  string sNewDir = _dirStack.back();
  _dirStack.pop_back();

  // go there.
  int retval = chdir(sNewDir.c_str());
  if (retval != 0)
    {
      cerr << "_pushd: " << sNewDir << " not found.\n";
      return false;
    }

  // Success!
  return true;
}

// Locates the directory where the bot's configuration is stored (AIML
// and whatnot).  Store the result in sConfigDir.  Returns true if the
// config directory is found, false otherwise.
static bool _getJaliceConfigDir(string& sConfigDir)
{
  FILE *pConfigFile = NULL;

  // Check the current directory.
  pConfigFile = fopen("std-startup.aiml", "r");
  if (pConfigFile != NULL)
    {
      fclose(pConfigFile);
      sConfigDir = ".";
      return true;
    }

  // Check $HOME/.jalice
  const char *szHome = getenv("HOME");
  if (szHome != NULL)
    {
      string sJaliceHome( szHome );
      sJaliceHome += "/.jalice";
      string sConfigFilePath = sJaliceHome + "/std-startup.aiml";
      pConfigFile = fopen(sConfigFilePath.c_str(), "r");
      if (pConfigFile != NULL)
	{
	  fclose(pConfigFile);
	  sConfigDir = sJaliceHome;
	  return true;
	}
    }

  // Check $JALICE_CONFIG
  const char *szJaliceConfig = getenv("JALICE_CONFIG");
  if (szJaliceConfig != NULL)
    {
      string sJaliceHome( szJaliceConfig );
      string sJaliceConfigFile = sJaliceHome + "/std-startup.aiml";
      pConfigFile = fopen(sJaliceConfigFile.c_str(), "r");
      if (pConfigFile != NULL)
	{
	  fclose(pConfigFile);
	  sConfigDir = sJaliceHome;
	  return true;
	}
    }

  // Failure.
  return false;
}

// Gives us a kernel to play with, locally.
static inline Kernel *_getKernel(void)
{
  static Kernel *_pKern = NULL;
  if (_pKern == NULL)
    {
      // Get the configuration directory.
      string sHome;
      if (!_getJaliceConfigDir(sHome))
	{
	  cerr << "bootstrap failed: config directory not found.\n";
	  return NULL;
	}
      
      // Initialize the ol' Kernel.
      _pushd(sHome);
      _pKern = new Kernel;
      assert(_pKern != NULL);
      _popd();
    }
  return _pKern;
}
#define g_pKern _getKernel()


// public functions
void bootstrap(void)
{
  // Get the configuration directory.
  string sHome;
  if (!_getJaliceConfigDir(sHome))
    {
      cerr << "bootstrap failed: config directory not found.\n";
      return;
    }

  // Perform the bootstrap, and the return to the current directory.
  // NOTE: for some reason, g_pKern->bootstrap() never calls _getKernel()!
  // But it somehow ends up with a valid Kernel object anyway...TRULY
  // messed up.  Workaround by creating a local copy of g_pKern.
  _pushd(sHome);
  Kernel *k = g_pKern;
  k->bootstrap();
  loadConfig();
  _popd();
}

string respond(const string sInput, const string sId)
{
    // Get the configuration directory.
    string sHome;
    if (!_getJaliceConfigDir(sHome))
    {
        cerr << "respond failed: config directory not found.\n";
        return "";
    }

    _pushd(sHome);
    Kernel *k = g_pKern;
    string sResult = k->respond(sInput, sId);
    _popd();
    return sResult;
}
