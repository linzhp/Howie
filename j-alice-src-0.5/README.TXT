

        W e l c o m e    T o    J - A l i c e

     ---=====================================---

          Version 0.5  (built: Jun 20 2003)

  Copyright © 2001-2003 Jonathan Roewen, Taras Glek

            Licensed under the MIT License

     ---======================================---

 About This Build:

 - This is the source only release of J-Alice
 - Anna AIML set pre-configured
 - Only IRC (plus PassThru) and mini HTTP Server are
   functional
 - Startup file is now 'std-startup.xml' instead of
   'std-startup.aiml'

 Compiling J-Alice:

   There are some issues with a number of compilers.
 Some are known to not like J-Alice, such as gcc pre
 3.x. However, changing headers, etc might make it
 compile.
   Two known standard classes known to be weird with
 some compilers is the string stream class, and the
 istreambuf_iterator (some compilers don't have this
 class). For the istreambuf_iterator, the standard
 istream_iterator class can be safely used instead.
 However, it is likely that the performance of
 J-Alice will suffer greatly. For the string stream
 class, the main issue is a matter of naming of the
 class itself, and the header name to include. Your
 compiler should have documentation on the string
 stream class, and the header to use.
   To enable the GUI, you'll need to download, and
 install wxWindows. You must also define the symbol
 GUI for the gui code to be compiled.
   To add JavaScript support, you'll need to download
 and install the SpiderMonkey engine available from
 <http://mozilla.org/js> and also define the symbol
 ENABLE_JAVASCRIPT for it to be compiled into
 J-Alice.
   The only tested compilers that are known to work
 have been Metrowerks CodeWarrior 5.3 for Windows
 (5.0 without the update does not work), and gcc 3.x
 for linux. Currently, latest J-Alice code has not
 been tested under any other operating system. To
 link under Windows, you must link against the
 WinSock2 library.

 Configuring J-Alice:

   Configuration options are in the j-alice.ini
 file. A lot of the options don't actually work as
 yet, just the core components to get things
 running.

   For creating a new IRC configuration, it is
 recommended to alter the existing configuration, or
 better yet, copy it. If you don't want to use the
 existing configuration, simply change the value of
 enabled to "false".

   The PassThru port value is the port that a built-
 in IRC server will listen on (one per irc
 connection).

 The GUI (Graphical User Interface):

   This is the new replacement over the old console.
 It's still very basic at present, and is mainly
 used to debug J-Alice, and to provide a nice and
 safe way to close J-Alice.

   Note that it doesn't actually provide any means
 of interacting with J-Alice directly.

 Example AIML:

   In the examples folder, several example AIML
 files have been provided. Note that the new file
 extension used is xml, since AIML files are a type
 of XML file, and makes opening in standard XML
 editors easy.

   These files contain examples of using J-Alice's
 custom tags for securing AIML content, using the
 context tag, and creating categories for automated
 learning. The use of context is demonstrated in
 the authorisation example.

 Obtaining Help & Assistance:

   For help & assistance, the recommended method is
 to post to the help forum. Links are available from
 the homepage <http://j-alice.org>

 Ideas for Improvement & Other Suggestions:

   The recommended forum to use is the open
 discussion forum. Alternatively, the J-Alice
 General mailing list can be used. Note that you
 must be subscribed to post. Links to subscribing to
 the list are on the main page <http://j-alice.org>

 Known Issues:

   A bug has been discovered in the parser, such that
 self closing tags with default parameters, such as
 <star /> are not parsed correctly. Currently, using
 <star /> instead of <star/> produces incorrect
 results. Solution is to either use <star/> or always
 state the parameters explicitly, such as:
 <star index="1"/> or <star index="1" />

 The End
