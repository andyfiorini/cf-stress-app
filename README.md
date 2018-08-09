### cf-stress-app

Cloud Foundry simple application to stress diego-cells.

This C application is a small web server that runs insde Cloud Foundry.  

You can access this app using curl, wget or telnet.   You can send commands
in order to execute operations inside the Diego-cell where that application
is running.  For example, to stress out the cpu resources, you can send the
command:

param=cpu

For Memory+cpu stress test:

param=mem

For testing disk operations:

param=disk

You can also use the putparam.sh script.    There is an example on how to 
use this script on the Makefile.  You will need to replace the Ip address 
and put the name of your deployment there.

Cheers!
