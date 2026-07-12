#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <tuple>
#include <limits>
#include <random>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <filesystem>

#include "grafo.hpp"
#include "guloso.hpp"


std::string numeroCurto(double valor)
{
    std::ostringstream texto;
    texto << std::setprecision(15) << valor;
    return texto.str();
}


std::string formataTempo(double segundos)
{
    std::ostringstream texto;
    texto << std::fixed << std::setprecision(3) << segundos;
    return texto.str();
}


void gravaCsv(const std::string &caminho, const std::string &instancia, const std::string &algoritmo,
              const std::string &alfa, const std::string &numIteracoes, const std::string &tamanhoBloco,
              unsigned seed, double tempoSegundos, double melhorCusto, double mediaCustos,
              const std::string &melhorAlfa)
{
    bool arquivoNovo;
    {
        std::ifstream teste(caminho);
        arquivoNovo = !teste.good() || teste.peek() == std::ifstream::traits_type::eof();
    }

    std::ofstream csv(caminho, std::ios::app);

    if (arquivoNovo) {
        csv << "data_hora,instancia,algoritmo,alfa,num_iteracoes,tamanho_bloco,seed,tempo_seg,melhor_custo,media_custos,melhor_alfa\n";
    }

    std::time_t agora = std::time(nullptr);
    char dataHora[20];
    std::strftime(dataHora, sizeof(dataHora), "%Y-%m-%d %H:%M:%S", std::localtime(&agora));

    csv << dataHora << "," << instancia << "," << algoritmo << ","
        << alfa << "," << numIteracoes << "," << tamanhoBloco << "," << seed << ","
        << formataTempo(tempoSegundos) << ","
        << numeroCurto(melhorCusto) << "," << numeroCurto(mediaCustos) << "," << melhorAlfa << "\n";
}


void imprimeSolucao(const std::vector<std::tuple<int, int, double>> &arestas,
                    const std::vector<int> &vertices)
{
    for (const auto &aresta : arestas) {
        std::cout << std::get<0>(aresta) << " " << std::get<1>(aresta) << " " << std::get<2>(aresta) << "\n";
    }

    if (arestas.empty()) {
        for (int v : vertices) {
            std::cout << v << "\n";
        }
    }
}

void executaInstancia(const std::string &instancia, std::mt19937 &rng, unsigned seed,
                      double alfaRandomizado, int iteracoesRandomizado,
                      const std::vector<double> &alfasReativo, int iteracoesReativo,
                      int tamanhoBloco, const std::string &arquivoCsv)
{
    Grafo g;
    g.carregaArquivo(instancia);

    std::cout << std::endl << " Instancia: " << instancia << " ("
              << g.getNumVertices() << " vertices, seed " << seed << ")" << std::endl;

    Guloso guloso(&g, rng);


    double melhorCustoGeral = std::numeric_limits<double>::infinity();
    std::string melhorAlgoritmo;
    std::vector<std::tuple<int, int, double>> melhoresArestas;
    std::vector<int> melhoresVertices;

    // ---------- guloso ----------
    auto inicio = std::chrono::steady_clock::now();
    guloso.executarGuloso();
    double tempo = std::chrono::duration<double>(std::chrono::steady_clock::now() - inicio).count();

    std::cout << std::endl << "Guloso" << std::endl;
    std::cout << "  custo: " << guloso.getMelhorCusto() << "   tempo: "
              << formataTempo(tempo) << "s" << std::endl;

    gravaCsv(arquivoCsv, instancia, "guloso", "", "", "", seed, tempo,
             guloso.getMelhorCusto(), guloso.getMediaCustos(), "");

    if (guloso.getMelhorCusto() < melhorCustoGeral) {
        melhorCustoGeral = guloso.getMelhorCusto();
        melhorAlgoritmo = "guloso";
        melhoresArestas = guloso.getMelhorSolucao();
        melhoresVertices = guloso.getMelhorVertices();
    }

    // ---------- guloso randomizado ----------
    inicio = std::chrono::steady_clock::now();
    guloso.executarRandomizado(alfaRandomizado, iteracoesRandomizado);
    tempo = std::chrono::duration<double>(std::chrono::steady_clock::now() - inicio).count();

    std::cout << std::endl << "Guloso randomizado (alfa = " << alfaRandomizado
              << ", " << iteracoesRandomizado << " iteracoes)" << std::endl;
    std::cout << "  melhor custo: " << guloso.getMelhorCusto()
              << "   media: " << guloso.getMediaCustos() << "   tempo: "
              << formataTempo(tempo) << "s" << std::endl;

    gravaCsv(arquivoCsv, instancia, "randomizado", numeroCurto(alfaRandomizado),
             std::to_string(iteracoesRandomizado), "", seed, tempo,
             guloso.getMelhorCusto(), guloso.getMediaCustos(), "");

    if (guloso.getMelhorCusto() < melhorCustoGeral) {
        melhorCustoGeral = guloso.getMelhorCusto();
        melhorAlgoritmo = "randomizado";
        melhoresArestas = guloso.getMelhorSolucao();
        melhoresVertices = guloso.getMelhorVertices();
    }

    // ---------- guloso randomizado reativo ----------
    std::string listaAlfas;
    for (size_t i = 0; i < alfasReativo.size(); i++) {
        if (i > 0) {
            listaAlfas += ";";
        }
        listaAlfas += numeroCurto(alfasReativo[i]);
    }

    inicio = std::chrono::steady_clock::now();
    guloso.executarReativo(alfasReativo, iteracoesReativo, tamanhoBloco);
    tempo = std::chrono::duration<double>(std::chrono::steady_clock::now() - inicio).count();

    std::cout << std::endl << "Guloso randomizado reativo (alfas = " << listaAlfas << ", "
              << iteracoesReativo << " iteracoes, blocos de " << tamanhoBloco << ")" << std::endl;
    std::cout << "  melhor custo: " << guloso.getMelhorCusto()
              << "   media: " << guloso.getMediaCustos()
              << "   alfa da melhor: " << guloso.getMelhorAlfa() << "   tempo: "
              << formataTempo(tempo) << "s" << std::endl;

    gravaCsv(arquivoCsv, instancia, "reativo", listaAlfas,
             std::to_string(iteracoesReativo), std::to_string(tamanhoBloco), seed, tempo,
             guloso.getMelhorCusto(), guloso.getMediaCustos(), numeroCurto(guloso.getMelhorAlfa()));

    if (guloso.getMelhorCusto() < melhorCustoGeral) {
        melhorCustoGeral = guloso.getMelhorCusto();
        melhorAlgoritmo = "reativo";
        melhoresArestas = guloso.getMelhorSolucao();
        melhoresVertices = guloso.getMelhorVertices();
    }

    // ---------- resumo ----------
    std::cout << std::endl << "Melhor solucao geral: " << melhorAlgoritmo
              << " (custo " << melhorCustoGeral << ", " << melhoresVertices.size()
              << " vertices, " << melhoresArestas.size() << " arestas)" << std::endl;
    std::cout << "\n Arvore da solucao" << std::endl;
    imprimeSolucao(melhoresArestas, melhoresVertices);
}

