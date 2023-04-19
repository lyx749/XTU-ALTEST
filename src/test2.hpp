#ifndef TEST_2_HPP
#define TEST_2_HPP
#include <vector>
#include <array>
#include <cmath>
#include <random>
#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include <list>
#include <memory>

const double PI = 3.1415926;
const double maxNums = 4194304.0;
using gene = std::array<double, 22>;
const double crossRate = 0.3;
const double mutateRate = 0.05;
const int populationSize = 32;
const int maxGeneration = 1000;

std::random_device seed;
std::mt19937 get(seed());

class individual;
class population;
std::list<individual *> allAllocatedMemory; // 用于释放内存
std::ofstream writeFile{"gene.txt", std::ios::out &std::ios::trunc &std::ios::app};

double targetFunction(double x)
{
    return (x * std::sin(PI * 10 * x) + 2.0);
}

void save(gene x)
{
    if (writeFile)
    {
        for (auto &e : x)
        {
            writeFile << e << " ";
        }
        writeFile << std::endl;
    }
    else
    {
        std::cerr << "can not open writefile" << std::endl;
    }
}

class individual
{
    friend class population;

public:
    gene x;
    double fitness;

public:
    double getDecode()
    {
        double k = 21;
        double sum = 0;
        for (int i = 0; i < 22; ++i)
        {
            sum += (x[i] * std::pow(2, k));
            k--;
        }
        return (-1 + (3.0 / maxNums * sum));
    }

private:
    void initIndividual()
    {
        std::uniform_int_distribution<> dist(0, 1);
        for (auto &e : x)
        {
            e = dist(get);
        }
    }

    void getFitness()
    {
        fitness = targetFunction(getDecode());
    }

    std::pair<std::shared_ptr<individual>, std::shared_ptr<individual>> cross(std::shared_ptr<individual> other, int point)
    {
        std::shared_ptr<individual> temp1 = std::make_shared<individual>();
        std::shared_ptr<individual> temp2 = std::make_shared<individual>();
        temp1->x = this->x;
        temp2->x = temp2->x;
        //writeFile << "cross over and cross location is " << point << std::endl;
        //save(temp1->x);
        //save(temp2->x);
        for (int i = point; i < 22; ++i)
        {
            std::swap(temp1->x[i], temp2->x[i]);
        }
        //save(temp1->x);
        //save(temp2->x);
        return std::make_pair(temp1, temp2);
    }

    std::shared_ptr<individual> mutate()
    {
        std::uniform_int_distribution<> dist(0, 21);
        int r = dist(get);
        //writeFile << "mutateOver and mutation location is " << r << std::endl;
        std::shared_ptr<individual> temp = std::make_shared<individual>();
        temp->x = this->x;
        temp->x[r] = !temp->x[r];
        //save(this->x);
        //save(temp->x);
        return temp;
    }
};

class population
{
public:
    std::array<std::shared_ptr<individual>, populationSize> selectedPopulation;
    std::vector<std::shared_ptr<individual>> currentPopulation;

public:
    population()
    {
        for (auto &e : selectedPopulation)
        {
            e = std::make_shared<individual>();
            e->initIndividual();
            currentPopulation.push_back(e);
        }
    }

    void calculateFitness()
    {
        for (auto &e : currentPopulation)
        {
            e->getFitness();
        }
    }

    void select()
    {
        double fitnessSum = 0.0;
        for (auto &e : currentPopulation)
        {
            fitnessSum += e->fitness;
        }
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        for (int i = 0; i < populationSize; ++i)
        {
            double r = dis(get) * fitnessSum;
            double sum = 0;
            for (int j = 0; j < currentPopulation.size(); ++j)
            {
                sum += currentPopulation[j]->fitness;
                if (sum >= r)
                {
                    selectedPopulation[i] = currentPopulation[j];
                    break;
                }
            }
        }
        currentPopulation.clear();
        std::for_each(selectedPopulation.begin(), selectedPopulation.end(), [&](std::shared_ptr<individual> e)
                      { currentPopulation.push_back(e); });
    }

    void crossOver()
    {
        size_t size = currentPopulation.size();
        std::uniform_int_distribution<> dId(0, size - 1);
        std::uniform_int_distribution<> dPoint(1, 21);
        std::uniform_real_distribution<double> dRate(0.0, 1.0);

        for (int i = 0; i < size; ++i)
        {
            int id = dId(get);
            int point = dPoint(get);
            double rate = dRate(get);
            if (rate <= crossRate)
            {

                std::pair<std::shared_ptr<individual>, std::shared_ptr<individual>> temp =
                    currentPopulation[i]->cross(currentPopulation[id], point);
                currentPopulation.push_back(temp.first);
                currentPopulation.push_back(temp.second);
            }
        }
    }

    void mutateOver()
    {
        std::uniform_real_distribution<double> dRate(0.0, 1.0);
        size_t size = currentPopulation.size();
        for (int i = 0; i < size; ++i)
        {
            double rate = dRate(get);
            if (rate <= mutateRate)
            {
                currentPopulation.push_back(currentPopulation[i]->mutate());
            }
        }
    }

    std::shared_ptr<individual> getBest()
    {
        std::shared_ptr<individual> best = selectedPopulation[0];
        for (int i = 1; i < populationSize; ++i)
        {
            best = (best->fitness < selectedPopulation[i]->fitness) ? selectedPopulation[i] : best;
        }
        return best;
    }

    void checkAndForcedMutate() // Check if have the same binary bits in the same location
    {
        std::uniform_int_distribution<> dId(0, populationSize - 1);
        for (int j = 0; j < 22; ++j)
        {
            bool ifSame = true;
            for (int i = 0; i < populationSize - 1; ++i)
            {
                if (selectedPopulation[i]->x[j] != selectedPopulation[i + 1]->x[j])
                    ifSame = false;
            }
            if (ifSame)
            {
                std::shared_ptr<individual> temp = std::make_shared<individual>();
                temp->x = selectedPopulation[dId(get)]->x;
                writeFile << "Forced mutation in position " << j << " all is " << temp->x[j] << std::endl;
                save(temp->x);
                temp->x[j] = !temp->x[j];
                save(temp->x);
                currentPopulation.push_back(temp);
                writeFile << "Forced mutation in position over!" << std::endl;
            }
        }
    }
};

void run()
{
    if (!writeFile)
    {
        std::cerr << "can not open writefile" << std::endl;
    }
    population p{};
    int count = 0;
    double tempBestFitness = p.getBest()->fitness;
    int i = 0;
    for (i = 0; i < maxGeneration; ++i)
    {
        writeFile << "Generation " << i + 1 << std::endl;
        p.calculateFitness();
        if (i % 100 == 0)
        {
            printf("Generation = %d\n", i);
        }
        //std::cout << "tempBestFitness = " << tempBestFitness << "p.getBest()->fitness = " << p.getBest()->fitness << std::endl;
        if(std::abs(tempBestFitness - p.getBest()->fitness) <= 0.000000000001)
        {
            ++count;
        }
        else
        {
            count = 0;
            tempBestFitness = p.getBest()->fitness;
        }
        p.select();
        p.crossOver();
        p.mutateOver();
        p.checkAndForcedMutate();
        for(auto &e : p.selectedPopulation)
        {
            save(e->x);
        }
        writeFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
        if(count == 20)
        {
            break;
        }
        //std::cout << count << std::endl;
    }
    std::shared_ptr<individual> best = p.getBest();
    printf("Generation = %d, x = %.10lf, fitness = %.10lf\n", i, best->getDecode(), best->fitness);
}

#endif