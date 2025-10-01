import sys
from pympler import tracker
import time

def lev(strA: str, strB: str) -> str:
    #Initialize variables
    m = len(strA)
    n = len(strB)
    v0 = [0] * (n + 1)
    v1 = [0] * (n + 1)
    deletion_cost = 0
    insertion_cost = 0
    substitution_cost = 0

    #initialize v0 (the previous row of distances)
    #this row is A[0][i]: edit distance from an empty s to t;
    #that distance is the number of characters to append to s to make t.
    for i in range(n):
        v0[i] = i

    for i in range(m):
        #calculate v1 (current row distances) from the previous row v0

        #first element of v1 is A[i + 1][0]
        #edit distance is delete (i + 1) chars from s to match empty t
        v1[0] = i + 1

        for j in range(n):
            #calculating costs for A[i + 1][j + 1]
            deletion_cost = v0[j + 1] + 1
            insertion_cost = v1[j] + 1

            if strA[i] == strB[j]:
                substitution_cost = v0[j]
            else:
                substitution_cost = v0[j] + 1

            v1[j + 1] = min(deletion_cost, insertion_cost, substitution_cost)

        #copy v1 (current row) to v0 (previous row) for next iteration
        v0, v1 = v1, v0

    return v0[n]

startTime = time.time() #Start time
tr = tracker.SummaryTracker() #Initial memory
file = ""

#Open file 
try:
    file = open(sys.argv[2], "r") 
except FileNotFoundError:
    print("Error: The file" + sys.argv[2] + "was not found.")
except Exception as e:
    print(f"An error occurred: {e}")

order = [] #output list

#Add tuples to list, call lev function
for line in file:
    order += list(zip([line.strip()], [lev(sys.argv[1], line.strip())]))

#Order tuple list by second value
order.sort(key=lambda x: x[1])

#Write output to txt file
with open(sys.argv[3], "w") as f:
    for item in order:
        f.write(f"{item[0]} {item[1]}\n")

#Calculate memory usage
summary_items = tr.diff()
memUsed = float(sum(item[2] for item in summary_items)) / (1024 * 1024)

#Calculate final time
timeUsed = time.time() - startTime

print("\nPython:")
print(f"Execution time: {timeUsed:.4f}s")
print(f"Memory increase: {memUsed:.4f} MiB")