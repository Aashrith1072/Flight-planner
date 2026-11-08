#include <bits/stdc++.h>
using namespace std;
#pragma once

// Big numbers that mean "not reached yet"
const int INF_LEVEL = INT_MAX;
const long long INF_COST = LLONG_MAX;

class Flight {
public:
    // ---- Data given in the question ----
    int flight_no;        // unique ID of the flight
    int start_city;       // city where the flight starts
    int departure_time;   // time when the flight leaves
    int end_city;         // city where the flight lands
    int arrival_time;     // time when the flight lands
    int fare;             // price of the flight

    // ---- Extra notes used while searching ----
    int parent;           // POSITION of the flight taken just before this one (-1 = none)
    int level2;           // number of flights used so far (Route 1)
    int level;            // number of flights used so far (Route 3)
    long long cost;       // total price so far (Route 2 and Route 3)
    bool visited;         // have we already used this flight?

    Flight(int fli_no, int start, int departure,
           int end, int arrival, int fr) {
        flight_no = fli_no;
        start_city = start;
        departure_time = departure;
        end_city = end;
        arrival_time = arrival;
        fare = fr;
        parent = -1;
        level2 = 0;
        level = INF_LEVEL;
        cost = INF_COST;
        visited = false;
    }
};
