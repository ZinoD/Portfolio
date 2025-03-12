#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 10
#define MAX_LINES 4
#define MAX_EMPLOYEES 10

// Define a structure to represent log entry
typedef struct {
    int product_id;
    int line_id;
    int issue_code;
    int day;
    int month;
    int year;
    int hour;
    int minute;
    char employee[MAX_EMPLOYEES][20];
    char issue_description[50];
    char resolution_description[50];
} LogEntry;

// Compare function for merge sort
int compare(const void *a, const void *b) {
    const LogEntry *entry1 = (const LogEntry *)a;
    const LogEntry *entry2 = (const LogEntry *)b;

    // Compare dates
    if (entry1->year != entry2->year) return entry1->year - entry2->year;
    if (entry1->month != entry2->month) return entry1->month - entry2->month;
    if (entry1->day != entry2->day) return entry1->day - entry2->day;

    // Compare times
    if (entry1->hour != entry2->hour) return entry1->hour - entry2->hour;
    return entry1->minute - entry2->minute;
}

// Function to sort logs by date & time
void sortLogs(LogEntry *logs, int num_logs) {
    qsort(logs, num_logs, sizeof(LogEntry), compare);
}

// Function to report issue codes by product ID and line ID, sorted by line and product ID
void reportIssues(LogEntry *logs, int num_logs) {
    // Sort logs by date & time
    sortLogs(logs, num_logs);

    // Print the report header
    printf("Issue Reports by Product ID and Line ID:\n");

    // Initialize hash map for product_id and line_id
    int issue_map[MAX_PRODUCTS][MAX_LINES] = {0};

    // Iterate through logs to populate the map
    for (int i = 0; i < num_logs; i++) {
        issue_map[logs[i].product_id][logs[i].line_id] = logs[i].issue_code;
    }

    // Print the report
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        for (int j = 0; j < MAX_LINES; j++) {
            if (issue_map[i][j] != 0) {
                printf("Product ID: %d, Line ID: %d, Issue Code: %d\n",
                       i + 1, j + 1, issue_map[i][j]);
            }
        }
    }
    printf("\n");
}

// Function to find the earliest occurrence of an issue code for a given product ID across all lines
int findEarliestIssue(LogEntry *logs, int num_logs, int product_id) {
    int earliest_time = -1;

    // Iterate through logs to find the earliest occurrence
    for (int i = 0; i < num_logs; i++) {
        if (logs[i].product_id == product_id && (earliest_time == -1 || compare(&logs[i], &logs[earliest_time]) < 0)) {
            earliest_time = i;
        }
    }

    return earliest_time;
}

// Function to summarize the number of issues reported for each product across all lines
void summarizeIssues(LogEntry *logs, int num_logs) {
    // Sort logs by date & time
    sortLogs(logs, num_logs);

    // Initialize hash map for product_id and count of issues
    int issue_count[MAX_PRODUCTS] = {0};

    // Iterate through logs to count the occurrences of issues
    for (int i = 0; i < num_logs; i++) {
        issue_count[logs[i].product_id]++;
    }

    // Print the summary header
    printf("Issue Summary by Product ID:\n");

    // Print the summary
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        printf("Product ID: %d, Number of Issues: %d\n", i + 1, issue_count[i]);
    }
    printf("\n");

    // Print the detailed summary
    printf("Detailed Issue Summary:\n");
    for (int i = 0; i < num_logs; i++) {
        printf("Product ID: %d, Issue Code: %d, Date: %02d/%02d/%02d, Time: %02d:%02d, Employees: ",
               logs[i].product_id, logs[i].issue_code, logs[i].day, logs[i].month, logs[i].year, logs[i].hour, logs[i].minute);
        for (int j = 0; j < MAX_EMPLOYEES; j++) {
            if (strlen(logs[i].employee[j]) > 0) {
                printf("%s, ", logs[i].employee[j]);
            }
        }
        printf(", Issue Description: %s, Resolution Description: %s\n",
               logs[i].issue_description, logs[i].resolution_description);
    }
}

int main() {
    // Sample logs data
    LogEntry logs[] = {
        {1, 1, 101, 1, 4, 24, 12, 30, {"Max"}, "Leak", "Cap replaced"},  // product_id, line_id, issue_code, day, month, year, hour, minute, employee, issue_description, resolution_description
        {2, 2, 102, 2, 4, 24, 10, 15, {"Ethan"}, "Faulty circuit", "Circuit fixed"},
        {3, 3, 103, 3, 4, 24, 8, 45, {"Max"}, "Broken seal", "Seal fixed"},
        {1, 2, 101, 4, 4, 24, 9, 0, {"Ethan"}, "Leak", "Cap tightened"},
        // More log entries can be added as data exapnds
    };
    int num_logs = sizeof(logs) / sizeof(logs[0]);

    // Task 1: Sort the logs by date & time and display
    printf("Task 1: Sort the logs by date & time:\n");
    sortLogs(logs, num_logs);
    for (int i = 0; i < num_logs; i++) {
        printf("Date: %02d/%02d/%02d, Time: %02d:%02d, Product ID: %d, Line ID: %d, Issue Code: %d\n",
               logs[i].day, logs[i].month, logs[i].year, logs[i].hour, logs[i].minute,
               logs[i].product_id, logs[i].line_id, logs[i].issue_code);
    }
    printf("\n");

    // Task 2: Report issue codes by product ID and line ID, sorted by line and product ID
    printf("Task 2: Report issue codes by product ID and line ID,:\n");
    reportIssues(logs, num_logs);

    // Task 3: Find the earliest occurrence of an issue code for a given product ID
    int product_id_to_search;
    printf("Task 3: Find the earliest occurrence of an issue code for a given product ID:\n");
    printf("Enter Product ID to search for earliest occurrence of an issue code: ");
    if (scanf("%d", &product_id_to_search) != 1) {
        printf("Invalid input. Please enter a valid Product ID.\n");
        return 1;
    }
    int earliest_index = findEarliestIssue(logs, num_logs, product_id_to_search);
    if (earliest_index != -1) {
        printf("Earliest occurrence of an issue code for Product ID %d: Issue Code %d at Date: %02d/%02d/%02d Time: %02d:%02d\n",
               product_id_to_search, logs[earliest_index].issue_code, logs[earliest_index].day, logs[earliest_index].month, logs[earliest_index].year, logs[earliest_index].hour, logs[earliest_index].minute);
    } else {
        printf("No issue code found for Product ID %d\n", product_id_to_search);
    }
    printf("\n");

    // Task 4: Summarize the number of issues reported for each product across all lines
    printf("Task 4: Summarize the number of issues reported for each product across all lines:\n");
    summarizeIssues(logs, num_logs);

    return 0;
}
2