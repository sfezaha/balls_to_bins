#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <ctime>
// #include "balls_to_bins.h"



class Bin{
    public:
    int load;

    Bin(){
        load = 0;
    }

    void addBall(bool dynamic_ld = true, int max_ld=100){
        if (dynamic_ld){
          max_ld = rand() % max_ld;
        }
        load +=max_ld;
    }
};


class BaseBallsToBins {
protected:
    int numOfBins;
    int numOfBalls;
    Bin* bins;



    BaseBallsToBins(int numOfBalls, int numOfBins) {
        this->numOfBalls = numOfBalls;
        this->numOfBins = numOfBins;
        bins = new Bin[this->numOfBins];
    }

    void calculateMinLoad(){
         minLoad = bins[0].load;
         for (int i = 1; i < numOfBins; i++) {
            if (bins[i].load < minLoad) {
                minLoad = bins[i].load;
            }
        }

    }

    void calculateMaxLoad(){
         maxLoad = bins[0].load;
         for (int i = 1; i < numOfBins; i++) {
            if (bins[i].load > maxLoad) {
                maxLoad = bins[i].load;
            }
        }

    }

    void calculateGap() {
        calculateMinLoad();
        calculateMaxLoad();
        maxGap = maxLoad - minLoad;
    }


    void analyze() {
        calculateGap(); 
        // std::cout << "+-------+--------+" << std::endl;
        // std::cout << "| Bin # | Load   |" << std::endl;
        // std::cout << "+-------+--------+" << std::endl;
        // for (int i = 0; i < numOfBins; ++i) {
        //     std::cout << "| " << std::setw(5) << i+1 << " | " << std::setw(6) << bins[i].load << " |" << std::endl;
        // }
        // std::cout << "+-------+--------+" << std::endl;
        std::cout << "Max Load: " << maxLoad << ", Min Load: " << minLoad << ", Max Gap: "
         << maxGap << "  , Runtime:   " << runtime << std::endl;

    }

public:

    int maxLoad;
    int minLoad;
    int maxGap;
    int runtime;

    virtual void calc() {
        std::cout << "Base calc. Not implemented." << std::endl;
    }

    void runSimulation(){
    auto start = std::chrono::high_resolution_clock::now();
    calc();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    runtime = duration.count();
    analyze();
    std::cout << "runtime in us:     " << runtime << std::endl; 
    
    }



    virtual ~BaseBallsToBins() {
        delete[] bins;
    }

};

class PowerOfChoices : public BaseBallsToBins {
private:
    int numOfChoices;

public:
    PowerOfChoices(int numOfBalls, int numOfBins, int numOfChoices)
        : BaseBallsToBins(numOfBalls, numOfBins), numOfChoices(numOfChoices) {}

    void calc() {
        int remainingBalls = numOfBalls;

        while (remainingBalls > 0) {
            int prevIdx = -1;

            for (int i = 0; i < numOfChoices; i++) {
                int idx = rand() % numOfBins;
                while(idx == prevIdx) {idx = rand() % numOfBins;}
                if (i == 0 || bins[idx].load < bins[prevIdx].load) {
                    prevIdx = idx;
                }
            }

            bins[prevIdx].addBall();
            remainingBalls -= 1;
        }
    }
};


class RandomBin : public BaseBallsToBins {


public:
    RandomBin(int numOfBalls, int numOfBins) : BaseBallsToBins(numOfBalls, numOfBins) {}
    void calc() {
        int remainingBalls = numOfBalls;

        while (remainingBalls > 0) {
            int idx = rand() % numOfBins;
            bins[idx].addBall();
            remainingBalls -= 1;
        }
    }
};



class MinLoadAllocation : public BaseBallsToBins {

public:

    MinLoadAllocation(int numOfBalls, int numOfBins) : BaseBallsToBins(numOfBalls, numOfBins) {}

