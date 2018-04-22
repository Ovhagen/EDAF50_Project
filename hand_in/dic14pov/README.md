# News Server - A project in the course EDAF50

##### Made by Hanna bayerlein (dic14hba), Erik olsson (dat14eol) and Pontus Ovhagen (dic14pov)

The service has mainly been developed on linux machines and some undefined behavior could appear in a Windows environment for your information.

### Installs
Use g++ compiler with support for C++ 11 to compile the source code.

Change the current directory to dic14pov/src/ where the MakeFile is situated.

Run the following commands to build and compile the project:

$ make all

$ make install

### Running the service

##### Starting up the client and the server
Now go to the directory dic14pov/bin/ and open two terminal windows. Below we provide the parameters needed to run the server and the client:

$ ./news_server [PORT] [Database version]     //Where database version is 1 for in memory and 2 for on disk.

$ ./news_client [HOST] [PORT]                 //The host parameter is usually localhost in this case.

You should now see that the server has started and that a client has connected.

##### Example of command issuance
The client program generally takes commands on the format <command> <option> <parameter1> <parameter2>... where either some or all parameters are required.
Add double quotes like "Text goes here" to input several words as one argument to a command.
The available options are '-ng' for Newsgroups and '-art' for article.

To clarify the structure of a command we give an example of a use case below:

->create &nbsp; -ng &nbsp; Sports    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;    // Creates a group sports with an ID of 1.

->create &nbsp; -art &nbsp; 1 &nbsp; "Why are athletes so fast today?" &nbsp; Martin &nbsp; "Athletes today drink to much red bull." &nbsp;&nbsp;&nbsp;&nbsp;   // Creates an article in the group with id 1.

Note that you can choose to use double quotes for capture of multiple words. However, when these are removed the program will just parse a single argument, like the name Martin in the example. If the double quotes are left out when e.g. using a text as an input an error of invalid number of arguments will appear.
