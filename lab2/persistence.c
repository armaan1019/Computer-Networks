/*
* Name: Armaan Sharma
* Date: Jan 13, 2026
* Title: Lab2 – HTTP connection time calculations
* Description: This program computes the time required for HTTP requests under different scenarios.
*/

#include <stdio.h>

int main() {
    // Given RTT values (in milliseconds)
    double RTT0 = 3;       // Local DNS cache
    double RTT1 = 20;      // Second DNS server
    double RTT2 = 26;      // Third DNS server
    double RTTHTTP = 47;   // RTT to Web server

    int num_objects = 6;   // additional objects
    int n;                 // parallel connections

    // DNS lookup time
    double DNS_time = RTT0 + RTT1 + RTT2;

    // Part (a): One object
    double time_a = DNS_time + 2 * RTTHTTP; // DNS + TCP connection + HTTP request/response

    printf("Part (a):\n");
    printf("Time to receive single object = %.2f ms\n\n", time_a);

    // Part (b): Non-persistent HTTP, no parallel connections
    double time_b = DNS_time + 2 * RTTHTTP * (1 + num_objects); // base object + additional objects

    printf("Part (b):\n");
    printf("Time to receive base object + %d objects (non-persistent, no parallel) = %.2f ms\n\n",
           num_objects, time_b);

    // Part (c): Parallel connections
    printf("Enter number of parallel TCP connections (n): ");
    scanf("%d", &n);

    // Non-persistent HTTP with n parallel connections
    double rounds = (double)num_objects / n;
    if (num_objects % n != 0) {
        rounds = (int)rounds + 1; // round up if not an integer
    }

    double time_c_nonpersistent = DNS_time + 2 * RTTHTTP + 2 * RTTHTTP * rounds; // TCP and http for base and objects

    // Persistent HTTP with n parallel connections
    double time_c_persistent = DNS_time + RTTHTTP + RTTHTTP + RTTHTTP * rounds; // singular TCP connection

    printf("\nPart (c):\n");
    printf("Non-persistent HTTP with %d parallel connections = %.2f ms\n",
           n, time_c_nonpersistent);

    printf("Persistent HTTP with %d parallel connections = %.2f ms\n",
           n, time_c_persistent);

    return 0;
}
