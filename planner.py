from flight import Flight


class Queue:
    def __init__(self):
        """Initialize an empty queue."""
        self.items = []
    
    def enqueue(self, item):
        """Add an item to the end of the queue."""
        self.items.append(item)
    
    def dequeue(self):
        """Remove and return the item from the front of the queue."""
        if not self.is_empty():
            return self.items.pop(0)
        else:
            raise IndexError("dequeue from an empty queue")
    
    def is_empty(self):
        """Check if the queue is empty."""
        return len(self.items) == 0
    
    def peek(self):
        """Return the item at the front of the queue without removing it."""
        if not self.is_empty():
            return self.items[0]
        else:
            raise IndexError("peek from an empty queue")
    
    def size(self):
        """Return the number of items in the queue."""
        return len(self.items)



class Heap:
    def __init__(self, comparison_function, init_array):
        self.comparison_function = comparison_function  # Store the comparison function
        self.heap = init_array[:]  # Create a copy of the initial array
        n = len(self.heap)
        for i in reversed(range(n // 2)):
            self._heapify_down(i)
        
    def insert(self, value):
        self.heap.append(value)  # Add value to the end
        self._heapify_up(len(self.heap) - 1)  # Restore heap property by bubbling up

    def extract(self):
        if not self.heap:
            raise IndexError("Extract from an empty heap")
        
        top_value = self.heap[0]  # Get the top value (root)
        last_value = self.heap.pop()  # Remove the last element
        
        if self.heap:
            self.heap[0] = last_value  # Move the last element to the root
            self._heapify_down(0)  # Restore heap property by bubbling down
        
        return top_value

    def top(self):
        if not self.heap:
            raise IndexError("Top from an empty heap")
        return self.heap[0]
    
    def _parent(self, index):
        return (index - 1) // 2 if index > 0 else None
    
    def is_empty(self):
        return len(self.heap) == 0

    def _heapify_down(self, index):
        left = 2 * index + 1
        right = 2 * index + 2
        smallest = index
        
        if left < len(self.heap) and self.comparison_function(self.heap[left], self.heap[smallest]):
            smallest = left
        if right < len(self.heap) and self.comparison_function(self.heap[right], self.heap[smallest]):
            smallest = right
        
        if smallest != index:
            self.heap[index], self.heap[smallest] = self.heap[smallest], self.heap[index]
            self._heapify_down(smallest)

    def _heapify_up(self, index):
        parent = self._parent(index)
        if parent is not None and self.comparison_function(self.heap[index], self.heap[parent]):
            self.heap[index], self.heap[parent] = self.heap[parent], self.heap[index]
            self._heapify_up(parent)


class Planner:
    def __init__(self, flights):
        """The Planner

        Args:
            flights (List[Flight]): A list of information of all the flights (objects of class Flight)
        """
        self.flights = flights
        self.cities=[[] for i in range(len(flights))]
        for flight in self.flights: #O(m)
            self.cities[flight.start_city].append(flight)

    def check(self):
        for i in range(len(self.cities)):
            print(i)
            for flight in self.cities[i]:
                print(flight.flight_no,flight.start_city,flight.departure_time,flight.end_city,flight.arrival_time,flight.fare)

    def least_flights_ealiest_route(self, start_city, end_city, t1, t2):
        return self.least_flights_earliest_route(start_city, end_city, t1, t2)
    
    def least_flights_earliest_route(self, start_city, end_city, t1, t2):
        """
        Return List[Flight]: A route from start_city to end_city, which departs after t1 (>= t1) and
        arrives before t2 (<=) satisfying: 
        The route has the least number of flights, and within routes with same number of flights, 
        arrives the earliest
        """
        # Initialize queue
        q = Queue()
        
        # Reset all flights' visited status and level
        for city_flights in self.cities:
            for flight in city_flights:
                flight.visited = False
                flight.level2 = 0
        
        # Initialize starting flights
        for flight in self.cities[start_city]:
            if flight.departure_time >= t1 and flight.arrival_time <= t2:
                flight.level2 = 1  # Initialize level
                flight.parent = None
                flight.visited = True
                q.enqueue(flight)
        
        best_route = None
        min_flights = float('inf')
        earliest_arrival = float('inf')
        
        while not q.is_empty():
            current_flight = q.dequeue()
            
            # If we found a route to destination
            if current_flight.end_city == end_city:
                # Found a route with fewer flights or same flights but earlier arrival
                if (current_flight.level2 < min_flights or
                    (current_flight.level2 == min_flights and 
                    current_flight.arrival_time < earliest_arrival)):
                    min_flights = current_flight.level2
                    earliest_arrival = current_flight.arrival_time
                    best_route = current_flight
            
            # If we've found a route, don't explore paths with more flights
            # if best_route and current_flight.level2 >= min_flights:
            #     continue
            if current_flight.level2 > min_flights:
                break
                
            # Explore next possible flights
            connection_time = current_flight.arrival_time + 20
            for next_flight in self.cities[current_flight.end_city]:
                # Check if flight hasn't been visited and meets time constraints
                if (not next_flight.visited and 
                    next_flight.departure_time >= connection_time and 
                    next_flight.arrival_time <= t2):
                    
                    next_flight.level2 = current_flight.level2 + 1
                    next_flight.parent = current_flight
                    next_flight.visited = True
                    q.enqueue(next_flight)
                    # if(next_flight.end_city == end_city):
                    #     min_flights = next_flight.level2
        
        # Reconstruct the route
        route = []
        current = best_route
        while current is not None:
            route.append(current)
            current = current.parent
        
        return route[::-1]  # Return reversed route
        
    def comparison_function(self, a, b):
        return a[1]<b[1]
    #check if a[1]==b[1]
    
    

    def cheapest_route(self, start_city, end_city, t1, t2):
        """
        Return List[Flight]: A route from start_city to end_city, which departs after t1 (>= t1) and
        arrives before t2 (<=) satisfying: 
        The route is a cheapest route
        """
        # Initialize heap
        # We want heap as we want cheapest flight at top, and want to explore it,
        #we know that reaching a city with least cost at the top of heap
        heap = Heap(self.comparison_function, [])
        
        # Reset all flights' visited status and cost
        for city_flights in self.cities:
            for flight in city_flights:
                flight.visited = False
                flight.cost = float('inf')
        
        # Initialize starting flights
        for flight in self.cities[start_city]:
            if flight.departure_time >= t1 and flight.arrival_time <= t2:
                heap.insert((flight, flight.fare))
                flight.cost = flight.fare
                flight.parent = None
        
        ans = None
        cheap = float('inf')
        
        while not heap.is_empty():
            flight, cost = heap.extract()
            
            # Skip if we've already visited this flight or found a cheaper path

            if flight.visited or cost > flight.cost:
                continue
            
            
            if flight.end_city == end_city:
                if cost < cheap:
                    ans = flight
                    cheap = cost
            else:
                for next_flight in self.cities[flight.end_city]:
                    if (not next_flight.visited and 
                        next_flight.departure_time >= (flight.arrival_time + 20) and 
                        next_flight.arrival_time <= t2):
                        
                        new_cost = cost + next_flight.fare
                        if new_cost <= next_flight.cost:
                            #I don't need to visit this flight again, as it's parent is already at minimum cost,
                            #coz it is popped!!!
                            #and there's no benefit to take flight to same city multiple times
                            flight.visited = True
                            next_flight.cost = new_cost
                            next_flight.parent = flight
                            heap.insert((next_flight, new_cost))
        
        # Reconstruct route
        route = []
        while ans is not None:
            route.append(ans)
            ans = ans.parent
        
        route.reverse()
        return route


    def comparison_function1(self, a, b):
        if(a[2]<b[2]):
            return True
        elif(a[2]==b[2]):
            if(a[1]<b[1]):
                return True
        return False
    #check if a[1]==b[1]
    
    def least_flights_cheapest_route(self, start_city, end_city, t1, t2):
        """
        Return List[Flight]: A route from start_city to end_city, which departs after t1 (>= t1) and
        arrives before t2 (<=) satisfying: 
        The route has the least number of flights, and within routes with same number of flights, 
        is the cheapest
        """
        heap = Heap(self.comparison_function1, [])
        
        # Reset all flights
        for city_flights in self.cities:
            for flight in city_flights:
                flight.visited = False
                flight.level = float('inf')
                flight.cost = float('inf')
        
        # Initialize starting flights
        for flight in self.cities[start_city]:
            if flight.departure_time >= t1 and flight.arrival_time <= t2:
                flight.level = 1
                flight.cost = flight.fare
                flight.parent = None
                flight.visited = True
                heap.insert((flight, flight.fare, 1))
        
        ans = None
        ans_level = float('inf')
        cheap = float('inf')
        
        while not heap.is_empty():
            flight, cost, level = heap.extract()
            
            #updated ans_level, if found
            #if level==ans_level, I will find cheapest
            if level > ans_level:
                # If we're looking at flights with more stops than our best route, stop
                break
                
            if flight.end_city == end_city:
                if (level < ans_level or 
                    (level == ans_level and cost < cheap)):
                    ans = flight
                    cheap = cost
                    ans_level = level
            else:
                for next_flight in self.cities[flight.end_city]:
                    if (not next_flight.visited and 
                        next_flight.departure_time >= (flight.arrival_time + 20) and 
                        next_flight.arrival_time <= t2):
                        
                        new_cost = cost + next_flight.fare
                        new_level = level + 1
                        
                        # Only process if this path has fewer flights or same flights but cheaper
    
                        if (new_level < next_flight.level or 
                            (new_level == next_flight.level and new_cost < next_flight.cost)):
                            
                            next_flight.level = new_level
                            next_flight.cost = new_cost
                            next_flight.parent = flight
                            #Update inside as we can't go to same city again cheaper
                            #If come to parent_city cheaper
                            next_flight.visited = True
                            heap.insert((next_flight, new_cost, new_level))
        
        # Reconstruct route
        route = []
        while ans is not None:
            route.append(ans)
            ans = ans.parent
        
        route.reverse()
        return route