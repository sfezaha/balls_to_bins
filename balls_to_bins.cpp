#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <ctime>








class Bin{
    public:
    int load;

    Bin(){
        load = 0;
    }

    void addBall(){
        load +=1;
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
        this->bins = new Bin[numOfBins]; 

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
        std::cout << "+-------+--------+" << std::endl;
        std::cout << "| Bin # | Load   |" << std::endl;
        std::cout << "+-------+--------+" << std::endl;
        maxLoad = bins[0].load;
        int minLoad = bins[1].load;
        for (int i = 0; i < numOfBins; ++i) {
            std::cout << "| " << std::setw(5) << i+1 << " | " << std::setw(6) << bins[i].load << " |" << std::endl;
        }
        std::cout << "+-------+--------+" << std::endl;
        std::cout << "Max Load: " << maxLoad << ", Min Load: " << minLoad << ", Max Gap: " << maxGap << std::endl;

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

void saveVectorsToCSV(const std::vector<std::pair<std::string, std::vector<int>>>& vectors) {
    // Prepare a CSV file for output
    std::ofstream file("vectors.csv");
    if (!file.is_open()) {
        std::cerr << "Unable to open vectors.csv for writing." << std::endl;
        return;
    }

    // Write CSV headers
    file << "Index";
    for (const auto& vector : vectors) {
        file << "," << vector.first;
    }
    file << "\n";

    // Write vector values to CSV
    int numElements = vectors[0].second.size();
    for (int i = 0; i < numElements; ++i) {
        file << i + 1;
        for (const auto& vector : vectors) {
            file << "," << vector.second[i];
        }
        file << "\n";
    }

    file.close();
}


int main() {
    // MinLoadAllocation child(10000, 32);
    // child.runSimulation();
    // PowerOfChoices child2(10000, 32, 2);
    // srand(static_cast<unsigned int>(time(NULL)));
    // child2.runSimulation(); 

    srand(static_cast<unsigned int>(time(NULL)));

    std::vector<int> balls{1000, 5000, 10000, 20000}; // Example numbers of balls
    int numOfBins = 32;
    int numOfChoices = 2;

    // Prepare a CSV file for output
    std::ofstream file("max_gaps.csv");
    if (!file.is_open()) {
        std::cerr << "Unable to open max_gaps.csv for writing." << std::endl;
        return 1;
    }

    // Write CSV headers
    file << "NumOfBalls,MinLoadAllocation,PowerOfChoices\n";

    for (int nBalls : balls) {
        MinLoadAllocation minLoadSimulation(nBalls, numOfBins);
        minLoadSimulation.runSimulation();
        int minLoadGap = minLoadSimulation.maxGap;

        PowerOfChoices powerOfChoicesSimulation(nBalls, numOfBins, numOfChoices);
        powerOfChoicesSimulation.runSimulation();
        int powerOfChoicesGap = powerOfChoicesSimulation.maxGap;

        // Write results to CSV
        file << nBalls << "," << minLoadGap << "," << powerOfChoicesGap << "\n";
    }

    file.close();

    

    return 0;
}
