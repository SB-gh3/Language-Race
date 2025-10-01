import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import org.javatuples.Pair;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.Arrays;

public class lev 
{
    static int lev(String strA, String strB)
    {
        //Initialize variables
    int m = strA.length();
    int n = strB.length();
    int[] v0 = new int[n + 1];
    int[]v1 = new int[n + 1];
    int deletion_cost;
    int insertion_cost;
    int substitution_cost;

    // initialize v0 (the previous row of distances)
    // this row is A[0][i]: edit distance from an empty s to t;
    // that distance is the number of characters to append to s to make t.
    for (int i = 0; i <= n; ++i) 
    {
        v0[i] = i;
    }

    for (int i = 0; i < m; ++i) 
    {
        // calculate v1 (current row distances) from the previous row v0

        // first element of v1 is A[i + 1][0]
        // edit distance is delete (i + 1) chars from s to match empty t
        v1[0] = i + 1;

        for (int j = 0; j < n; ++j) 
        {
            // calculating costs for A[i + 1][j + 1]
            deletion_cost = v0[j + 1] + 1;
            insertion_cost = v1[j] + 1;

            if (strA.charAt(i) == strB.charAt(j)) 
            {
                substitution_cost = v0[j];
            } 
            else 
            {
                substitution_cost = v0[j] + 1;
            }

            v1[j + 1] = Math.min(deletion_cost, Math.min(insertion_cost, substitution_cost));
        }

        // copy v1 (current row) to v0 (previous row) for next iteration
        //This requires so many more lines than the other languages, why isnt there a swap function
        int[] temp = v1;
        v1 = v0;
        v0 = temp;
    }

    return v0[n];
    }

    public static void main(String[] args) 
    {
        long startTime = System.currentTimeMillis(); //Start time
        long startMem = Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory(); //Start memory
        String output = args[2];
        List<Pair<String, Integer>> order = new ArrayList<>();

        String file = args[1]; // Might give $str
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) 
        {
            String line;

            while ((line = reader.readLine()) != null) 
            {
                order.add(Pair.with(line, lev(args[0], line)));
            }
        } 
        catch (IOException e) 
        {
            System.err.println("Error reading file: " + e.getMessage());
        }

        // Sort the list using a custom Comparator
        Collections.sort(order, (Pair<String, Integer> p1, Pair<String, Integer> p2) -> 
        {
            // Primary sort by the integer distance (value1)
            int result = p1.getValue1().compareTo(p2.getValue1());
            // Secondary sort by the string (value0) if distances are equal
            if (result == 0) 
            {
                return p1.getValue0().compareTo(p2.getValue0());
            }
            return result;
        });

        //Write to output file
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(output))) 
        {
            for (Pair<String, Integer> item : order) 
            {
                writer.write(item.getValue0() + " " + item.getValue1());
                writer.newLine(); // Add a new line after each item
            }
        } catch (IOException e) 
        {
            System.err.println("Error writing list to file: " + e.getMessage());
            e.printStackTrace();
        }

        System.out.println("\nJava:");
        System.out.println("Execution time: " + (System.currentTimeMillis() - startTime)  + "ms");
        System.out.println("Memory increase: " + (float)(Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory() - startMem) / (float)(1024 * 1024) + " MiB\n");
    }
}