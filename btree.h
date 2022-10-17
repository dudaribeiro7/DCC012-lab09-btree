#ifndef BTREE_H__
#define BTREE_H__

#include <iostream>
#include <fstream>
using namespace std;

#include <vector>

template<typename T> class BTree;

// TODO: Implementar a  classe BNode
//       Use friend 
template<typename T>
class BNode 
{
    friend class BTree<T>;

    private:
        int m;              //número máximo de filhos do nó
        T *chaves;          //array com as m-1 chaves do nó
        BNode **filhos;     //array de ponteiros para os m filhos
        int n;              //número de chaves presentes no nó
        bool folha;         //indica se o nó é folha ou não

    public:
        BNode(int _m, bool _folha)
        {
            m = _m;
            folha = _folha;
            chaves = new T[2*m-1];
            filhos = new BNode*[2*m];
            n = 0;
        }
        
        ~BNode()
        {
        }

        T*      getChaves(){ return chaves; }
        BNode** getFilhos(){ return filhos; }
        int     getN()     { return n;      }
        bool    isFolha()  { return folha;  }

        // Função para dividir o filho y do nó 
        // i é o índice de y no array filhos
        // O filho y deve estar cheio quando esta função é chamada
        void splitChild(int i, BNode<T> *y){
            BNode<T> *z = new BNode<T>(y->m, y->folha);
            z->n = m - 1;

            for(int j=0; j<m-1; j++)
                z->chaves[j] = y->chaves[j+m];
            
            if(y->folha == false)
                for(int j=0; j<m; j++)
                    z->filhos[j] = y->filhos[j+m];
            
            y->n = m - 1;

            for(int j=n; j>=i+1; j--)
                filhos[j+1] = filhos[j];
            
            filhos[i+1] = z;

            for(int j=n-1; j>=i; j--)
                chaves[j+1] = chaves[j];
            
            chaves[i] = y->chaves[m-1];

            n++;
        }

        // Função para inserir uma nova chave no nó
        // A suposição é que o nó não deve estar cheio quando a função é chamada
        void insertNonFull(T k)
        {
            int i = n-1;
            
            // verifica se é um nó folha:
            if(folha)
            {
                // O loop a seguir faz duas coisas
                // a) Encontra a localização da nova chave a ser inserida
                // b) Move todas as chaves maiores para um lugar à frente  
                while (i>=0 && chaves[i]>k)
                {
                    chaves[i+1] = chaves[i];
                    i--;
                }

                // insere a nova chave na localização encontrada:
                chaves[i+1] = k;
                n++;
            }
            else
            {
                // encontra o filho que terá a nova chave:
                while(i>=0 && chaves[i] > k)
                    i--;
                
                // verifica se o filho encontrado está cheio:
                if(filhos[i+1]->n == 2*m-1)
                {
                    // se estiver cheio, parte o filho:
                    splitChild(i+1, filhos[i+1]);

                    // depois de partir, a chave mediana de filhos[i] sobe e filhos[i] é separado em dois.
                    // verifica qual filho ficará com a nova chave:
                    if(chaves[i+1] < k)
                        i++;
                }
                filhos[i+1]->insertNonFull(k);
            }
        }

        //Função para procurar a chave k nas subarvores de um nó: 
        bool searchNode(T k)
        {
            // encontra a primeira chave maior ou igual a k:
            int i = 0;
            while(i < n && k > chaves[i])
                i++;
            
            // se a chave encontrada é igual a k, retorna true:
            if(chaves[i] == k)
                return true;
            
            // se a chave nao for encontrada nesse nó, e é uma folha, retorna false:
            if(folha)
            {
                cout << "Esta chave não está presente na árvore" << endl;
                return false;
            }
            
            // se esse nó não é folha, desce para o próximo filho:
            return filhos[i]->searchNode(k);
        }

        //Função que retorna o index da primeira chave maior ou igual a k:
        int findKey(T k){
            int idx = 0;
            while(idx < n && chaves[idx] < k)
            {
                idx++;
            }
            return idx;
        }

        //Função para remover a chave presente na posição idx no nó, que é folha:
        void removeFromLeaf(int idx){
            for(int i = idx + 1; i < n; i++)
                chaves[i-1] = chaves[i];
            
            n--;
        }

