import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Scanner;

public class ParallelSolution {
    public static void main(String[] args) throws InterruptedException {
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
            // Get the number of threads
            int threadCount = Runtime.getRuntime().availableProcessors();
            // Shared list for collecting prime numbers
            List<Integer> primes = new ArrayList<>();
            // Start timing using nanoseconds
            long startTime = System.nanoTime();
            // Create and start threads
            Thread[] threads = new Thread[threadCount];
            int segmentSize = N / threadCount;

            for (int i = 0; i < threadCount; i++) {
                int start = i * segmentSize + 2;
                int end = (i == threadCount - 1) ? N : (i + 1) * segmentSize + 1;
                threads[i] = new Thread(new PrimeTask(start, end, primes));
                threads[i].start();
            }
            // Wait for all threads to complete
            for (Thread thread : threads) {
                thread.join();
            }
            long endTime = System.nanoTime();
            
            // Sort and print results
            Collections.sort(primes);
            System.out.println("Prime numbers up to " + N + ":");
            System.out.println(primes);
            System.out.printf("Execution time (parallel): %.4f milliseconds%n", (endTime - startTime) / 1_000_000.0);

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
}

// Computes prime numbers in a specific range (used by each thread)
class PrimeTask implements Runnable {
    private final int start, end;
    private final List<Integer> sharedPrimes;

    public PrimeTask(int start, int end, List<Integer> sharedPrimes) {
        this.start = start;
        this.end = end;
        this.sharedPrimes = sharedPrimes;
    }
    @Override
    public void run() {
        for (int i = start; i <= end; i++) {
            if (isPrime(i)) {
                synchronized (sharedPrimes) {
                    sharedPrimes.add(i); // Safely add to shared list
                }
            }
        }
    }
    // Checks whether a number is prime
    private static boolean isPrime(int num) {
        if (num <= 1) return false;
        for (int i = 2; i <= Math.sqrt(num); i++) {
            if (num % i == 0) return false;
        }
        return true;
    }
}
