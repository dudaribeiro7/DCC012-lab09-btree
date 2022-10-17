#include <iostream>
#include <chrono>
using namespace std;
#include "btree.h"
#include <unistd.h>

typedef chrono::high_resolution_clock Clock;


int randint(int min, int max) {
    return min + rand() % (max - min + 1);
}


void print_help(char * argv)
{
    cout << "Uso: " << argv << " -o [ordem] -n [numero de inserção] " << endl;
    cout << "Exemplo: " << argv << " -o 5 -n 100" << endl;  
    exit(0);
}


int main(int argc, char* argv[]) {
    

    int ordem        = 2;
    int num_insercao = 5;
    char c; 

    while((c = getopt(argc, argv, "o:n:")) != -1) {
        switch(c) {
            case 'o':
                ordem = atoi(optarg);
                break;
            case 'n':
                num_insercao = atoi(optarg);
                break;
            default:
                print_help(argv[0]);
                break;
        }
    }

    BTree<int> btree(ordem);
    double elapsed_seconds_insert = 0;
    double elapsed_seconds_remove = 0;
    double elapsed_seconds_search = 0;
    
    // TODO: descomente para testar a inserção de elementos 
    cout << "Inserting keys..." << endl;
    for (int i = 0; i < num_insercao; i++) {
        auto start = Clock::now();
        btree.insert(i);
        auto end = Clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        elapsed_seconds_insert += elapsed_seconds.count();
    }
    
    // TODO: descomente para testar a impressão da árvore:
    cout << "Printing tree..." << endl;
    btree.print();

    // TODO: descomente para testar a remoção de elementos 
    cout << "Removing keys..." << endl;
    for (int i = 0; i < num_insercao/4; i++) 
    {
        int key = randint(0, num_insercao);
        cout << "Removing key " << key << endl;
        auto start = Clock::now();
        btree.remove(key);
        auto end = Clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        elapsed_seconds_remove +=elapsed_seconds.count();
    }

    // TODO: descomente para testar a pesquisa de elementos 
    cout << "Searching keys..." << endl;
    for (int i = 0; i < num_insercao/2; i++)
    {
        int key = randint(0, num_insercao);
        cout << "Removing key " << key << endl;
        auto start = Clock::now();
        btree.remove(key);
        auto end = Clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        elapsed_seconds_remove += elapsed_seconds.count();
    }

    cout << "Printing tree..." << endl;
    btree.print();

    cout << "B-Tree order: " << ordem << endl;
    cout << "B-Tree size: "  << num_insercao << endl;
    cout << "Insertion CPU Time: " << elapsed_seconds_insert << " seconds" << endl;
    cout << "Removal CPU Time: " << elapsed_seconds_remove << " seconds" << endl;
    cout << "Search CPU Time: " << elapsed_seconds_search << " seconds" << endl;

    return 0;
}