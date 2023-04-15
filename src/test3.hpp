#ifndef TEST3_HPP
#define TEST3_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <ctime>

const int inputNodes = 8000; // 输入层神经元数量，即32256维图像向量
const int hiddenNodes = 64;  // 隐藏层神经元的数量
const int outputNodes = 15;  // 输出层神经元数量，即YALE人脸数据库中人类数量

const double learningRate = 0.1;    // 学习率
const int maxEpochs = 1000;         // 最大训练轮数
const double targetAccuracy = 0.95; // 目标准确率

// 用于神经元的输出
double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

// sigmoid函数的导数，用于误差的反向传播 x = sigmoid
double sigmoidDerivative(double x)
{
    return x * (1.0 - x);
}

double getRandom(double x)
{
    std::random_device seed;
    std::mt19937 get(seed());
    std::uniform_real_distribution<double> dis(0.0, x);
    return dis(get);
}

class neuralNetwork
{
public:
    neuralNetwork();
    void train();
    void test();
    void saveWeightAndThreshold();
    void readWeightAndThreshold();
    double getAccuracy() { return this->accuracy; }

private:
    std::vector<std::vector<double>> inputWeights;  // 输入层到隐藏层的权重矩阵
    std::vector<double> hiddenBiases;               // 隐藏层神经元的偏置值
    std::vector<std::vector<double>> hiddenWeights; // 隐藏层到输入层的权重矩阵
    std::vector<double> outputBiases;               // 输出层神经元的偏置值
private:
    std::vector<double> feedForward(std::vector<double> input);                        // 前向传播
    void backpropagation(std::vector<double> input, std::vector<double> targetOutput); // 反向传播
    std::vector<double> bmpToVector(char *fileName);
    double accuracy = 0.0;
};

neuralNetwork::neuralNetwork()
{
    inputWeights.resize(inputNodes);
    for (int i = 0; i < inputNodes; ++i)
    {
        inputWeights[i].resize(hiddenNodes);
        for (int j = 0; j < hiddenNodes; ++j)
        {
            inputWeights[i][j] = getRandom(0.0001);
        }
    }

    hiddenBiases.resize(hiddenNodes);
    for (int i = 0; i < hiddenNodes; ++i)
    {
        hiddenBiases[i] = getRandom(0.0001);
    }

    hiddenWeights.resize(hiddenNodes);
    for (int i = 0; i < hiddenNodes; ++i)
    {
        hiddenWeights[i].resize(outputNodes);
        for (int j = 0; j < outputNodes; ++j)
        {
            hiddenWeights[i][j] = getRandom(0.0001);
        }
    }

    outputBiases.resize(outputNodes);
    for (int i = 0; i < outputNodes; ++i)
    {
        outputBiases[i] = getRandom(0.0001);
    }
}

std::vector<double> neuralNetwork::feedForward(std::vector<double> input)
{
    std::vector<double> hiddenOutput(hiddenNodes);
    for (int i = 0; i < hiddenNodes; ++i)
    {
        // 计算隐藏层的输出
        double activation = hiddenBiases[i];
        for (int j = 0; j < inputNodes; ++j)
        {
            activation += input[j] * inputWeights[j][i];
        }
        hiddenOutput[i] = sigmoid(activation);
    }
    // 计算输出层的输出
    std::vector<double> output(outputNodes);
    for (int i = 0; i < outputNodes; i++)
    {
        double activation = outputBiases[i];
        for (int j = 0; j < hiddenNodes; ++j)
        {
            activation += hiddenOutput[j] * hiddenWeights[j][i];
        }
        output[i] = sigmoid(activation);
    }
    return output;
}

void neuralNetwork::backpropagation(std::vector<double> input, std::vector<double> targetOutput)
{
    // 计算隐藏层和输出层的误差
    std::vector<double> hiddenOutput(hiddenNodes);
    for (int i = 0; i < hiddenNodes; ++i)
    {
        double activation = hiddenBiases[i];
        for (int j = 0; j < inputNodes; ++j)
        {
            activation += input[j] * inputWeights[j][i];
        }
        hiddenOutput[i] = sigmoid(activation);
    }

    std::vector<double> output(outputNodes);
    for (int i = 0; i < outputNodes; ++i)
    {
        double activation = outputBiases[i];
        for (int j = 0; j < hiddenNodes; ++j)
        {
            activation += hiddenOutput[j] * hiddenWeights[j][i];
        }
        output[i] = sigmoid(activation);
    }
    // for(auto &e : output)
    // {
    //     std::cout << e << std::endl;
    // }
    // std::cout << std::endl;
    std::vector<double> outputError(outputNodes);
    for (int i = 0; i < outputNodes; ++i)
    {
        outputError[i] = (targetOutput[i] - output[i]) * sigmoidDerivative(output[i]);
    }

    std::vector<double> hiddenError(hiddenNodes);

    for (int i = 0; i < hiddenNodes; ++i)
    {
        double error = 0.0;
        for (int j = 0; j < outputNodes; ++j)
        {
            error += outputError[j] * hiddenWeights[i][j];
        }
        hiddenError[i] = error * sigmoidDerivative(hiddenOutput[i]);
    }
    // for (auto &e : outputError)
    // {
    //     std::cout << e << std::endl;
    // }
    // std::cout << std::endl;
    // 更新权重和偏置值
    for (int i = 0; i < outputNodes; ++i)
    {
        outputBiases[i] += learningRate * outputError[i];
        for (int j = 0; j < hiddenNodes; ++j)
        {
            hiddenWeights[j][i] += learningRate * hiddenOutput[j] * outputError[i];
        }
    }

    for (int i = 0; i < hiddenNodes; ++i)
    {
        hiddenBiases[i] += learningRate * hiddenError[i];
        for (int j = 0; j < inputNodes; ++j)
        {
            inputWeights[j][i] += learningRate * input[j] * hiddenError[i];
        }
    }
}

