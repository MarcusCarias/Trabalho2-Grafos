#ifndef PODA_HPP
#define PODA_HPP

#include "grafo.hpp"

#include <vector>
#include <tuple>

class Poda
{
    Grafo *grafo;

    std::vector<std::tuple<int, int, double>> arestas;
    std::vector<std::tuple<int, int, double>> arvoreFinal;
    std::vector<bool> mantido;
    double custoFinal;

    
    std::vector<std::vector<std::pair<int, double>>> adjArvore;
    std::vector<double> ganho;
    std::vector<int> pai;
    std::vector<bool> visitado;

    void calculaGanho(int u, int p);
    void coletaMantidas(int u, int p);

public:
    Poda(Grafo *grafo, std::vector<std::tuple<int, int, double>> arvore);

    void podar();
    void podarForte();

    double getCustoFinal();
    std::vector<std::tuple<int, int, double>> getArestasPodadas();
    std::vector<bool> getVerticesMantidos();
};

#endif
