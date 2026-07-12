#include "guloso.hpp"
#include "mst.hpp"
#include "poda.hpp"

#include <limits>
#include <cmath>

Guloso::Guloso(Grafo *g, std::mt19937 &gerador) : rng(gerador) {

    grafo = g;
    custo = 0.0;
    melhorCusto = 0.0;
    mediaCustos = 0.0;
    custoAntesReligacao = 0.0;
    custoPosReligacao = 0.0;
    mediaAntesReligacao = 0.0;
    mediaPosReligacao = 0.0;
}

void Guloso::construir(double alfa){

    solucao.clear();

    int numVertices = grafo->getNumVertices();

    std::vector<bool> arvoreFinal(numVertices + 1, false);
    std::vector<double> melhorCustoLigacao(numVertices + 1, std::numeric_limits<double>::infinity());
    std::vector<int> origem(numVertices + 1, -1);

    int raiz = 1;
    double maiorPremio = grafo->getPremio(1);

    for (int i = 2; i <= numVertices; i++) {
        double premio = grafo->getPremio(i);
        if (premio > maiorPremio) {
            maiorPremio = premio;
            raiz = i;
        }
    }

    arvoreFinal[raiz] = true;

    for (const auto &vizinho : grafo->getVizinhos(raiz)) {
        int v = vizinho.first;
        double custoAresta = vizinho.second;

        if (custoAresta < melhorCustoLigacao[v]) {
            melhorCustoLigacao[v] = custoAresta;
            origem[v] = raiz;
        }
    }

    
    while (true)
    {
        std::vector<int> candidatos;
        double menorCusto = std::numeric_limits<double>::infinity();
        double maiorCusto = -std::numeric_limits<double>::infinity();

        for (int v = 1; v <= numVertices; v++) {
            if (arvoreFinal[v]) {
                continue;
            }
            if (melhorCustoLigacao[v] == std::numeric_limits<double>::infinity()) {
                continue;
            }

            candidatos.push_back(v);

            if (melhorCustoLigacao[v] < menorCusto) {
                menorCusto = melhorCustoLigacao[v];
            }
            if (melhorCustoLigacao[v] > maiorCusto) {
                maiorCusto = melhorCustoLigacao[v];
            }
        }

        if (candidatos.empty()) {
            break;
        }

        double limiar = menorCusto + alfa * (maiorCusto - menorCusto);

        std::vector<int> rcl;
        for (int v : candidatos) {
            if (melhorCustoLigacao[v] <= limiar) {
                rcl.push_back(v);
            }
        }

        std::uniform_int_distribution<int> dist(0, (int)rcl.size() - 1);
        int escolhido = rcl[dist(rng)];

        arvoreFinal[escolhido] = true;
        solucao.push_back(std::make_tuple(origem[escolhido], escolhido, melhorCustoLigacao[escolhido]));

        for (const auto &vizinho : grafo->getVizinhos(escolhido)) {
            int v = vizinho.first;
            double custoAresta = vizinho.second;

            if (!arvoreFinal[v] && custoAresta < melhorCustoLigacao[v]) {
                melhorCustoLigacao[v] = custoAresta;
                origem[v] = escolhido;
            }
        }
    }

    
    Poda poda(grafo, solucao);
    poda.podarForte();

    solucao = poda.getArestasPodadas();
    custo = poda.getCustoFinal();
    std::vector<bool> mantido = poda.getVerticesMantidos();
    posProcessamento = "poda";

    custoAntesReligacao = custo;

    MST religada(grafo, mantido);
    Poda podaReligada(grafo, religada.getArestasMST());
    podaReligada.podarForte();

    custoPosReligacao = podaReligada.getCustoFinal();

    if (podaReligada.getCustoFinal() < custo) {
        solucao = podaReligada.getArestasPodadas();
        custo = podaReligada.getCustoFinal();
        mantido = podaReligada.getVerticesMantidos();
        posProcessamento = "poda+mst";
    }

    verticesSolucao.clear();
    for (int v = 1; v <= numVertices; v++) {
        if (mantido[v]) {
            verticesSolucao.push_back(v);
        }
    }
}

