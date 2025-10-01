package main

import(
    "fmt"
    "os"
    "bufio"
    "log"
    "sort"
    "runtime"
    "time"
)

//Create tuple
type Tuple struct{
    e1 string
    e2 int
}

func lev(strA string, strB string) int{
    //Initialize variables
    m := len(strA)
    n := len(strB)
    v0 := make([]int, n + 1)
    v1 := make([]int, n + 1)
    var deletion_cost int
    var insertion_cost int
    var substitution_cost int

    // initialize v0 (the previous row of distances)
    // this row is A[0][i]: edit distance from an empty s to t;
    // that distance is the number of characters to append to s to make t.
    for i := 0; i <= n; i++{
        v0[i] = i;
    }

    for i := 0; i < m; i++{
        // calculate v1 (current row distances) from the previous row v0

        // first element of v1 is A[i + 1][0]
        // edit distance is delete (i + 1) chars from s to match empty t
        v1[0] = i + 1;

        for j := 0; j < n; j++{
            // calculating costs for A[i + 1][j + 1]
            deletion_cost = v0[j + 1] + 1;
            insertion_cost = v1[j] + 1;

            if strA[i] == strB[j]{
                substitution_cost = v0[j];
            } else {
                substitution_cost = v0[j] + 1;
            }

            v1[j + 1] = min(deletion_cost, insertion_cost, substitution_cost);
        }

        // copy v1 (current row) to v0 (previous row) for next iteration
        v0, v1 = v1, v0
    }

    return v0[n];
}

func main(){
    now := time.Now() //Start time
    var m, m2 runtime.MemStats
	runtime.ReadMemStats(&m) //Initial memory
    args := os.Args[1:] //Accept arguments excluding program path
    file, err := os.Open(args[1]) //Has to be both file and err
    var order []Tuple

    if err != nil{
		log.Fatalf("Error opening file: %v", err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan(){
		order = append(order, Tuple{e1: scanner.Text(), e2: lev(args[0], scanner.Text())}) // Call lev on each line and add to slice
	}

    //Sort slice by element 2 in tuple
    sort.SliceStable(order, func(i, j int) bool{
		return order[i].e2 < order[j].e2
	})

    //Write to file
    outputPath := args[2]
    outFile, err := os.Create(outputPath) //Create file
    if err != nil {
        log.Fatalf("Error creating output file: %v", err)
    }
    defer outFile.Close()

    writer := bufio.NewWriter(outFile) //Create writer
    for i := 0; i < len(order); i++ {
        line := fmt.Sprintf("%s %d\n", order[i].e1, order[i].e2) //Convert tuple to string
        _, err := writer.WriteString(line) //Write string to file
        if err != nil {
            log.Fatalf("Error writing to file: %v", err)
        }
    }
    writer.Flush()

	if err := scanner.Err(); err != nil{
		log.Fatalf("Error during scanning: %v", err)
	}

    runtime.ReadMemStats(&m2) //Final memory

    // Print info
    fmt.Println("\nGo:")
    fmt.Println("Execution time: ", time.Since(now)) //Print total time
    fmt.Println("Memory increase: ", float64(m2.HeapAlloc - m.HeapAlloc) / 1024. / 1024., "MiB") //Print total memory usage
}