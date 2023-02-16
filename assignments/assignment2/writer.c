#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[]) {
    openlog("writer", LOG_PID, LOG_USER); // initialize syslog
    
    if (argc != 3) {
        syslog(LOG_ERR, "Usage: writer <file> <string>");
        fprintf(stderr, "Usage: writer <file> <string>\n");
        return 1;
    }
    
    const char* filename = argv[1];
    const char* text = argv[2];
    
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        syslog(LOG_ERR, "Error opening file %s", filename);
        perror("Error opening file");
        return 1;
    }
    
    syslog(LOG_DEBUG, "Writing \"%s\" to %s", text, filename);
    fputs(text, file);
    
    if (fclose(file) != 0) {
        syslog(LOG_ERR, "Error closing file %s", filename);
        perror("Error closing file");
        return 1;
    }
    
    closelog(); // close syslog
    
    return 0;
}