std::vector<double> neuralNetwork::bmpToVector(char *filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (file.is_open())
    {
        file.seekg(54, std::ios::beg);
        std::vector<char> vec(8000);
        file.read(vec.data(), 8000);
        std::vector<double> input(8000);
        for (int i = 0; i < 8000; ++i)
        {
            input[i] = vec[i] / 255.0;
            // std::cout << input[i] << std::endl;
        }
        return input;
    }
    else
    {
        std::cerr << "can't open the file" << std::endl;
        exit(1);
    }
}

void neuralNetwork::train()
{
    for (int i = 1; i <= 5; ++i)
    {
        for (int j = 1; j <= 15; ++j)
        {
            char *path = new char[50]{};
            if (j < 10)
            {
                sprintf(path, "testImage\\subject0%d_%d.bmp", j, i);
            }
            else
            {
                sprintf(path, "testImage\\subject%d_%d.bmp", j, i);
            }
            std::vector<double> input = bmpToVector(path);
            std::vector<double> target(outputNodes, 0);
            target[j - 1] = 1;
            backpropagation(input, target);
        }
    }
}

void neuralNetwork::saveWeightAndThreshold()
{
    std::ofstream writeFile("WeightAndThreshold.txt", std::ios::out & std::ios::app);
    if (writeFile.is_open())
    {
        for (int i = 0; i < inputNodes; ++i)
        {
            for (int j = 0; j < hiddenNodes; ++j)
            {
                writeFile << inputWeights[i][j] << " ";
            }
            writeFile << std::endl;
        }
        writeFile << std::endl;
        for (int i = 0; i < hiddenNodes; ++i)
        {
            writeFile << hiddenBiases[i] << " ";
        }
        writeFile << std::endl;
        for (int i = 0; i < hiddenNodes; ++i)
        {
            for (int j = 0; j < outputNodes; ++j)
            {
                writeFile << hiddenWeights[i][j] << " ";
            }
            writeFile << std::endl;
        }
        writeFile << std::endl;
        for (int i = 0; i < outputNodes; ++i)
        {
            writeFile << outputBiases[i] << " ";
        }
        writeFile << std::endl;
    }
    else
    {
        std::cerr << "can not open writeFile !" << std::endl;
        exit(1);
    }
}

void neuralNetwork::readWeightAndThreshold()
{
    std::ifstream readFile("WeightAndThreshold.txt", std::ios::in);
    if (readFile.is_open())
    {
        for (int i = 0; i < inputNodes; ++i)
        {
            for (int j = 0; j < hiddenNodes; ++j)
            {
                readFile >> inputWeights[i][j];
            }
        }
        for (int i = 0; i < hiddenNodes; ++i)
        {
            readFile >> hiddenBiases[i];
        }
        for (int i = 0; i < hiddenNodes; ++i)
        {
            for (int j = 0; j < outputNodes; ++j)
            {
                readFile >> hiddenWeights[i][j];
            }
        }
        for (int i = 0; i < outputNodes; ++i)
        {
            readFile >> outputBiases[i];
        }
    }
    else
    {
        std::cerr << "can not open readFile !" << std::endl;
        exit(1);
    }
}

void neuralNetwork::test()
{
    double total = 0.0;
    double correct = 0.0;
    for (int i = 6; i <= 11; ++i)
    {
        for (int j = 1; j <= 15; ++j)
        {
            ++total;
            char *path = new char[50]{};
            if (j < 10)
            {
                sprintf(path, "testImage\\subject0%d_%d.bmp", j, i);
            }
            else
            {
                sprintf(path, "testImage\\subject%d_%d.bmp", j, i);
            }
            std::vector<double> input = bmpToVector(path);
            std::vector<double> output = feedForward(input);
            int id = 0;
            for (int i = 1; i < 15; ++i)
            {
                if (output[id] < output[i])
                    id = i;
            }
            std::cout << "this is the " << id + 1 << "th person!"
                      << " " << output[id] << std::endl;
            if (id + 1 == j)
                ++correct;
        }
    }
    this->accuracy = correct / total;
}

void runtest()
{
    neuralNetwork n{};
    int epochsCount = 0;
    // while (epochsCount != maxEpochs)
    // {
    //     epochsCount++;
    //     std::cout << epochsCount << std::endl;
    //     n.train();
    // }
    // n.saveWeightAndThreshold();
    n.readWeightAndThreshold();
    n.test();
    std::cout << "Recognition accuracy is " << n.getAccuracy() << std::endl;
}
#endif