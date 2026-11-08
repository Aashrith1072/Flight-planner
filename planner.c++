#include <bits/stdc++.h>
using namespace std;
#pragma once
#include "flight.c++"

// =====================================================================
// Queue : first in, first out (used by BFS in Route 1)
// It stores flight POSITIONS (ints)
// =====================================================================
class Queue {
private:
    vector<int> items;
    int head;   // position of the front item which starts from left and moves 
    // to the right accordingly as we do dequeue and peek just gives the element at that position head and does not do anything there.     

public:
    Queue(){
       head=0; 
    }

    void enqueue(int item) {
        items.push_back(item);
    }

    int dequeue() {
        if (is_empty()){
            //stops the program at this line and gives there is an error instead of running with out of bound values.
            //outofrange is a class throw stops the code from going to next line and it prints error.
            throw out_of_range("dequeue from an empty queue which is not possible");
        }
        int a = items[head];
        head++;
        return a;
    }

    bool is_empty() {
        return head == items.size();
    }

    int peek() {
        if (is_empty()){
            throw out_of_range("peek from an empty queue");
        }
        return items[head];
    }

    int size() {
        return items.size() - head;
    }
};


// =====================================================================
// Entry : one box stored inside the heap
// (flight position + cost to reach it + number of flights used)
// =====================================================================
struct Entry {
    int flight;
    long long cost;
    int level;
};


// =====================================================================
// Heap : a min-heap. YOU decide who is "smaller" using comparison_function
// =====================================================================
class Heap {
private:
    function<bool(const Entry&, const Entry&)> comparison_function;
    vector<Entry> heap;

    int _parent(int index) {
        if(index>0){
            return (index-1)/2;
        }else{
            return -1;
        }
    }

    void _heapify_down(int index) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;
        int n =heap.size();
        
        if (left < n && comparison_function(heap[left], heap[smallest])) smallest = left;
        if (right < n && comparison_function(heap[right], heap[smallest])) smallest = right;

        if (smallest != index) {
            swap(heap[index], heap[smallest]);
            _heapify_down(smallest);
        }
    }

    void _heapify_up(int index) {
        int parent = _parent(index);
        if (parent != -1 && comparison_function(heap[index], heap[parent])) {
            swap(heap[index], heap[parent]);
            _heapify_up(parent);
        }
    }

public:
    Heap(function<bool(const Entry&, const Entry&)> comparison_function,
         vector<Entry> init_array) {
        this->comparison_function = comparison_function;
        this->heap = init_array;
        int n = (int)heap.size();
        for (int i = n / 2 - 1; i >= 0; i--) _heapify_down(i);
    }

    void insert(Entry value) {
        heap.push_back(value);
        _heapify_up((int)heap.size() - 1);
    }

    Entry extract() {
        if (heap.empty()) throw out_of_range("Extract from an empty heap");
        Entry top_value = heap[0];
        Entry last_value = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heap[0] = last_value;
            _heapify_down(0);
        }
        return top_value;
    }

    Entry top() {
        if (heap.empty()) throw out_of_range("Top from an empty heap");
        return heap[0];
    }

    bool is_empty() {
        return heap.empty();
    }
};


// =====================================================================
// Planner : the three route finders
// A route is returned as a list of flight POSITIONS (positions in the
// flights list given to the Planner)
// =====================================================================
class Planner {
private:
    vector<Flight> flights;            // the flights themselves (our own copy)
    vector<vector<int>> cities;        // cities[c] = positions of flights that LEAVE city c

    bool in_range(int city) {
        return city >= 0 && city < cities.size();
    }

    // Walk back using parent positions and build the route
    vector<int> build_route(int last) {
        vector<int> route;
        for (int cur = last; cur != -1; cur = flights[cur].parent) {
            route.push_back(cur);
        }
        reverse(route.begin(), route.end());
        return route;
    }

public:
    // O(m): group flights by the city they leave from
    Planner(vector<Flight> fght) {

        flights = fght;
        int max_city = -1;
        for (int i = 0; i < flights.size(); i++) {
            max_city = max(max_city, max(flights[i].start_city, flights[i].end_city));
        }
        cities.resize(max_city + 1);
        for (int i = 0; i < flights.size(); i++) {
            cities[flights[i].start_city].push_back(i);
        }
    }

    // Print flights city by city (for debugging)
    void check() {
        for (int c = 0; c < (int)cities.size(); c++) {
            cout << c << "\n";
            for (int id : cities[c]) {
                cout << flights[id].flight_no << " " << flights[id].start_city << " "
                     << flights[id].departure_time << " " << flights[id].end_city << " "
                     << flights[id].arrival_time << " " << flights[id].fare << "\n";
            }
        }
    }

    // ------------------------------------------------------------------
    // ROUTE 1 : fewest flights, then earliest arrival   (BFS, O(m))
    // ------------------------------------------------------------------
   
