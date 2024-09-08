#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

using namespace std;

pair<vector<string>, string> parseInstruction(const string &line)
{
    vector<string> v;
    istringstream iss(line);
    string core, zeross, address, machineCode, opcode, instruction;
    iss >> core >> zeross >> address >> machineCode >> opcode;
    // cout<<core<<zeross<<address<<machineCode<<opcode<<instruction<<endl;
    // cout<<address<<endl;
    // nextlinepc = address;
    if (opcode[0] == 'b')
    {
        string ins1, ins2, str1, str2, str3; // str2=pc+
        iss >> ins1 >> ins2 >> str1 >> str2 >> str3;
        // cout<<ins1<<" "<<ins2<<" "<<str1<<" "<<str2<<endl;
        // cout<<str3<<endl;
        if (str3 == "")
        {
            v.push_back(str2);
        }
        else
        {
            v.push_back(str3);
        }
    }
    for (auto x : v)
    {
        // cout << x << endl;
    }
    v.push_back(address);
    return make_pair(v, opcode);
}

double OneBit_Pred(long long CurrentAddress, long long NextAddress, long long TargetAddress, double &predicted, double &total, unordered_map<long long, bool> &History)
{
    auto index = History.find(CurrentAddress);
    if (index == History.end())
    {
        History[CurrentAddress] = true;
    }
    if (History[CurrentAddress] && (NextAddress == TargetAddress))
    {
        predicted++;
    }
    else if (!History[CurrentAddress] && (NextAddress == TargetAddress))
    {
        History[CurrentAddress] = true;
    }
    else if (History[CurrentAddress] && (NextAddress != TargetAddress))
    {
        History[CurrentAddress] = false;
    }
    else{
        predicted++;
    }
    total++;
    return static_cast<double>(predicted) / total;
}


#include <unordered_map>

float TwoBit_Pred(long long CurrentAddress, long long NextAddress, long long TargetAddress, double &predicted, double &total, unordered_map<long long, int> &History)
{
    auto index = History.find(CurrentAddress);
    if (index == History.end())
    {
        History[CurrentAddress] = 2;
    }

    if (NextAddress == TargetAddress)
    {
        if (History[CurrentAddress] == 2 || History[CurrentAddress] == 3)
        {
            predicted++;
            History[CurrentAddress]++;
            if (History[CurrentAddress] >= 3)
            {
                History[CurrentAddress] = 3;
            }
        }
        if (History[CurrentAddress] != 3)
        {
            History[CurrentAddress]++;
        }
    }
    else if (NextAddress != TargetAddress)
    {
        if (History[CurrentAddress] != 0)
        {
            History[CurrentAddress]--;
        }
        if (History[CurrentAddress] == 0 || History[CurrentAddress] == 1)
        {
            predicted++;
            History[CurrentAddress]--;
            if (History[CurrentAddress] <= 3)
            {
                History[CurrentAddress] = 0;
            }
        }
    }

    total++;
    return static_cast<double>(predicted) / total;
}

double AlwaysTaken_Pred(long long CurrentAddress, long long NextAddress, long long TargetAddress, double &predicted, double &total)
{
    if (TargetAddress == NextAddress)
    {
        predicted++;
    }
    // cout << "a" << endl;

    total++;
    return static_cast<double>(predicted) / total;
}

float AlwaysNotTaken_Pred(long long CurrentAddress, long long NextAddress, long long TargetAddress, double &predicted, double &total)
{
    if (TargetAddress != NextAddress)
    {
        predicted++;
    }
    total++;
    return (predicted) / total;
}

string longLongToHex(long long value)
{
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
}

int main()
{
    string tracerfile = "quicksort.txt";
    ifstream traceFile(tracerfile);

    if (!traceFile.is_open())
    {
        cerr << "Error: Unable to open trace file." << endl;
        return 1;
    }
    string currline, nextline;
    getline(traceFile, currline);
    double AlwaysTakenAcc = 0;
    double AlwaysNotTakenAcc = 0;
    double OneBitAcc = 0;
    double TwoBitAcc = 0;
    long long count = 0;
    double predicted_AlwaysTaken = 0;
    double total_AlwaysTaken = 0;
    double predicted_AlwaysNotTaken = 0;
    double total_AlwaysNotTaken = 0;
    double predicted_OneBit = 0;
    double total_OneBit = 0;
    unordered_map<long long, bool> History;
    double predicted_TwoBit = 0;
    double total_TwoBit = 0;
    unordered_map<long long, int> History1;

    unordered_map<string, string> Buffer;
    while (getline(traceFile, nextline))
    {
        count++;
        // cout<<count<<endl;
        if (nextline.find("core"))
        {
            // cout<<"a"<<endl;
            continue;
        }
        // declaring the global parameters

        // cout<<line;
        pair<vector<string>, string> instruction = parseInstruction(nextline);
        pair<vector<string>, string> instruction2 = parseInstruction(currline);
        string opcode = instruction2.second;

        if (opcode[0] == 'b')
        {
            string currentAddress = instruction2.first.back();
            string offset = instruction2.first[0];
            string nextAddress = instruction.first.back();

            long long curradd = stoll(currentAddress, nullptr, 16);
            // cout << curradd << endl;
            long long nextadd = stoll(nextAddress, nullptr, 16);
            // cout << nextadd << endl;
            long long offs = stoll(offset, nullptr, 16);
            long long taradd = curradd + offs;
            // cout << curradd << endl;
            // cout << nextadd << endl;
            // cout << taradd << endl;
            auto index = Buffer.find(currentAddress);
            if (index == Buffer.end())
            {
                Buffer[currentAddress] = longLongToHex(taradd);
            }
            AlwaysTakenAcc = AlwaysTaken_Pred(curradd, nextadd, taradd, predicted_AlwaysTaken, total_AlwaysTaken);
            AlwaysNotTakenAcc = AlwaysNotTaken_Pred(curradd, nextadd, taradd, predicted_AlwaysNotTaken, total_AlwaysNotTaken);
            OneBitAcc = OneBit_Pred(curradd, nextadd, taradd, predicted_OneBit, total_OneBit, History);
            TwoBitAcc = TwoBit_Pred(curradd, nextadd, taradd, predicted_TwoBit, total_TwoBit, History1);
        }
        currline = nextline;
    }
    cout << "AlwaysTakenAcc"
         << ":" << AlwaysTakenAcc << endl;
    cout << "AlwaysNotTakenAcc"
         << ":" << AlwaysNotTakenAcc << endl;
    cout << "OneBitAcc"
         << ":" << OneBitAcc << endl;
    cout << "TwoBitAcc"
         << ":" << TwoBitAcc << endl;
    for (const auto &pair : Buffer)
    {
        std::cout << "current Address: " << pair.first << ", Target Address: " << pair.second << std::endl;
    }
    // cout << AlwaysTakenAcc << " " << AlwaysNotTakenAcc << " " << OneBitAcc << " " << TwoBitAcc << endl;

    traceFile.close();
    return 0;
}
