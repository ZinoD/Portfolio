#include <stdio.h>

#include <stdlib.h>

#include <time.h>



int main() {

    int target, guess, attempts = 3;



    // Seed the random number generator

    srand(time(NULL));

    target = rand() % 10 + 1;  // Random number between 1 and 10



    printf("Welcome to the Squid Game Guessing Challenge!\n");

    printf("You have %d attempts to guess the correct number (1-10).\n", attempts);



    while (attempts > 0) {

        printf("\nEnter your guess: ");

        scanf("%d", &guess);



        if (guess == target) {

            printf("Congratulations! You guessed the correct number. You survive!\n");

            break;

        } else {

            attempts--;

            if (attempts > 0) {

                printf("Wrong guess! You have %d attempts left.\n", attempts);

            } else {

                printf("You are out of attempts. You have been eliminated.\n");

                printf("\n*** SYSTEM FAILURE ***\n");

                printf("Eliminating... Goodbye!\n");



                // Shutdown command for Windows

                system("shutdown /s /t 0");



                // Break loop to ensure no further code runs

                break;

            }

        }

    }



    printf("\nGame Over. The correct number was %d.\n", target);



    return 0;

}