void Guloso::executarGuloso(){

    construir(0.0);

    melhorSolucao = solucao;
    melhorVertices = verticesSolucao;
    melhorCusto = custo;
    mediaCustos = custo;
    melhorAlfa = 0.0;
    melhorPosProcessamento = posProcessamento;
    mediaAntesReligacao = custoAntesReligacao;
    mediaPosReligacao = custoPosReligacao;
    mediasPorAlfa.clear();
}

void Guloso::executarRandomizado(double alfa, int numIteracoes){

    melhorSolucao.clear();
    melhorCusto = std::numeric_limits<double>::infinity();
    mediaCustos = 0.0;
    melhorAlfa = alfa;
    mediaAntesReligacao = 0.0;
    mediaPosReligacao = 0.0;
    mediasPorAlfa.clear();

    for (int i = 0; i < numIteracoes; i++) {
        construir(alfa);

        mediaCustos += custo;
        mediaAntesReligacao += custoAntesReligacao;
        mediaPosReligacao += custoPosReligacao;

        if (custo < melhorCusto) {
            melhorCusto = custo;
            melhorSolucao = solucao;
            melhorVertices = verticesSolucao;
            melhorPosProcessamento = posProcessamento;
        }
    }

    mediaCustos /= numIteracoes;
    mediaAntesReligacao /= numIteracoes;
    mediaPosReligacao /= numIteracoes;
}

void Guloso::executarReativo(std::vector<double> alfas, int numIteracoes, int tamanhoBloco){

    int numAlfas = (int) alfas.size();

    melhorSolucao.clear();
    melhorCusto = std::numeric_limits<double>::infinity();
    mediaCustos = 0.0;
    melhorAlfa = alfas[0];
    mediaAntesReligacao = 0.0;
    mediaPosReligacao = 0.0;


    std::vector<double> probabilidade(numAlfas, 1.0 / numAlfas);
    std::vector<double> somaCusto(numAlfas, 0.0);
    std::vector<int> quantidade(numAlfas, 0);

    for (int i = 0; i < numIteracoes; i++) {

        std::discrete_distribution<int> sorteio(probabilidade.begin(), probabilidade.end());
        int indice = sorteio(rng);

        construir(alfas[indice]);

        somaCusto[indice] += custo;
        quantidade[indice]++;
        mediaCustos += custo;
        mediaAntesReligacao += custoAntesReligacao;
        mediaPosReligacao += custoPosReligacao;

        if (custo < melhorCusto) {
            melhorCusto = custo;
            melhorSolucao = solucao;
            melhorVertices = verticesSolucao;
            melhorAlfa = alfas[indice];
            melhorPosProcessamento = posProcessamento;
        }

        
        if ((i + 1) % tamanhoBloco == 0) {
            std::vector<double> qualidade(numAlfas);
            double somaQualidade = 0.0;

            for (int a = 0; a < numAlfas; a++) {
                double media = (quantidade[a] > 0) ? somaCusto[a] / quantidade[a] : melhorCusto;

                
                qualidade[a] = std::pow(melhorCusto / media, 10.0);
                somaQualidade += qualidade[a];
            }

            for (int a = 0; a < numAlfas; a++) {
                probabilidade[a] = qualidade[a] / somaQualidade;
            }
        }
    }

    mediaCustos /= numIteracoes;
    mediaAntesReligacao /= numIteracoes;
    mediaPosReligacao /= numIteracoes;

    // media de custo obtida por cada alfa, na mesma ordem do vetor de alfas
    mediasPorAlfa.assign(numAlfas, 0.0);
    for (int a = 0; a < numAlfas; a++) {
        if (quantidade[a] > 0) {
            mediasPorAlfa[a] = somaCusto[a] / quantidade[a];
        }
    }
}

std::vector<std::tuple<int, int, double>> Guloso::getMelhorSolucao(){
    return melhorSolucao;
}

std::vector<int> Guloso::getMelhorVertices(){
    return melhorVertices;
}

double Guloso::getMelhorCusto(){
    return melhorCusto;
}

double Guloso::getMediaCustos(){
    return mediaCustos;
}

double Guloso::getMelhorAlfa(){
    return melhorAlfa;
}

std::string Guloso::getMelhorPosProcessamento(){
    return melhorPosProcessamento;
}

double Guloso::getMediaAntesReligacao(){
    return mediaAntesReligacao;
}

double Guloso::getMediaPosReligacao(){
    return mediaPosReligacao;
}

std::vector<double> Guloso::getMediasPorAlfa(){
    return mediasPorAlfa;
}
