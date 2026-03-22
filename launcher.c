#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // Get the directory where this binary resides
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("readlink failed");
        return 1;
    }
    exe_path[len] = '\0';

    // Extract directory path (remove the filename)
    char *last_slash = strrchr(exe_path, '/');
    if (!last_slash) {
        fprintf(stderr, "Could not determine directory\n");
        return 1;
    }
    *last_slash = '\0';  // Now exe_path contains the directory

    // --- Run your custom script / executable ---
    // Change "custom.sh" to whatever you want to run (must be in the same directory)
    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), "%s/custom.sh", exe_path);

    // Check if the script exists and is executable
    if (access(script_path, X_OK) == 0) {
        printf("Running pre-start script: %s\n", script_path);
        int ret = system(script_path);
        if (ret != 0) {
            fprintf(stderr, "Script returned non-zero exit status: %d\n", ret);
            // Optionally abort starting the server
            // return 1;
        }
    } else {
        printf("No executable script found at %s (ignoring)\n", script_path);
    }

    // --- Start the real TeamSpeak server ---
    // Build the path to the renamed original binary (must be in the same directory)
    char real_server[PATH_MAX];
    snprintf(real_server, sizeof(real_server), "%s/ts3server_real", exe_path);

    // Replace current process with the real server, passing all arguments
    execv(real_server, argv);

    // If we get here, execv failed
    perror("execv failed");
    return 1;
}
