#ifndef GULOSO_HPP
#define GULOSO_HPP

#include "grafo.hpp"

#include <vector>
#include <tuple>
#include <random>

class Guloso
{
    Grafo *grafo;
    std::mt19937 &rng;

    
    std::vector<std::tuple<int, int, double>> solucao;
    std::vector<int> verticesSolucao;
    double custo;
    std::string posProcessamento;

    
    std::vector<std::tuple<int, int, double>> melhorSolucao;
    std::vector<int> melhorVertices;
    double melhorCusto;
    double mediaCustos;
    double melhorAlfa;
    std::string melhorPosProcessamento;

    void construir(double alfa);

public:
    Guloso(Grafo *grafo, std::mt19937 &rng);

    void executarGuloso();
    void executarRandomizado(double alfa, int numIteracoes);
    void executarReativo(std::vector<double> alfas, int numIteracoes, int tamanhoBloco);

    std::vector<std::tuple<int, int, double>> getMelhorSolucao();
    std::vector<int> getMelhorVertices();
    double getMelhorCusto();
    double getMediaCustos();
    double getMelhorAlfa();
    std::string getMelhorPosProcessamento();
};

#endif
