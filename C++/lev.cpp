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
    if (strB.empty())
    {
        return strA.size();
    }
    else if (strA.empty())
    {
        return strB.size();
    }
    else if (strA[0] == strB[0])
    {
        return lev(strA.substr(1), strB.substr(1));
    }
    else
    {
        return 1 + min(lev(strA.substr(1), strB), min(lev(strA, strB.substr(1)), lev(strA.substr(1), strB.substr(1))));
    }
}

int main(int argc, char* argv[]) 
{
    auto start = chrono::high_resolution_clock::now(); //Start time
    long initialMem = getMem(); //Start mem
    ifstream inputFile(argv[2]);
    ofstream outputFile("/home/spencerb/Documents/Misc-Development/Language-Race/Ordered/output-cpp.txt");
    vector<tuple<string, int>> order;

    if (inputFile.is_open()) 
    {
        string line;

        while (getline(inputFile, line)) //Variable line gets the value of a line in inputFile
        {
            order.push_back(make_tuple(line, lev(argv[1], line))); //Push tuple of values into vector
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

    cout << "\nC++:";
    cout << "Execution time: " << (float(milliseconds) / float(1000)) << "s" << endl;
    cout << "Memory increase: " << memIncrease << " MiB\n" << endl;
}