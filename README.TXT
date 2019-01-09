Raydium is a game engine. It provides a set of functions wich allow quick and flexible games creation.
	http://raydium.org

	You can find lot's of usefull doc and tutorials in the wiki, there are also some application example and lot's of informations. (http://wiki.raydium.org/wiki/Raydium) 

	Use the forum http://memak.raydium.org/ for questions.

Details of this Sdk:

This sdk come with CodeBlock Ide.

	This Sdk integrate CodeBlock and Mingw as Ide and compiler. With this sdk you can begin to work with raydium in few minutes (except download time ).
	To use the ide just run codeblock.bat batch file.


Other batch files:

Blender.bat:

	This command file install import and export scripts directly in blender script directory.
	Be carrefull this script is really basic and just copy file to the standart blender script directory (C:\Program Files\Blender Foundation\Blender).

compile_raydium_dll.bat

	Build raydium dll and put the result in the bin directory.

compile.bat

	Used to build an application or test program. Use in a command line for example:
	compile test6
	The executable program is in the bin directory.

compile_all.bat

	batch compile of all tests sample and put executable in bin directory. A good way to see what raydium can do.

cleanup.bat

	Clean and delete sdk directory from temp / obj files.

update bin.bat

	Copy php scripts (mania_drive, train and others) to bin directory. This is usefull if those scripts are updated from svn.
	Copy Translation files to bin directory.

tests directory:

	This directory is there for your own tests. A sample workspace and projets for code blocks allows to tests and debug several applications. It's a base to begin to work with raydium.
	You can change what you want as needed.



