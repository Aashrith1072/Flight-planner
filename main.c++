#include <bits/stdc++.h>
using namespace std;
#include "flight.c++"
#include "planner.c++"

// Print a route as positions, e.g. [1, 5]
void print_route(const vector<int>& route) {
    cout << "[";
    for (int i = 0; i < (int)route.size(); i++) {
        if (i) cout << ", ";
        cout << route[i];
    }
    cout << "]\n";
}

int main() {
    vector<Flight> flights = {
        Flight(0, 0, 0,   1, 30,  50),    // City 0 to 1
        Flight(1, 0, 0,   3, 80,  200),   // City 0 to 3

        Flight(2, 1, 40,  2, 60,  20),    // City 1 to 2
        Flight(3, 1, 50,  2, 100, 120),   // City 1 to 2

        Flight(4, 2, 120, 4, 200, 100),   // City 2 to 4

        Flight(5, 3, 100, 4, 150, 500),   // City 3 to 4
        Flight(6, 3, 100, 4, 250, 300)    // City 3 to 4
    };

    Planner flight_planner = Planner(flights);

    // The three tasks (each answer is a list of flight positions)
    vector<int> route1 = flight_planner.least_flights_earliest_route(0, 4, 0, 300);      
    vector<int> route2 = flight_planner.cheapest_route(0, 4, 0, 300);
    vector<int> route3 = flight_planner.least_flights_cheapest_route(0, 4, 0, 300);

    // Model output
    vector<int> expected_route1 = {1, 5};      // 0-3-4, 2 flights, lands at t=150
    vector<int> expected_route2 = {0, 3, 4};   // 0-1-2-4, fare 270
    vector<int> expected_route3 = {1, 6};      // 0-3-4, 2 flights, fare 500

    if (route1 == expected_route1) {
        cout << "Task 1 PASSED\n";
    } else {
        cout << "Task 1 FAILED\nExpected: "; print_route(expected_route1);
        cout << "Got: "; print_route(route1);
    }

    if (route2 == expected_route2) {
        cout << "Task 2 PASSED\n";
    } else {
        cout << "Task 2 FAILED\nExpected: "; print_route(expected_route2);
        cout << "Got: "; print_route(route2);
    }

    if (route3 == expected_route3) {
        cout << "Task 3 PASSED\n";
    } else {
        cout << "Task 3 FAILED\nExpected: "; print_route(expected_route3);
        cout << "Got: "; print_route(route3);
    }

    return 0;
}
