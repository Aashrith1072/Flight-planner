## Flight Planner

A small C++ project that answers one question:

> **"I want to go from city A to city B. There are many flights with different times and prices. Which flights should I take?"**

I built this to understand how flight search really works when many flights overlap like many flights leave the same city, many reach the same city, and every flight has its own time and price.

> **Important:** I did **not** use C++'s ready-made `queue` or `priority_queue`. I wrote my **own `Queue` class** and my **own `Heap` class**, then created objects of them and used their methods.

---

## 1. The Problem (in simple words)

You are given a list of flights. Each flight has:

| Field | Meaning |
|---|---|
| `flight_no` | A unique ID (number) of the flight |
| `start_city` | City where the flight starts |
| `departure_time` | Time when the flight leaves |
| `end_city` | City where the flight lands |
| `arrival_time` | Time when the flight lands |
| `fare` | Price of the ticket |

Cities are numbers (0, 1, 2, ...). Time is a whole number of minutes (0, 1, 2, ...).

You are asked for a trip with these inputs: `start_city`, `end_city`, `t1`, `t2`.

### Rules of the trip

1. You must **leave at or after `t1`**.
2. You must **reach at or before `t2`**.
3. You can take many flights one after another (these are called *connecting flights*).
4. When you change flights in a city, the next flight must leave **at least 20 minutes after** the previous one landed (time to walk, check in, etc.).

### The three tasks

| Task | What we want |
|---|---|
| **Route 1** | **Fewest flights**. If there is a tie, choose the one that **reaches earliest**. |
| **Route 2** | **Cheapest total price**. The number of flights does not matter. |
| **Route 3** | **Fewest flights**. If there is a tie, choose the **cheapest**. |

The question asks for **one** best route (not all of them). If no trip is possible, the answer is an empty list.

### Speed targets

| Part | Target time |
|---|---|
| Building the planner | `O(m)` |
| Route 1 | `O(m)` |
| Route 2 | `O(m log m)` |
| Route 3 | `O(m log m)` |

Here `m` = number of flights.


## 2. An Example to Understand the Question

**Trip:** go from city **0** to city **4**, leave at time **0** or later, and land by time **300**.

| Flight | From → To | Leaves | Lands | Price |
|---|---|---|---|---|
| F0 | 0 → 1 | 0 | 30 | 50 |
| F1 | 0 → 3 | 0 | 80 | 200 |
| F2 | 1 → 2 | 40 | 60 | 20 |
| F3 | 1 → 2 | 50 | 100 | 120 |
| F4 | 2 → 4 | 120 | 200 | 100 |
| F5 | 3 → 4 | 100 | 150 | 500 |
| F6 | 3 → 4 | 100 | 250 | 300 |

### All possible trips from city 0 to city 4

| Trip | Cities | Flights used | Lands at | Total price |
|---|---|---|---|---|
| F1 → F5 | 0 → 3 → 4 | 2 | 150 | 700 |
| F1 → F6 | 0 → 3 → 4 | 2 | 250 | 500 |
| F0 → F3 → F4 | 0 → 1 → 2 → 4 | 3 | 200 | 270 |

(F0 → F2 is **not** allowed: F0 lands at 30 and F2 leaves at 40, which is only 10 minutes. We need 20.)

### The best trip for each question

| Question | Best trip | Why |
|---|---|---|
| **Route 1** (fewest flights, then earliest) | **F1 → F5** | Both F1→F5 and F1→F6 use 2 flights. F5 lands at 150, which is earlier than 250. |
| **Route 2** (cheapest) | **F0 → F3 → F4** | Total price is 270. The others cost 700 and 500. |
| **Route 3** (fewest flights, then cheapest) | **F1 → F6** | Both F1→F5 and F1→F6 use 2 flights. F6 costs 500, which is cheaper than 700. |

---

## 3. The Main Idea: Flights are the Dots

**Think of every flight as a dot (node), not every city.**

Why? Because the same city can be reached at different times and prices. What matters is *which flight you are on*, because that decides when you land and how much you have paid. A city cannot remember that, but a flight can.

