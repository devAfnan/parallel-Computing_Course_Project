#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// Compares two strings 
int my_strcasecmp(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        char c1 = tolower((unsigned char)*s1);
        char c2 = tolower((unsigned char)*s2);
        if (c1 != c2)
            return c1 - c2;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// Checks if the string contains digits only 
int is_digits_only(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

// Checks if the string contains letters only 
int is_letters_only(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (!isalpha((unsigned char)str[i])) return 0;
    }
    return 1;
}

// Checks if the string represents a valid decimal number 
int is_valid_decimal(const char* str) {
    int dot_count = 0;
    int digit_count = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] == '.') {
            dot_count++;
            if (i == 0 || str[i + 1] == '\0') return 0; // Starts or ends with dot
        }
        else if (isdigit((unsigned char)str[i])) {
            digit_count++;
        }
        else {
            return 0; // Invalid character
        }
    }
    return (dot_count == 1 && digit_count > 0);
}

// Checks whether a number is prime.
int isPrime(int num) {
    if (num <= 1) return 0;
    for (int i = 2; i <= (int)sqrt(num); i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char** argv) {
    int rank, size;
    int N;
    char continue_flag[10] = "yes";

    double start_time, end_time, local_elapsed, max_elapsed;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    do {
        if (rank == 0) {
            if (my_strcasecmp(continue_flag, "yes") == 0) {
                char input[100];
                // Input validation loop
                while (1) {
                    printf("Enter the value of N (or type 'exit' to quit): ");
                    fflush(stdout);
                    fgets(input, sizeof(input), stdin);
                    input[strcspn(input, "\n")] = '\0';

                    if (my_strcasecmp(input, "exit") == 0) {
                        snprintf(continue_flag, sizeof(continue_flag), "exit");
                        break;
                    }
                    // Handle different types of invalid inputs
                    if (strlen(input) == 0) {
                        printf("You did not type anything! Please try again.\n");
                    }
                    else if (is_valid_decimal(input)) {
                        printf("Decimals are not allowed! Please enter a whole number.\n");
                    }
                    else if (is_letters_only(input)) {
                        printf("Letters are not valid input! Please enter numbers only.\n");
                    }
                    // Handle the case for negative numbers
                    else if (input[0] == '-' && is_digits_only(input + 1)) {
                        printf("Negative numbers are not allowed! Please enter a positive whole number.\n");
                    }
                    else if (is_digits_only(input)) {
                        N = atoi(input);
                        // Handle the case for 0 or 1
                        if (N < 2) {
                            printf("There are no prime numbers less than 2! Try a bigger number.\n");
                        }
                        else {
                            break; // Valid number entered
                        }
                    }
                    else {
                        printf("Symbols or mixed characters are not allowed! Please enter a valid number.\n");
                    }
                }
            }
        }

        MPI_Bcast(continue_flag, 10, MPI_CHAR, 0, MPI_COMM_WORLD);
        if (my_strcasecmp(continue_flag, "exit") == 0) {
            if (rank == 0) printf("See you next time...!\n");
            MPI_Finalize();
            return 0; // Exit the program
        }

        // Broadcast the value of N from process 0 to all other processes
        MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);// Synchronize all processes
        start_time = MPI_Wtime();

        // Partitioning N among the processes
        int range_start = 2 + rank * (N - 1) / size;
        int range_end = 2 + (rank + 1) * (N - 1) / size - 1;
        if (range_end > N) range_end = N;

        int* local_primes = (int*)malloc((N - 1) * sizeof(int));
        int local_count = 0;

        // Each process calculates the prime numbers in its assigned range
        for (int i = range_start; i <= range_end; i++) {
            if (isPrime(i)) {
                local_primes[local_count++] = i;
            }
        }

        end_time = MPI_Wtime();
        local_elapsed = end_time - start_time;

        int* recv_counts = NULL;
        if (rank == 0) {
            recv_counts = (int*)malloc(size * sizeof(int));
        }
        // Gather all local prime numbers 
        MPI_Gather(&local_count, 1, MPI_INT, recv_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int* displs = NULL;
        int total_primes = 0;
        int* all_primes = NULL;

        if (rank == 0) {
            displs = (int*)malloc(size * sizeof(int));
            displs[0] = 0;
            for (int i = 1; i < size; i++) {
                displs[i] = displs[i - 1] + recv_counts[i - 1];
            }
            total_primes = displs[size - 1] + recv_counts[size - 1];
            all_primes = (int*)malloc(total_primes * sizeof(int));
        }
        //Gather local primes in unequal quantities 
        MPI_Gatherv(local_primes, local_count, MPI_INT,
            all_primes, recv_counts, displs, MPI_INT,
            0, MPI_COMM_WORLD);

        MPI_Reduce(&local_elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        //Print max elapsed time and the list of prime numbers up to N
        if (rank == 0) {
            printf("\nPrime numbers up to %d:\n[", N);
            for (int i = 0; i < total_primes; i++) {
                printf("%d", all_primes[i]);
                if (i < total_primes - 1) printf(", ");
            }
            printf("]\n");
            printf("Max Elapsed time = %f seconds\n", max_elapsed);

            free(recv_counts);
            free(displs);
            free(all_primes);

            // Ask user if they want to try again
            printf("Try another number? (yes/no): ");
            while (1) {
                fflush(stdout);
                fgets(continue_flag, sizeof(continue_flag), stdin);
                continue_flag[strcspn(continue_flag, "\n")] = '\0';

                if (my_strcasecmp(continue_flag, "yes") == 0 ||
                    my_strcasecmp(continue_flag, "no") == 0) {
                    break;
                }
                else {
                    printf("Invalid input! Please type ('yes' or 'no'): ");
                }
            }
        }

        MPI_Bcast(continue_flag, 10, MPI_CHAR, 0, MPI_COMM_WORLD);
        free(local_primes);

    } while (my_strcasecmp(continue_flag, "no") != 0 && my_strcasecmp(continue_flag, "exit") != 0);

    // Exit message
    if (rank == 0) {
        printf("Thank you for using The Program. Hope you come back soon...!\n");
    }

    MPI_Finalize();
    return 0;
}
