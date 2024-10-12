#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


void show_help(char *argv[]);
void count_log_levels(const char *file_path);
void list_log_messages(const char *file_path, char *level);
void calculate_uptime(const char *file_path);
int strptime(const char *timestamp, struct tm *tm_time);

int main(int argc, char *argv[])
{
    int count_levels = 0;
    int list_level = 0;
    int uptime = 0;
    char *file_path = NULL;
    //String to store list level
    char level[20] = {0};

    //Argument count must be greater than 3.
    //If fewer than 3 arguments are provided, the program does not have enough infomation to proceed.
    if(argc < 3)
    {
        printf("Error: Not enough arguments\n");
        show_help(argv);
        return 1;
    }
    //A for loop that starts at i = 1 and iterates through all command-line arguments provided to the program.
    for(int i = 0; i < argc; i++)
    {
        //Checks whether the current argument is equal to the string "--file".
        //If so, it indicates that the --file option has been provided.
        //If i + 1 is greater than or equal to argc, that means there are no more arguments left, and the program would run into an error if it tries to access argv[i + 1]
        if(strcmp(argv[i], "--file") == 0 && i + 1 < argc)
        {
            //If both condition in the if statement are true, then the next argument (argv[++i]) is assumed to be the file path
            file_path = argv[++i];
        }
        //Checks whether the current argument is equal to the string "--count-levels".
        //If so, it indicates that the --count-levels option has been provided.
        else if(strcmp(argv[i], "--count-levels") == 0)
        {
            count_levels = 1;
        }
        //Checks whether the current argument is equal to the string "--list".
        //If so, it indicates that the --list option has been provided.
        else if(strcmp(argv[i], "--list") == 0 && i + 1 < argc)
        {
            list_level = 1;
            //If both condition in the if statement are true, then the next argument (argv[++i]) is assumed to be the <level>
            strcpy(level, argv[++i]);
        }
        //Checks whether the current argument is equal to the string "--uptime".
        //If so, it indicates that the --uptime option has been provided.
        else if(strcmp(argv[i], "--uptime") == 0)
        {
            uptime = 1;
        }
        //Checks whether the current argument is equal to the string "--help".
        //If so, it indicates that the --help option has been provided.
        else if(strcmp(argv[i], "--help") == 0)
        {
            show_help(argv);
            return 0;
        }
    }

    if(file_path == NULL)
    {
        printf("Error: File path not exist. \n");
        show_help(argv);
        return 1;
    }

    if(count_levels)
    {
        count_log_levels(file_path);
    }

    if(list_level)
    {
        list_log_messages(file_path, level);
    }

    if(uptime)
    {
        calculate_uptime(file_path);
    }
}

//This function will displays the count of log message by log level(INFO, WARNING, ERROR).
void count_log_levels(const char *file_path)
{
    char line[200],log_level[200], time_stamp[200], message[200];
    int warning_count = 0, info_count = 0, error_count = 0;

    //First open file from file path in read mode.
    FILE *file;
    file = fopen(file_path,"r");
    if(file == NULL)
    {
        printf("Error: Error opening file");
        return;
    }

    //Read the file line by line then store it in the line array.
    while(fgets(line, sizeof(line), file))
    {
        //Extracts the time_stamp, log_level, and message from each line based on the format provided.
        sscanf(line, "%19[^\t] %9s %[^\n]", time_stamp, log_level, message);
        if(strcmp(log_level, "WARNING") == 0)
        {
            warning_count += 1;
        }
        else if(strcmp(log_level, "INFO") == 0)
        {
            info_count += 1;
        }
        else if(strcmp(log_level, "ERROR") == 0)
        {
            error_count += 1;
        }
    }
    fclose(file);

    //Display log info.
    printf("Log Summary:\n");
    printf("INFO: %d messages\n", info_count);
    printf("WARNING: %d messages\n", warning_count);
    printf("ERROR: %d messages\n", error_count);
    printf("\n");
}