### Why not use cities as the dots?

A city dot **cannot remember** *when* you arrived or *how much* you paid. But the same city can be reached in many different ways.

**Example:** In our flights, city 4 can be reached in three different ways.

| Way to reach city 4 | Lands at | Total price |
|---|---|---|
| F1 → F5 | 150 | 700 |
| F1 → F6 | 250 | 500 |
| F0 → F3 → F4 | 200 | 270 |

All three end in the **same city** but with **different times and prices**. A city dot would have to keep only one of them and throw the others away. A flight dot keeps each way separate.


#### Why flights as dots fix this

A flight dot already knows:
- when it leaves and when it lands,
- its price,
- which city it starts and ends in.

So the 20-minute rule is checked on each arrow, and every time and price option stays separate.

city dots forget *when* and *how much*, but flight dots remember both.

We draw an arrow from flight **F1** to flight **F2** when:

- F1 lands in the city where F2 starts, and
- F2 leaves at least 20 minutes after F1 lands, and
- F2 lands on or before `t2`.

### The graph for our example

```
Start flights: F0, F1          End flights (land in city 4): F4, F5, F6

F0 ──► F3 ──► F4
F1 ──► F5
F1 ──► F6

F2 has no arrow coming in, so it can never be used.
```

### Levels (used by BFS)

A **level** means *how many flights you have taken so far*.

| Level | Flights in this level |
|---|---|
| Level 1 | F0, F1 (they leave the start city 0) |
| Level 2 | F3 (after F0), F5 and F6 (after F1) |
| Level 3 | F4 (after F3) |

Now the problem becomes **path-finding on a graph**.

| Task | Tool used | Why |
|---|---|---|
| Route 1 | **BFS** (Breadth First Search) | BFS visits dots level by level, so the first time we reach a dot is with the fewest flights. |
| Route 2 | **Dijkstra** with a **min-heap** | Dijkstra always expands the cheapest option first, so the first time we finish a dot, its price is the lowest. |
| Route 3 | **Dijkstra** comparing **(number of flights, price)** | Compare flights first. If equal, compare price. |

---

## 4. Flights are Referred to by Position

All flights live in one list (`vector<Flight> flights`), and I talk about a flight by its **position** in that list.

| Idea | How it looks in code |
|---|---|
| Flight number 3 | `flights[3]` |
| "Flight just before this one" | `int parent` (a position, `-1` means none) |
| Flights leaving city `c` means flights whose starting city is city c | `cities[c]` (a list of positions) |
| A route | A list of positions, like `{1, 5}` |

### Notes stored inside every flight

Each `Flight` object has 6 **fixed** values from the question and 5 **notes** that the search writes:

| Note | Meaning |
|---|---|
| `parent` | Position of the flight taken just before this one (`-1` = none) |
| `level2` | Number of flights used so far (Route 1) |
| `level` | Number of flights used so far (Route 3) |
| `cost` | Total price so far (Route 2 and Route 3) |
| `visited` | Have we already used this flight? |

The **constructor** (the function that runs when a flight is created) sets these notes to their starting values. Before every new search, the code **resets** these notes, because the previous search left old values in them.

---

## 5. My Own Queue and Heap (not the built-in ones)

C++ already has `queue` and `priority_queue`. I did not use them. Instead I wrote **my own classes**:

- `class Queue` (first in, first out line)
- `class Heap` (a min-heap, a box that always gives the smallest item first)

Then, inside the route functions, I **created objects** of these classes and **used their methods**:

```cpp
Queue q;                                 // make a queue object
q.enqueue(id);                           // add to the back
int current = q.dequeue();               // take from the front

Heap heap(comparison_function, {});      // make a heap object
heap.insert({id, cost, level});          // add an item
Entry e = heap.extract();                // take out the smallest item
```

I did this to learn how these structures really work inside.

### 5.1 How my Queue works

A queue is a line: the first one in is the first one out.

It has two properties:
- `items`: a vector that stores everything that was added.
- `head`: the position of the front item (like a finger pointing at who is next).

