#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

struct Aluno {
    string nome;
    vector<float> notas;
};

float calcularMedia(const vector<float>& notas) {
    float soma = 0;
    for (float nota : notas)
        soma += nota;
    return notas.empty() ? 0 : soma / notas.size();
}

void carregarAlunos(vector<Aluno>& alunos, const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) return;

    string linha;
    while (getline(arquivo, linha)) {
        stringstream ss(linha);
        string token;
        Aluno a;

        getline(ss, a.nome, ';');
        while (getline(ss, token, ';')) {
            a.notas.push_back(stof(token));
        }
        alunos.push_back(a);
    }

    arquivo.close();
}

void salvarAlunos(const vector<Aluno>& alunos, const string& nomeArquivo) {
    ofstream arquivo(nomeArquivo);
    for (const Aluno& a : alunos) {
        arquivo << a.nome;
        for (float nota : a.notas)
            arquivo << ";" << nota;
        arquivo << "\n";
    }
    arquivo.close();
}

int main() {
    vector<Aluno> alunos;
    const string nomeArquivo = "dados.txt";

    carregarAlunos(alunos, nomeArquivo);

    int opcao;
    do {
        cout << "\n--- Menu ---\n";
        cout << "1. Cadastrar novo aluno\n";
        cout << "2. Ver boletim\n";
        cout << "3. Atualizar aluno\n";
        cout << "4. Remover aluno\n";
        cout << "0. Sair\n";
        cout << "Escolha: ";
        cin >> opcao;

        cin.ignore(); // limpar buffer

        if (opcao == 1) {
            Aluno a;
            cout << "Nome do aluno: ";
            getline(cin, a.nome);

            int numNotas;
            cout << "Quantas notas? ";
            cin >> numNotas;

            for (int i = 0; i < numNotas; ++i) {
                float nota;
                cout << "Nota " << i + 1 << ": ";
                cin >> nota;
                a.notas.push_back(nota);
            }

            alunos.push_back(a);
            salvarAlunos(alunos, nomeArquivo);
            cout << "\nAluno cadastrado com sucesso!\n";
            cin.ignore();
        } else if (opcao == 2) {
            cout << "\n--- Boletim ---\n";
            for (const Aluno& a : alunos) {
                float media = calcularMedia(a.notas);
                cout << "Aluno: " << a.nome << " | Média: " << media;
                cout << (media >= 7.0 ? " | Aprovado\n" : " | Reprovado\n");
            }
        } else if (opcao == 3) {
            cout << "\n--- Atualizar Aluno ---\n";
            for (size_t i = 0; i < alunos.size(); ++i) {
                cout << i + 1 << ". " << alunos[i].nome << "\n";
            }

            int indice;
            cout << "Escolha o número do aluno para atualizar: ";
            cin >> indice;
            cin.ignore();

            if (indice < 1 || indice > alunos.size()) {
                cout << "Índice inválido!\n";
            } else {
                Aluno& a = alunos[indice - 1];

                cout << "Novo nome (ou ENTER para manter): ";
                string novoNome;
                getline(cin, novoNome);
                if (!novoNome.empty()) {
                    a.nome = novoNome;
                }

                cout << "Deseja atualizar as notas? (s/n): ";
                char resp;
                cin >> resp;
                cin.ignore();

                if (resp == 's' || resp == 'S') {
                    a.notas.clear();
                    int numNotas;
                    cout << "Quantas novas notas? ";
                    cin >> numNotas;

                    for (int i = 0; i < numNotas; ++i) {
                        float nota;
                        cout << "Nota " << i + 1 << ": ";
                        cin >> nota;
                        a.notas.push_back(nota);
                    }
                    cin.ignore();
                }

                salvarAlunos(alunos, nomeArquivo);
                cout << "Aluno atualizado com sucesso!\n";
            }
        } else if (opcao == 4) {
            cout << "\n--- Remover Aluno ---\n";
            for (size_t i = 0; i < alunos.size(); ++i) {
                cout << i + 1 << ". " << alunos[i].nome << "\n";
            }

            int indice;
            cout << "Escolha o número do aluno para remover: ";
            cin >> indice;

            if (indice < 1 || indice > alunos.size()) {
                cout << "Índice inválido!\n";
            } else {
                alunos.erase(alunos.begin() + (indice - 1));
                salvarAlunos(alunos, nomeArquivo);
                cout << "Aluno removido com sucesso!\n";
            }
            cin.ignore();
        } else if (opcao == 0) {
            cout << "Encerrando o programa...\n";
        } else {
            cout << "Opção inválida! Tente novamente.\n";
        }

    } while (opcao != 0);

#ifdef _WIN32
    system("pause");
#else
    cout << "Pressione ENTER para sair...";
    cin.get();
#endif

    return 0;
}