int main(int argc, char *argv[])
{
    // ------------------- configuracao (valores padrao) -------------------
    std::string instancia = "";
    std::string seedParametro = "";

    double alfaRandomizado = 0.1;
    int iteracoesRandomizado = 30;

    std::vector<double> alfasReativo = {0.1, 0.2, 0.3};
    int iteracoesReativo = 300;
    int tamanhoBloco = 30;

    std::string arquivoCsv = "resultados.csv";
    // ---------------------------------------------------------------------

    // argumentos da linha de comando (sobrescrevem os padroes)
    for (int a = 1; a < argc; a++) {
        std::string arg = argv[a];
        if (arg == "-i" && a + 1 < argc) {
            instancia = argv[++a];
        } else if (arg == "-s" && a + 1 < argc) {
            seedParametro = argv[++a];
        } else if (arg == "-h") {
            std::cout << "Uso: ./pcstp [-i instancia.stp] [-s seed]" << std::endl;
            std::cout << "  -i   caminho do arquivo .stp (vazio = todas as instancias)" << std::endl;
            std::cout << "  -s   seed de randomizacao       (vazio = horario atual)" << std::endl;
            std::cout << "  -h   esta ajuda" << std::endl;
            std::cout << std::endl;
            std::cout << "Demais parametros (alfa, iteracoes, etc.) devem ser alterados" << std::endl;
            std::cout << "no bloco \"configuracao\" no inicio da funcao main() em main.cpp." << std::endl;
            return 0;
        }
    }

    unsigned seed;
    if (seedParametro.empty()) {
        seed = (unsigned) std::time(nullptr);
    } else {
        seed = (unsigned) std::stoul(seedParametro);
    }

    std::cout << "seed utilizada: " << seed << std::endl;

    std::mt19937 rng(seed);

    std::vector<std::string> instancias;

    if (instancia.empty()) {
        for (const std::string pasta : {"./PCSPG-CRR", "./PCSPG-JMP"}) {
            if (!std::filesystem::exists(pasta)) {
                std::cout << "aviso: pasta " << pasta << " nao encontrada, pulando" << std::endl;
                continue;
            }
            for (const auto &entrada : std::filesystem::directory_iterator(pasta)) {
                if (entrada.path().extension() == ".stp") {
                    instancias.push_back(entrada.path().string());
                }
            }
        }
        
        std::sort(instancias.begin(), instancias.end());
    } else {
        instancias.push_back(instancia);
    }

    unsigned seedInstancia = seed;
    rng.seed(seedInstancia);
    
    for (size_t i = 0; i < instancias.size(); i++) {
        executaInstancia(instancias[i], rng, seedInstancia, alfaRandomizado, iteracoesRandomizado,
                         alfasReativo, iteracoesReativo, tamanhoBloco, arquivoCsv);
    }

    std::cout << std::endl << instancias.size() << " instancia(s) executada(s), resultados registrados em "
              << arquivoCsv << std::endl;

    return 0;
}
