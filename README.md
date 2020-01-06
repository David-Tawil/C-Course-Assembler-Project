# C-Course-Assembler-Project
 C assembler for final project - Course 20465 Open University
 
 This is not a real Assembler, What the program really do is receiving a text file written like a assembly code which have been defined to us in the project guide and translate it to a special binary code which was also defined in the project guide.
 
 
 ## Overview
The assembler gets assembly files (.as) as input and generate 3 files for each:

The object file (.ob) which contains the encoding of the assembly code commands and the encoding of the data defined in the assembly file.

The entry file (.ent) which contains the encoding of the variables that are used by other files and their location in memory.

The external file (.ext) which contains the external variables that are used by the assembly code commands and the respective locations of the commands.

The assembly language used in this program is defined in the Project Definition document which can be found in the repository in Hebrew. The project is written in C in the ANSI C standard. The encoding of the output is done in base64.


## Usage
To run the assembler, provide the assembly files names (without the ".as" suffix) as parameters and run the program. The following code will run the program on the files "file1.as", "file2.as", "file3.as":

```
./my_assembler file1 file2 file3
```

