#include <stdio.h>

#include <stdlib.h>

#include <string.h>



#define MAX_LENGTH 100



// Function to input workout details

void inputWorkout(FILE *file) {

    char workout[MAX_LENGTH];

    printf("Enter your workout for the day: ");

    getchar();  // To clear the buffer

    fgets(workout, MAX_LENGTH, stdin);

    workout[strcspn(workout, "\n")] = 0;  // Remove newline character if present



    fprintf(file, "Workout Plan: %s\n", workout);

    printf("Workout plan saved.\n");

}



// Function to input diet details

void inputDiet(FILE *file) {

    char diet[MAX_LENGTH];

    printf("Enter your diet for the day: ");

    getchar();  // To clear the buffer

    fgets(diet, MAX_LENGTH, stdin);

    diet[strcspn(diet, "\n")] = 0;  // Remove newline character if present



    fprintf(file, "Diet Plan: %s\n", diet);

    printf("Diet plan saved.\n");

}



// Function to display the saved plans

void displayPlans() {

    FILE *file = fopen("plan.txt", "r");

    if (file == NULL) {

        printf("No plans saved yet.\n");

        return;

    }



    char ch;

    printf("\nSaved Plans:\n");

    while ((ch = fgetc(file)) != EOF) {

        putchar(ch);

    }

    fclose(file);

}



// Function to write workout and diet plans to file

void savePlans() {

    FILE *file = fopen("plan.txt", "a");

    if (file == NULL) {

        printf("Error opening file!\n");

        return;

    }



    inputWorkout(file);

    inputDiet(file);



    fclose(file);

}



int main() {

    int choice;



    while (1) {

        printf("\n--- Workout and Diet Planner ---\n");

        printf("1. Input new workout and diet plan\n");

        printf("2. Display saved plans\n");

        printf("3. Exit\n");

        printf("Enter your choice: ");

        scanf("%d", &choice);



        switch (choice) {

            case 1:

                savePlans();

                break;

            case 2:

                displayPlans();

                break;

            case 3:

                printf("Exiting program.\n");

                return 0;

            default:

                printf("Invalid choice. Please try again.\n");

        }

    }



    return 0;

}