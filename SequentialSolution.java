import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class SequentialSolution {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        String continueChoice;

        do {
            int N = -1;
            // Input validation loop
            while (true) {
                System.out.print("Enter the value of N (or type 'exit' to quit): ");
                String input = scanner.nextLine().trim();

                if (input.equalsIgnoreCase("exit")) {
                    System.out.println("See you next time...!");
                    scanner.close();
                    return; // Exit the program
                }
                // Handle different types of invalid inputs
                if (input.isEmpty()) {
                    System.out.println("You did not type anything! Please try again.");
                } else if (input.matches("-?\\d+\\.\\d+")) {
                    System.out.println("Decimals are not allowed! Please enter a whole number.");
                } else if (input.matches("[a-zA-Z]+")) {
                    System.out.println("Letters are not valid input! Please enter numbers only.");
                } else if (!input.matches("-?\\d+")) {
                    System.out.println("Symbols or mixed characters are not allowed! Please enter a valid number.");
                } else {
                    N = Integer.parseInt(input);
                    // Check for negative or too-small numbers
                    if (N < 0) {
                        System.out.println("Negative numbers are not allowed! Please enter a positive whole number.");
                    } else if (N < 2) {
                        System.out.println("There are no prime numbers less than 2! Try a bigger number.");
                    } else {
                        break; // Valid number entered
                    }
                }
            }
            // Start timing using nanoseconds
            long startTime = System.nanoTime();  
            List<Integer> primes = computePrimesUpToN(N); // Compute primes
            long endTime = System.nanoTime();    // End timing
            // Output results
            System.out.println("Prime numbers up to " + N + ":");
            System.out.println(primes);
            System.out.printf("Execution time: %.4f milliseconds%n", (endTime - startTime) / 1_000_000.0);

            // Ask user if they want to repeat
            while (true) {
                System.out.print("Try another number? (yes/no): ");
                continueChoice = scanner.nextLine().trim();

                if (continueChoice.equalsIgnoreCase("yes") || continueChoice.equalsIgnoreCase("no")) {
                    break; // Valid input
                } else {
                    System.out.println("Invalid input! Please type ('yes' or 'no'):");
                }
            }

        } while (!continueChoice.equalsIgnoreCase("no") && !continueChoice.equalsIgnoreCase("exit"));

        // Exit message
        System.out.println("Thank you for using The Program. Hope you come back soon...!");
        scanner.close();
    }

     // Computes all prime numbers from 2 up to N
    public static List<Integer> computePrimesUpToN(int N) {
        List<Integer> primeList = new ArrayList<>();
        for (int number = 2; number <= N; number++) {
            if (isPrime(number)) {
                primeList.add(number); // Add if prime
            }
        }
        return primeList;
    }

     // Checks whether a number is prime
    public static boolean isPrime(int num) {
        if (num <= 1) return false; // Not prime
        for (int i = 2; i <= Math.sqrt(num); i++) {
            if (num % i == 0) return false; // Not prime if divisible
        }
        return true; // Prime number
    }
}