**Trick:** when we take an item out, we do **not** erase it. We only move `head` forward by 1. This is instant. (Erasing from the front of a vector is slow, because everything else has to shift.)

Start: `items = []`, `head = 0`

| Step | Action | `items` | `head` | Result |
|---|---|---|---|---|
| 1 | `enqueue(0)` | `[0]` | 0 | |
| 2 | `enqueue(1)` | `[0, 1]` | 0 | |
| 3 | `dequeue()` | `[0, 1]` | 1 | returns **0** |
| 4 | `enqueue(3)` | `[0, 1, 3]` | 1 | |
| 5 | `peek()` | `[0, 1, 3]` | 1 | returns **1** (not removed) |
| 6 | `dequeue()` | `[0, 1, 3]` | 2 | returns **1** |
| 7 | `size()` | `[0, 1, 3]` | 2 | `3 - 2 =` **1** |
| 8 | `dequeue()` | `[0, 1, 3]` | 3 | returns **3** |
| 9 | `is_empty()` | `[0, 1, 3]` | 3 | `head == 3`, so **true** |

The old items 0 and 1 are still inside `items`, but they are behind `head`, so we treat them as gone. Order in: 0, 1, 3. Order out: 0, 1, 3. ✅

| Method | What it does |
|---|---|
| `enqueue(item)` | Add the item at the back. |
| `dequeue()` | Give the item at `head`, then move `head` forward. |
| `peek()` | Show the front item without removing it. |
| `is_empty()` | True when `head` has passed every item. |
| `size()` | `items.size() - head` |

### 5.2 How my Heap works

A heap is a box that always gives you the **smallest** item first. I store it in a vector like a tree:

- Parent of position `i` is `(i - 1) / 2`.
- Children of position `i` are `2i + 1` (left) and `2i + 2` (right).

**You decide who is "smaller"** by passing a **comparison function** when you make the heap:

- Route 2: smaller means **lower cost**.
- Route 3: smaller means **fewer flights**, and if equal, **lower cost**.

That is why one `Heap` class works for both routes.

**Two moves keep the heap in order:**
- **Bubble up** (after `insert`): the new item starts at the end and swaps with its parent while it is smaller than the parent.
- **Bubble down** (after `extract`): the last item is moved to the top, and swaps with its smaller child while it is bigger than that child.

**Example (smaller = lower cost).** Insert the costs 700, 200, 50, 170:

| Step | Action | Heap (as a list) | What happened |
|---|---|---|---|
| 1 | `insert(700)` | `[700]` | First item. |
| 2 | `insert(200)` | `[200, 700]` | 200 is smaller than its parent 700, so they swap. |
| 3 | `insert(50)` | `[50, 700, 200]` | 50 is smaller than its parent 200, so they swap. |
| 4 | `insert(170)` | `[50, 170, 200, 700]` | 170 beats its parent 700 and swaps. It does not beat 50, so it stops. |

Now take items out with `extract()`:

| Step | Action | Heap after | Returns | What happened |
|---|---|---|---|---|
| 5 | `extract()` | `[170, 700, 200]` | **50** | The last item (700) goes to the top, then bubbles down: 170 is the smaller child, so they swap. |
| 6 | `extract()` | `[200, 700]` | **170** | The last item (200) goes to the top. Its child 700 is bigger, so it stays. |
| 7 | `extract()` | `[700]` | **200** | The last item (700) goes to the top. |
| 8 | `extract()` | `[]` | **700** | Last item. |

Items came out as **50, 170, 200, 700**: smallest first. ✅ Each `insert` and `extract` takes `O(log m)` time.

### 5.3 Errors: `throw`

Both classes protect themselves. If you try to take something from an **empty** queue or heap, they stop with an error:

```cpp
int dequeue() {
    if (is_empty()) throw out_of_range("dequeue from an empty queue");
    ...
}
```

| Part | Meaning |
|---|---|
| `throw` | Raise an error and stop this function right here. |
| `out_of_range` | The type of error ("asked for something outside the allowed range"). It is a class that comes with C++. |
| `("dequeue from an empty queue")` | The message that is shown. |

