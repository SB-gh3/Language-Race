#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <tuple>
#include <chrono>
#include <unistd.h>

using namespace std;

//Get current memory usage
long getMem() 
{
    long kb = 0;
    ifstream file("/proc/self/status");
    string line;

    if (file.is_open()) 
    {
        while (getline(file, line)) 
        {
            //Look for VmRSS, which indicates the Resident Set Size
            if (line.compare(0, 6, "VmRSS:") == 0) 
            {
                //Extract the number, typically followed by " kB"
                size_t start = line.find_first_of("0123456789");
                size_t end = line.find(" kB");
                if (start != string::npos && end != string::npos) {
                    string value_str = line.substr(start, end - start);
                    kb = stol(value_str);
                }
                break;
            }
        }
        file.close();
    }
    return kb;
}

template <size_t Index = 0, typename... Args>
typename enable_if<Index == sizeof...(Args), void>::type
write_tuple_elements(ofstream& os, const tuple<Args...>& t) 
{
    //If all elements already written
}

template <size_t Index = 0, typename... Args>
typename enable_if<Index < sizeof...(Args), void>::type
write_tuple_elements(ofstream& os, const tuple<Args...>& t) 
{
    os << get<Index>(t) << " "; //Write current element
    write_tuple_elements<Index + 1>(os, t); //Recursive call for next element of tuple
}

//Levenshtein Algorithm
int lev(string strA, string strB)
{
    //Initialize variables
    int m = strA.length();
    int n = strB.length();
    vector<int> v0(n + 1);
    vector<int> v1(n + 1);
    int deletion_cost;
    int insertion_cost;
    int substitution_cost;

    // initialize v0 (the previous row of distances)
    // this row is A[0][i]: edit distance from an empty s to t;
    // that distance is the number of characters to append to s to make t.
    for (size_t i = 0; i <= n; ++i) 
    {
        v0[i] = i;
    }

    for (size_t i = 0; i < m; ++i) 
    {
        // calculate v1 (current row distances) from the previous row v0

        // first element of v1 is A[i + 1][0]
        // edit distance is delete (i + 1) chars from s to match empty t
        v1[0] = i + 1;

        for (size_t j = 0; j < n; ++j) 
        {
            // calculating costs for A[i + 1][j + 1]
            deletion_cost = v0[j + 1] + 1;
            insertion_cost = v1[j] + 1;

            if (strA[i] == strB[j]) 
            {
                substitution_cost = v0[j];
            } 
            else 
            {
                substitution_cost = v0[j] + 1;
            }

            v1[j + 1] = min(deletion_cost, min(insertion_cost, substitution_cost));
        }

        // copy v1 (current row) to v0 (previous row) for next iteration
        swap(v0, v1);
    }

    return v0[n];
}

int main(int argc, char* argv[]) 
{
    auto start = chrono::high_resolution_clock::now(); //Start time
    long initialMem = getMem(); //Start mem
    ifstream inputFile(argv[2]);
    ofstream outputFile(argv[3]);
    vector<tuple<string, int>> order;

    if (inputFile.is_open()) 
    {
        string line;

        while (getline(inputFile, line)) //Variable line gets the value of a line in inputFile
        {
            //Broken newline character fix. Makes code compatable with Windows
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            // Calculate Levenshtein distance and store it with the line
            order.push_back(make_tuple(line, lev(argv[1], line)));
        }

        //Sort Vector
        stable_sort(order.begin(), order.end(), [](const auto& a, const auto& b)
        {
            return get<1>(a) < get<1>(b);
        });

        //Write contents of sorted vector to output file
        for (size_t i = 0; i < order.size(); i++)
        {
            write_tuple_elements(outputFile, order[i]);
            outputFile << "\n";
        }
    

        inputFile.close();
    } 
    else 
    {
        cerr << "Error: Unable to open the file." << endl;
    }

    //Calculate total used memory
    long finalMem = getMem();
    double memIncrease = (double)(finalMem - initialMem) / (1024.0 * 1024.0);

    //Calculate total time
    auto end = chrono::high_resolution_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "\nC++:" << endl;
    cout << "Execution time: " << milliseconds << "ms" << endl;
    cout << "Memory increase: " << memIncrease << " MiB\n" << endl;
}