#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Run your custom script/program
    system("./custom.sh");
    
    // Execute the real TeamSpeak server with all arguments
    execv("./ts3server_real", argv);
    
    return 1;
}
