#ifndef TEST_2_HPP
#define TEST_2_HPP
#include <vector>
#include <array>
#include <cmath>
#include <random>
#include <iostream>
#include <format>

const double PI = 3.1415926;
const double maxNums = 4194304.0;
using gene = std::array<double, 22>;
const double crossRate = 0.9;
const double mutateRate = 0.1;
const int populationSize = 1000;
const int maxGeneration = 200;

std::random_device seed;
std::mt19937 get(seed());

double targetFunction(double x)
{
    return (x * std::sin(PI * 10 * x) + 2.0);
}

class individual
{
public:
    gene x;
    double fitness;

public:
    void initIndividual()
    {
        std::uniform_int_distribution<> dist(0, 1);
        for (auto &e : x)
        {
            e = dist(get) % 2;
        }
    }

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

    void getFitness()
    {
        fitness = targetFunction(getDecode());
    }

    void cross(individual &other, int point)
    {
        for (int i = point; i < 22; ++i)
        {
            std::swap(x[i], other.x[i]);
        }
    }

    void mutate()
    {
        std::uniform_int_distribution<> dist(0, 21);
        int r = dist(get);
        x[r] = !x[r];
    }
};

class population
{
public:
    std::array<individual, populationSize> allPopulation;

public:
    population()
    {
        for (auto &e : allPopulation)
        {
            e.initIndividual();
        }
    }

    void calculateFitness()
    {
        for (auto &e : allPopulation)
        {
            e.getFitness();
        }
    }

    void select()
    {
        double fitnessSum = 0.0;
        for (auto &e : allPopulation)
        {
            fitnessSum += e.fitness;
        }
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        for (int i = 0; i < 1000; ++i)
        {
            double r = dis(get) * fitnessSum;
            double sum = 0;
            for (int j = 0; j < 1000; ++j)
            {
                sum += allPopulation[j].fitness;
                if (sum >= r)
                {
                    allPopulation[i] = allPopulation[j];
                }
            }
        }
    }

    void crossOver()
    {
        std::uniform_int_distribution<> dId(0, 999);
        std::uniform_int_distribution<> dSize(0, 21);
        std::uniform_real_distribution<double> dRate(0.0, 1.0);
        for (int i = 0; i < 1000; ++i)
        {
            int id = dId(get);
            int size = dSize(get);
            double rate = dRate(get);
            if (rate <= crossRate)
            {
                allPopulation[i].cross(allPopulation[id], size);
            }
        }
    }

    void mutateOver()
    {
        std::uniform_real_distribution<double> dRate(0.0, 1.0);
        for (int i = 0; i < 1000; ++i)
        {
            double rate = dRate(get);
            if (rate <= mutateRate)
            {
                allPopulation[i].mutate();
            }
        }
    }

    individual getBest()
    {
        individual best = allPopulation[0];
        for (int i = 1; i < 1000; ++i)
        {
            best = (best.fitness < allPopulation[i].fitness) ? allPopulation[i] : best;
        }
        return best;
    }
};

void run()
{
    population p{};
    individual best;
    best.fitness = 0.0;
    for (int i = 0; i < maxGeneration; ++i)
    {
        p.calculateFitness();
        auto temp = p.getBest();
        std::cout << "Generation " << i + 1 << std::endl;
        best = (best.fitness > temp.fitness) ? best : temp;
        std::cout << "best finess = " << temp.fitness << std::endl;
        p.select();
        p.crossOver();
        p.mutateOver();
    }
    //std::cout << "best finess = " <<  best << std::endl;
    printf("x = %.10lf, fitness = %.10lf\n", best.getDecode(), best.fitness);
}

#endif