**Why not just `cout`?** `cout` only prints a message, and the code **keeps running** on an empty queue. It would read a garbage value (reading outside the list), and `head` would move past the end. `throw` **stops the function**, so a wrong value is never used, and the bug is found at once.

If nobody handles the error, the program ends and shows:

```
terminate called after throwing an instance of 'std::out_of_range'
  what():  dequeue from an empty queue
```

You can also **catch** it, so the program goes on:

```cpp
try {
    int x = q.dequeue();                 // may throw
} catch (out_of_range& e) {
    cout << "Problem: " << e.what() << "\n";
}
```

In our search code, we always check `!q.is_empty()` before taking an item, so this error never happens. It is a safety net.

---

## 6. How I Solved Each Part

### Setup (`Planner` constructor)
For each city, I keep a list of the flights that **leave** from it. This is one pass over all flights, so it takes `O(m)` time. The size of this list is *(biggest city number + 1)*.

### Route 1: `least_flights_earliest_route`
1. Reset the notes. Put every valid starting flight in a queue (level 1). A valid starting flight leaves at or after `t1` and lands by `t2`.
2. Take a flight out of the queue. For every flight that can follow it, set `level = level + 1`, set its `parent`, mark it `visited`, and add it to the queue.
3. Whenever a flight lands in `end_city`, keep it if it has fewer flights than the best so far, or the same number of flights but an earlier landing.
4. Stop when we reach flights with more levels than the best answer.
5. Follow the `parent` links backwards to build the route, then reverse it.

### Route 2: `cheapest_route`
1. Reset the notes. Put the valid starting flights in a min-heap (smaller = lower cost).
2. Take out the cheapest flight. If it lands in `end_city`, it is a possible answer. Otherwise, try all connecting flights: `new cost = cost so far + next flight's fare`. If it is better, save the cost and the parent, and push it into the heap.
3. Old heap entries are skipped (this almost never happens in this problem, but it is the safe way to write Dijkstra).
4. Build the route using the `parent` links.

### Route 3: `least_flights_cheapest_route`
Same as Route 2, but the heap compares **(number of flights first, then price)**. Because the best (flights, price) pair always comes out first, the first time we reach a flight is already the best way to reach it. So each flight is marked `visited` right when it is pushed.

---

## 7. Dry Run on the Example

Trip: start city 0, end city 4, `t1 = 0`, `t2 = 300`. (Flight data is in section 2.)

### Route 1 (BFS)

| Step | Take out | What we add | Queue after |
|---|---|---|---|
| Start | | F0, F1 (level 1) | F0, F1 |
| 1 | F0 (lands 30, next must leave at 50 or later) | F3 (level 2). F2 leaves at 40, so it is skipped. | F1, F3 |
| 2 | F1 (lands 80, next must leave at 100 or later) | F5, F6 (level 2) | F3, F5, F6 |
| 3 | F3 (lands 100, next must leave at 120 or later) | F4 (level 3) | F5, F6, F4 |
| 4 | F5 | Lands in city 4. **Best = F5** (2 flights, lands 150). | F6, F4 |
| 5 | F6 | Lands in city 4, also 2 flights, but 250 is later than 150. Not better. | F4 |
| 6 | F4 | Level 3 is more than 2. **Stop.** | |

**Answer: F1 → F5** ✅

### Route 2 (Dijkstra, smaller = cheaper)

| Step | Take out (cost) | What we do |
|---|---|---|
| Start | | Push F0 (50), F1 (200) |
| 1 | F0 (50) | F3 can follow: 50 + 120 = 170. Push F3 (170). |
| 2 | F3 (170) | F4 can follow: 170 + 100 = 270. Push F4 (270). |
| 3 | F1 (200) | F5: 200 + 500 = 700. F6: 200 + 300 = 500. Push both. |
| 4 | F4 (270) | Lands in city 4. **Best = F4, price 270.** |
| 5 | F6 (500) | Lands in city 4, but 500 is not less than 270. |
| 6 | F5 (700) | Lands in city 4, but 700 is not less than 270. |

**Answer: F0 → F3 → F4, total price 270** ✅

