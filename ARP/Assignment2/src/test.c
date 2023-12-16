#include <stdio.h>
#include <time.h>

void writeToLogFile(const char *logpath, const char *logMessage) {
    FILE *logFile = fopen(logpath, "a");  // Open file in append mode

    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    // Get the current time
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = localtime(&rawTime);

    // Write log entry with timestamp
    fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
            timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, logMessage);

    fclose(logFile);
}

int main(int argc, char *argv[]) 
{   
    char *logpath = "Assignment2/log/server.log"; // Path for the log file
    writeToLogFile(logpath,"hello");
    while(1){

    }
    return 0;
}