    vector<int> least_flights_earliest_route(int start_city, int end_city, int t1, int t2) {

        if (in_range(start_city) == false) return {}; // for out of bounds checking wriiten above 
        Queue q;
        // Reset all flights
        for (int i = 0; i < flights.size(); i++) {
            flights[i].visited = false;
            flights[i].level2 = 0;
            flights[i].parent = -1;
        }

        // Starting flights are level 1
        for (int id : cities[start_city]) {
            if (flights[id].departure_time >= t1 && flights[id].arrival_time <= t2) {
                flights[id].level2 = 1;
                flights[id].parent = -1;
                flights[id].visited = true;
                q.enqueue(id);
            }
        }

        int best_route = -1;
        int min_flights = INF_LEVEL;
        int earliest_arrival = INT_MAX;

        while (!q.is_empty()) {
            int current = q.dequeue();

            // Reached the destination: is it better than the best so far?
            if (flights[current].end_city == end_city) {
                if (flights[current].level2 < min_flights ||
                    (flights[current].level2 == min_flights &&
                     flights[current].arrival_time < earliest_arrival)) {
                    min_flights = flights[current].level2;
                    earliest_arrival = flights[current].arrival_time;
                    best_route = current;
                }
            }

            // Longer routes cannot win, so stop
            if (flights[current].level2 > min_flights) break;

            // Try all connecting flights
            int connection_time = flights[current].arrival_time + 20;
            for (int next : cities[flights[current].end_city]) {
                if (!flights[next].visited &&
                    flights[next].departure_time >= connection_time &&
                    flights[next].arrival_time <= t2) {
                    flights[next].level2 = flights[current].level2 + 1;
                    flights[next].parent = current;
                    flights[next].visited = true;
                    q.enqueue(next);
                }
            }
        }
        return build_route(best_route);
    }

    // ------------------------------------------------------------------
    // ROUTE 2 : cheapest total price   (Dijkstra, O(m log m))
    // ------------------------------------------------------------------
    vector<int> cheapest_route(int start_city, int end_city, int t1, int t2) {

        if (!in_range(start_city)) return {};

        // "smaller" = lower cost
        auto comparison_function = [](const Entry& a, const Entry& b) {
            return a.cost < b.cost;
        };

        Heap heap(comparison_function, {});

        // Reset all flights
        for (int i = 0; i < flights.size(); i++) {
            flights[i].visited = false;
            flights[i].cost = INF_COST;
            flights[i].parent = -1;
        }

        // Starting flights
        for (int id : cities[start_city]) {
            if (flights[id].departure_time >= t1 && flights[id].arrival_time <= t2) {
                flights[id].cost = flights[id].fare;
                flights[id].parent = -1;
                heap.insert({id, flights[id].fare, 0});
            }
        }

        int ans = -1;
        long long cheap = INF_COST;

        while (!heap.is_empty()) {
            Entry e = heap.extract();
            int flight = e.flight;
            long long cost = e.cost;

            flights[flight].visited = true;   // its cost is final now

            if (flights[flight].end_city == end_city) {
                if (cost < cheap) {
                    ans = flight;
                    cheap = cost;
                }
            } else {
                for (int next : cities[flights[flight].end_city]) {
                    if (!flights[next].visited &&
                        flights[next].departure_time >= flights[flight].arrival_time + 20 &&
                        flights[next].arrival_time <= t2) {
                        long long new_cost = cost + flights[next].fare;
                        if (new_cost < flights[next].cost) {
                            flights[next].cost = new_cost;
                            flights[next].parent = flight;
                            heap.insert({next, new_cost, 0});
                        }
                    }
                }
            }
        }

        return build_route(ans);
    }

    // ------------------------------------------------------------------
    // ROUTE 3 : fewest flights, then cheapest   (Dijkstra on (flights, price), O(m log m))
    // ------------------------------------------------------------------
    vector<int> least_flights_cheapest_route(int start_city, int end_city, int t1, int t2) {
        if (!in_range(start_city)) return {};

        // "smaller" = fewer flights; if equal, lower cost
        auto comparison_function1 = [](const Entry& a, const Entry& b) {
            if (a.level < b.level) return true;
            if (a.level == b.level && a.cost < b.cost) return true;
            return false;
        };
        Heap heap(comparison_function1, {});

        // Reset all flights
        for (int i = 0; i < (int)flights.size(); i++) {
            flights[i].visited = false;
            flights[i].level = INF_LEVEL;
            flights[i].cost = INF_COST;
            flights[i].parent = -1;
        }

        // Starting flights are level 1
        for (int id : cities[start_city]) {
            if (flights[id].departure_time >= t1 && flights[id].arrival_time <= t2) {
                flights[id].level = 1;
                flights[id].cost = flights[id].fare;
                flights[id].parent = -1;
                flights[id].visited = true;
                heap.insert({id, (long long)flights[id].fare, 1});
            }
        }

        int ans = -1;
        int ans_level = INF_LEVEL;
        long long cheap = INF_COST;

        while (!heap.is_empty()) {
            Entry e = heap.extract();
            int flight = e.flight;
            long long cost = e.cost;
            int level = e.level;

            // Already have an answer with fewer flights: stop
            if (level > ans_level) break;

            if (flights[flight].end_city == end_city) {
                if (level < ans_level || (level == ans_level && cost < cheap)) {
                    ans = flight;
                    cheap = cost;
                    ans_level = level;
                }
            } else {
                for (int next : cities[flights[flight].end_city]) {
                    if (!flights[next].visited &&
                        flights[next].departure_time >= flights[flight].arrival_time + 20 &&
                        flights[next].arrival_time <= t2) {
                        long long new_cost = cost + flights[next].fare;
                        int new_level = level + 1;

                        if (new_level < flights[next].level ||
                            (new_level == flights[next].level && new_cost < flights[next].cost)) {
                            flights[next].level = new_level;
                            flights[next].cost = new_cost;
                            flights[next].parent = flight;
                            flights[next].visited = true;   // first time we reach it is the best time
                            heap.insert({next, new_cost, new_level});
                        }
                    }
                }
            }
        }

        return build_route(ans);
    }
};