        //Função para remover a chave presente na posição idx no nó, que não é folha:
        void removeFromNonLeaf(int idx){
            int k = chaves[idx];

            if(filhos[idx]->n >= m)
            {
                T pred = getPred(idx);
                chaves[idx] = pred;
                filhos[idx]->remove(pred);
            }
            else
            {
                if(filhos[idx+1]->n >= m)
                {
                    int suc = getSuc(idx);
                    chaves[idx] = suc;
                    filhos[idx+1]->remove(suc);
                }
                else
                {
                    merge(idx);
                    filhos[idx]->remove(k);
                }
            }
        }

        //Função para resgatar o predecessor da chave presente na posição idx do nó:
        T getPred(int idx){
            BNode *aux = filhos[idx];
            while(!aux->folha)
                aux = aux->filhos[aux->n];

            return aux->chaves[aux->n - 1];
        }

        //Função para resgatar o sucessor da chave presente na posição idx do nó:
        T getSuc(int idx){
            BNode *aux = filhos[idx+1];
            while(!aux->folha)
                aux = aux->filhos[0];

            return aux->chaves[0];
        }

        void fill(int idx){
            if (idx != 0 && filhos[idx - 1]->n >= m)
                borrowFromPrev(idx);

            else 
            {
                if (idx != n && filhos[idx + 1]->n >= m)
                    borrowFromNext(idx);
                else 
                {
                    if (idx != n)
                        merge(idx);
                    else
                        merge(idx - 1);
                }
            }
        }

        //Função que "pega emprestado" uma chave do nó filhos[idx-1] e coloca no nó filhos[idx]:
        void borrowFromPrev(int idx){
            BNode *filho = filhos[idx];
            BNode *irmao = filhos[idx - 1];

            for (int i = filho->n - 1; i >= 0; --i)
                filho->chaves[i + 1] = filho->chaves[i];

            if (!filho->folha) {
                for (int i = filho->n; i >= 0; --i)
                filho->filhos[i + 1] = filho->filhos[i];
            }

            filho->chaves[0] = chaves[idx - 1];

            if (!filho->folha)
                filho->filhos[0] = irmao->filhos[irmao->n];

            chaves[idx - 1] = irmao->chaves[irmao->n - 1];

            filho->n += 1;
            irmao->n -= 1;
        }

        //Função que "pega emprestado" uma chave do nó filhos[idx+1] e coloca no nó filhos[idx]:
        void borrowFromNext(int idx){
            BNode *filho = filhos[idx];
            BNode *irmao = filhos[idx + 1];

            filho->chaves[(filho->n)] = chaves[idx];

            if (!(filho->folha))
                filho->filhos[(filho->n) + 1] = irmao->filhos[0];

            chaves[idx] = irmao->chaves[0];

            for (int i = 1; i < irmao->n; ++i)
                irmao->chaves[i - 1] = irmao->chaves[i];

            if (!irmao->folha) {
                for (int i = 1; i <= irmao->n; ++i)
                irmao->filhos[i - 1] = irmao->filhos[i];
            }

            filho->n += 1;
            irmao->n -= 1;
        }

        //Função para juntar o nó filho de posição idx com o nó filho de posição idx+1:
        void merge(int idx){
            BNode *filho = filhos[idx];
            BNode *irmao = filhos[idx + 1];

            filho->chaves[m - 1] = chaves[idx];

            for (int i = 0; i < irmao->n; ++i)
                filho->chaves[i + m] = irmao->chaves[i];

            if (!filho->folha) {
                for (int i = 0; i <= irmao->n; ++i)
                filho->filhos[i + m] = irmao->filhos[i];
            }

            for (int i = idx + 1; i < n; ++i)
                chaves[i - 1] = chaves[i];

            for (int i = idx + 2; i <= n; ++i)
                filhos[i - 1] = filhos[i];

            filho->n += irmao->n + 1;
            n--;

            delete (irmao);
        }

