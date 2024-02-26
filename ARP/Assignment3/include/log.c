#include <stdio.h>
#include <time.h>

// void writeToLogFile(const char *logpath, const char *logMessage) {
//     FILE *logFile = fopen(logpath, "a");  // Open file in append mode

//     if (logFile == NULL) {
//         perror("Error opening log file");
//         return;
//     }

//     // Get the current time
//     time_t rawTime;
//     struct tm *timeInfo;
//     time(&rawTime);
//     timeInfo = localtime(&rawTime);

//     // Write log entry with timestamp
//     fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
//             timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
//             timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, logMessage);

//     fclose(logFile);
// }


// void clearLogFile(const char *logpath) {
//     FILE *logFile = fopen(logpath, "w");  // Open file in write mode (truncates if exists)

//     if (logFile == NULL) {
//         perror("Error opening log file");
//         // Handle the error as needed
//     } else {
//         fclose(logFile);
//         printf("Logfile cleared successfully.\n");
//     }
// }