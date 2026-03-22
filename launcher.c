#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

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

    // Build path to custom.sh
    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), "%s/custom.sh", exe_path);

    // Check if custom.sh exists
    if (access(script_path, F_OK) != 0) {
        fprintf(stderr, "ERROR: custom.sh not found at %s\n", script_path);
        return 1;
    }

    // Make it executable
    char chmod_cmd[PATH_MAX + 50];
    snprintf(chmod_cmd, sizeof(chmod_cmd), "chmod +x \"%s\"", script_path);
    system(chmod_cmd);  // Ignore errors

    // Fix Windows line endings (CRLF -> LF) if sed is available
    char sed_cmd[PATH_MAX + 100];
    snprintf(sed_cmd, sizeof(sed_cmd), "sed -i 's/\\r$//' \"%s\" 2>/dev/null", script_path);
    system(sed_cmd);  // Ignore errors

    // Run the script
    printf("Running: %s\n", script_path);
    int ret = system(script_path);
    if (ret != 0) {
        fprintf(stderr, "Script exited with status %d\n", ret);
    }
    return ret;
}