        //Função para remover a chave k nas subarvores do nó:
        void remove(T k){
            int idx = findKey(k);

            if(idx < n && chaves[idx] == k)
            {
                if(folha)
                    removeFromLeaf(idx);
                else
                    removeFromNonLeaf(idx);
            }
            else
            {
                if(folha)
                {
                    cout << "A chave " << k << " não existe na árvore." << endl;
                    return;
                }
                bool flag = ((idx == n) ? true : false);

                if(filhos[idx]->n < m)
                    fill(idx);
                
                if(flag && idx > n)
                    filhos[idx - 1]->remove(k);
                else
                    filhos[idx]->remove(k);
            }
        }

};

// TODO: Implementar a classe BTree
//  Os seguintes métodos deve ser implementados:
//   - BTree(int ordem): Cria uma Btree com a ordem passada como parâmetro.
//   - ~BTree(): Destrutor da Btree.
//   - void insert(T element): Insere o elemento passado como parâmetro na Btree.
//   - void remove(T element): Remove o elemento passado como parâmetro da Btree.
//   - bool search(T element): Retorna true se o elemento passado como parâmetro está na Btree, false caso contrário.
//   - void print(): Imprime a Btree in-order
template<typename T>
class BTree 
{
    private:
        int m;           //ordem da árvore B
        BNode<T> *root;  //raiz da árvore B

    public:
        //Cria uma Btree vazia com a ordem passada como parâmetro:
        BTree(int ordem)
        {
            m = ordem;
            root = nullptr;
        } 

        // Destrutor da Btree:
        ~BTree()
        {
            deallocate(root);
        }

        //Auxiliar para o destrutor:
        void deallocate(BNode<T>* node)
        {
            if (node == nullptr)
            {
                return;
            }
            for(int i=0; i<=node->n; i++){
                deallocate(node->filhos[i]);
            }
            delete [] node->chaves;
            delete node;
            node = nullptr;
        }

        //Verifica se a árvore está vazia:
        bool isVazia()
        {
            if(root == nullptr)
                return true;
            else
                return false;
        }

        //Insere o elemento passado como parâmetro na Btree:
        void insert(T element)
        {
            // verifica se a árvore está vazia:
            if(isVazia())
            {
                // aloca memória para a raiz:
                root = new BNode<T>(m, true);
                // insere a chave:
                root->chaves[0] = element;
                // atualiza numero de chaves na raiz:
                root->n = 1;
            }
            else
            {
                // verifica se a raiz está cheia:
                if(root->n == 2*m-1)
                {
                    // aloca memória para nova raiz:
                    BNode<T> *new_root = new BNode<T>(m, false);
                    // faz a raiz antiga ser filha da nova raiz
                    new_root->filhos[0] = root;
                    // parte a antiga raiz e move 1 chave para a nova raiz:
                    new_root->splitChild(0, root);

                    // A nova raiz tem dois filhos agora. 
                    // Decida qual dos dois terá uma nova chave
                    int i = 0;
                    if(new_root->chaves[0] < element)
                        i++;
                    new_root->filhos[i]->insertNonFull(element);

                    // troca a raiz:
                    root = new_root;
                }
                else
                {
                    root->insertNonFull(element);
                }
            }
        }

        //Remove o elemento passado como parâmetro da Btree:
        void remove(T element)
        {
            if(isVazia())
                return;
            root->remove(element);

            if(root->n == 0)
            {
                BNode<T> *tmp = root;
                if(root->isFolha())
                    root = NULL;
                else 
                    root = root->filhos[0];
                delete tmp;
            }
        }

        //Retorna true se o elemento passado como parâmetro está na Btree, false caso contrário:
        bool search(T element)
        {
            // se estiver vazia, retorna false:
            if(isVazia())
                return false;
            // chama a função auxiliar searchNode para a raiz:
            return root->searchNode(element);
        }

        //Imprime a Btree in-order:
        void print()
        {
            auxPrint(root);
            cout << endl;
        }

        void auxPrint(BNode<T> *node)
        {
            int i;
            for(i = 0; i < node->n; i++)
            {
                if(! node->folha)
                    auxPrint(node->filhos[i]);
                cout << node->chaves[i] << " ";
            }
            if(! node->folha)
                auxPrint(node->filhos[i]);
        }        
};

#endif /* BTREE_H__ */