    void calc() {
        int remainingBalls = numOfBalls;
        while (remainingBalls > 0) {
            int minLoadIndex = getMinLoadIndex();
            bins[minLoadIndex].addBall();
            remainingBalls -= 1;
            }
    }

private:
    int getMinLoadIndex() {
        int minLoadIndex = 0;
        int minLoad = bins[0].load;

        for (int i = 1; i < numOfBins; i++) {
            if (bins[i].load < minLoad) {
                minLoadIndex = i;
                minLoad = bins[i].load;
            }
        }

        return minLoadIndex;
    }

};


class SimulationResults {
public:
    std::string filename;
    std::vector<std::string> colNameVec;
    std::vector<std::vector<int>> resVec;

    SimulationResults(const std::string& baseDirectory, const std::string& baseFilename) {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::ostringstream timeStamp;
        timeStamp << std::put_time(std::localtime(&now_c), "%Y-%m-%d_%H-%M-%S");

        filename = baseDirectory + baseFilename + "_" + ".csv";
    }


void performAndStoreResults(const std::vector<int>& bins, int numOfBalls, std::string restParam = "max_gap") {
    colNameVec = {"Number of Bins", "Power of 2 Choices", "Absolute minimum load", "Random allocation"};

    for (int nBins : bins) {
        PowerOfChoices pocSim(numOfBalls, nBins, 2);
        pocSim.runSimulation();
        MinLoadAllocation mlaSim(numOfBalls, nBins);
        mlaSim.runSimulation();
        RandomBin randSim(numOfBalls, nBins);
        randSim.runSimulation();

        int resultPoC, resultMLA, resultRand;

        if (restParam == "max_load") {
            resultPoC = pocSim.maxLoad;
            resultMLA = mlaSim.maxLoad;
            resultRand = randSim.maxLoad;
        } else if (restParam == "runtime") {
            resultPoC = pocSim.runtime;
            resultMLA = mlaSim.runtime;
            resultRand = randSim.runtime;
        } else if (restParam == "max_gap") {
            resultPoC = pocSim.maxGap;
            resultMLA = mlaSim.maxGap;
            resultRand = randSim.maxGap;
        }
         else {
            throw std::invalid_argument("performAndStoreResults: wrong restParam");
        }

        resVec.push_back({nBins, resultPoC, resultMLA, resultRand});
    }
}
    
    void saveToCSV() {
        std::ofstream outFile(filename);

        if (!outFile.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        for (size_t i = 0; i < colNameVec.size(); ++i) {
            outFile << colNameVec[i];
            if (i < colNameVec.size() - 1) {
                outFile << ",";
            }
        }
        outFile << "\n";

        for (const auto& row : resVec) {
            for (size_t i = 0; i < row.size(); ++i) {
                outFile << row[i];
                if (i < row.size() - 1) {
                    outFile << ",";
                }
            }
            outFile << "\n";
        }

        outFile.close();
    }
};



int main() {
    std::vector<int> bins = {16, 32, 64, 128, 256, 512, 1024};
    // std::vector<int> bins = {16, 32, 64, 128, 256, 512, 1024};
    int numOfBalls = 10000;
    std::string base_dir = "/home/asfez/personal/cpp_pojects/balls_to_bins/data/";
    int numberOfSimulations = 50;
    
    for (int i = 0; i < numberOfSimulations; i++) {
        srand(static_cast<unsigned int>(time(NULL)) + i);

        std::string mg_filename = "max_gaps/max_gaps_simulation_results" + std::to_string(i);
        SimulationResults mg_simResults(base_dir, mg_filename);
        mg_simResults.performAndStoreResults(bins, numOfBalls, "max_gap");
        mg_simResults.saveToCSV();

        std::string ml_filename = "max_loads/max_load_simulation_results" + std::to_string(i);
        SimulationResults ml_simResults(base_dir, ml_filename);
        ml_simResults.performAndStoreResults(bins, numOfBalls, "max_load");
        ml_simResults.saveToCSV();

        std::string rt_filename = "runtimes/runtime_simulation_results" + std::to_string(i);
        SimulationResults rt_simResults(base_dir, rt_filename);
        rt_simResults.performAndStoreResults(bins, numOfBalls, "runtime");
        rt_simResults.saveToCSV();
    }

    return 0;
}