### Route 3 (Dijkstra, smaller = fewer flights, then cheaper)

Each heap item is written as (flights, price).

| Step | Take out | What we do |
|---|---|---|
| Start | | Push F0 (1, 50), F1 (1, 200) |
| 1 | F0 (1, 50) | Push F3 (2, 170). |
| 2 | F1 (1, 200) | Push F5 (2, 700), F6 (2, 500). |
| 3 | F3 (2, 170) | Push F4 (3, 270). |
| 4 | F6 (2, 500) | Lands in city 4. **Best = F6** (2 flights, price 500). |
| 5 | F5 (2, 700) | Same 2 flights, but 700 is not cheaper than 500. |
| 6 | F4 (3, 270) | 3 flights is more than 2. **Stop.** |

**Answer: F1 → F6 (2 flights, price 500)** ✅

---

## 8. Code Structure

```
.
├── flight.h      # Flight class (a flight's data and notes)
├── planner.h     # Queue, Entry, Heap and Planner classes (all the logic)
├── main.cpp      # Small test with the example above
└── README.md
```

- **`Queue`**: my own first-in, first-out line (used by BFS).
- **`Entry`**: a small box `{flight position, cost, level}` stored in the heap.
- **`Heap`**: my own min-heap where you give the rule for "who is smaller".
- **`Planner`**: has the three route-finding methods.

Note: the method `least_flights_ealiest_route` (with the spelling "ealiest") is kept because the assignment uses that exact name. It simply calls the correctly spelled `least_flights_earliest_route`.

---

## 9. How to Run

You need a C++ compiler (like `g++`).

```bash
git clone https://github.com/Aashrith1072/flight-planner.git
cd flight-planner
g++ -std=c++17 main.cpp -o flight_planner
./flight_planner
```

Expected output:

```
Task 1 PASSED
Task 2 PASSED
Task 3 PASSED
```

### Using the planner

```cpp
vector<Flight> flights = {
    Flight(0, 0, 0,   1, 30,  50),
    Flight(1, 0, 0,   3, 80,  200),
    Flight(2, 1, 40,  2, 60,  20),
    Flight(3, 1, 50,  2, 100, 120),
    Flight(4, 2, 120, 4, 200, 100),
    Flight(5, 3, 100, 4, 150, 500),
    Flight(6, 3, 100, 4, 250, 300)
};

Planner flight_planner(flights);   // make a Planner object

vector<int> r1 = flight_planner.least_flights_ealiest_route(0, 4, 0, 300);  // {1, 5}
vector<int> r2 = flight_planner.cheapest_route(0, 4, 0, 300);               // {0, 3, 4}
vector<int> r3 = flight_planner.least_flights_cheapest_route(0, 4, 0, 300); // {1, 6}
```

Each answer is a list of flight positions, in the order you take the flights.

---

## 10. Time Complexity

| Part | Time |
|---|---|
| Build planner | `O(m)` |
| Route 1 (BFS) | `O(m)` |
| Route 2 (Dijkstra) | `O(m log m)` |
| Route 3 (Dijkstra) | `O(m log m)` |

This works because at most 100 flights touch a single city, so checking the connecting flights of one flight is a small, fixed amount of work.

---

## 11. Notes and Ideas to Improve

- If two routes are exactly equally good, the code returns **one** of them (the first it finds). The question asks for one route, so this is fine.
- Route 2 has a line that skips old heap entries. In this problem it never triggers (I tested it), but I keep it because it is the standard safe way to write Dijkstra.
- I could replace my own `Queue` with `deque`, or the built-in `priority_queue` for the heap, to write less code. I kept my own classes to learn how they work.
- Add more test cases (no route, same start and end city, flights with equal prices).

---

## 12. What I Learned

- How to turn a real-life problem (flight search) into a graph problem.
- When to use BFS and when to use Dijkstra.
- How a queue and a heap work inside, by writing them myself as classes.
- Why choosing the right "dot" (a flight, not a city) matters.
- How `throw` protects code from using wrong values.

---

## Author

**Aashrith** – [@Aashrith1072](https://github.com/Aashrith1072)