//This function will lists all messages with the input level.
void list_log_messages(const char *file_path, char *level)
{
    char line[200],log_level[200], time_stamp[200], message[200];

    //First open file from file path in read mode.
    FILE *file;
    file = fopen(file_path,"r");
    if(file == NULL)
    {
        printf("Error: Error opening file");
        return;
    }
    level[0] = toupper(level[0]);
    printf("%s messages: \n", level);
    //Read the file line by line then store it in the line array.
    while(fgets(line, sizeof(line), file))
    {
        //Extracts the time_stamp, log_level, and message from each line based on the format provided.
        sscanf(line, "%19[^\t] %9s %[^\n]", time_stamp, log_level, message);
        //Compare the value of log_level with level
        if (strcasecmp(log_level, level) == 0)
        {
            printf("%s %s %s\n", time_stamp, log_level, message);
        }
    }
    printf("\n");
    fclose(file);
}

//This function will parses a timestamp string into a struct tm format
char *custom_strptime(const char *s, const char *format, struct tm *tm)
{
    // The format expected: "%Y-%m-%d %H:%M:%S"
    if (sscanf(s, "%d-%d-%d %d:%d:%d",
               &tm->tm_year, &tm->tm_mon, &tm->tm_mday,
               &tm->tm_hour, &tm->tm_min, &tm->tm_sec) != 6) {
        return NULL; // Parsing failed
    }

    // Adjust values for struct tm (years since 1900 and months 0-11).
    tm->tm_year -= 1900;
    tm->tm_mon -= 1;

    return (char *)(s + strlen(s));
}

//This function will calculates the total system uptime based on the "System startup" and current time.
void calculate_uptime(const char *file_path)
{
    struct tm startup_time = {0}, shutdown_time = {0};
    char line[200],log_level[200], time_stamp[200], message[200];
    int startup_found = 0, shutdown_found = 0;

    //First open file from file path in read mode.
    FILE *file;
    file = fopen(file_path,"r");
    if(file == NULL)
    {
        printf("Error: Error opening file");
        return;
    }

     //Read the file line by line then store it in the line array.
    while(fgets(line, sizeof(line), file))
    {
        //Extracts the time_stamp, log_level, and message from each line based on the format provided.
        sscanf(line, "%19[^\t] %9s %[^\n]", time_stamp, log_level, message);
        //Check if the string "System startup" exists in the message.
        if (strstr(message, "System startup") && !startup_found)
        {
            //If the string "System startup" exists in the message and startup_found is still 0, It will parses the time_stamp string and convert it into a struct tm
            //using the specified format(%Y-%m-%d %H:%M:%S). This format expects the timestamp to be in format YYYY-MM-DD HH:MM:SS
            custom_strptime(time_stamp, "%Y-%m-%d %H:%M:%S", &startup_time);
            startup_found = 1;
        }
        //Check if the string "System shutdown" exists in the message.
        if (strstr(message, "System shutdown"))
        {
            //If the string "System shutdown" exists in the message, It will parses the time_stamp string and convert it into a struct tm using the custom custom_strptime() function.
            custom_strptime(time_stamp, "%Y-%m-%d %H:%M:%S", &shutdown_time);
            shutdown_found = 1;
        }
    }
    fclose(file);

    //If no shutdown message was found, the system is assumed to still be running.
    if(shutdown_found == 0)
    {
        //Retrieves the current system time using time() function
        time_t cur_time = time(NULL);
        //Convert the current time into a struct tm.
        shutdown_time = *localtime(&cur_time);
    }

    //Calculate Uptime.
    time_t start_time = mktime(&startup_time);
    time_t end_time = mktime(&shutdown_time);
    double uptime_seconds = difftime(end_time, start_time);

    int hours = (int)(uptime_seconds / 3600);
    int minutes = (int)((uptime_seconds - hours * 3600) / 60);
    int seconds = (int)(uptime_seconds - hours * 3600 - minutes * 60);

    printf("System uptime: %02d:%02d:%02d\n", hours, minutes, seconds);
    printf("\n");
}

void show_help(char *argv[])
{
    printf("Usage: %s --file <file_path> [OPTIONS]\n", argv[0]);
    printf("Options:\n");
    printf("  --file <file_path>   Path to the log file\n");
    printf("  --count-levels       Count log messages by log level (INFO, WARNING, ERROR)\n");
    printf("  --list <level>       List all level messages\n");
    printf("  --uptime             Calculate the total system uptime\n");
    printf("  --help               Display this help message\n");
}
