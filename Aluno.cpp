#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "sqlite3.h"

using namespace std;

struct Aluno {
    int id;
    string nome;
    vector<float> notas;
};

class SistemaAlunos {
private:
    sqlite3* db;
    
public:
    SistemaAlunos() : db(nullptr) {}
    
    ~SistemaAlunos() {
        if (db) {
            sqlite3_close(db);
        }
    }
    
    bool inicializar() {
        int rc = sqlite3_open("alunos.db", &db);
        if (rc != SQLITE_OK) {
            cout << "Erro ao abrir banco de dados: " << sqlite3_errmsg(db) << endl;
            return false;
        }
        
        // Criar tabelas se não existirem
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS alunos (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                nome TEXT NOT NULL
            );
            
            CREATE TABLE IF NOT EXISTS notas (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                aluno_id INTEGER,
                nota REAL NOT NULL,
                FOREIGN KEY (aluno_id) REFERENCES alunos (id)
            );
        )";
        
        char* errMsg = nullptr;
        rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        
        if (rc != SQLITE_OK) {
            cout << "Erro ao criar tabelas: " << errMsg << endl;
            sqlite3_free(errMsg);
            return false;
        }
        
        return true;
    }
    
    void carregarAlunos(vector<Aluno>& alunos) {
        alunos.clear();
        
        const char* sql = R"(
            SELECT a.id, a.nome, n.nota
            FROM alunos a
            LEFT JOIN notas n ON a.id = n.aluno_id
            ORDER BY a.id, n.id;
        )";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        
        if (rc != SQLITE_OK) {
            cout << "Erro ao carregar alunos: " << sqlite3_errmsg(db) << endl;
            return;
        }
        
        int currentId = -1;
        Aluno* currentAluno = nullptr;
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* nome = (const char*)sqlite3_column_text(stmt, 1);
            
            if (id != currentId) {
                // Novo aluno
                Aluno a;
                a.id = id;
                a.nome = nome;
                a.notas.clear();
                alunos.push_back(a);
                currentAluno = &alunos.back();
                currentId = id;
            }
            
            // Adicionar nota se existir
            if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) {
                float nota = sqlite3_column_double(stmt, 2);
                currentAluno->notas.push_back(nota);
            }
        }
        
        sqlite3_finalize(stmt);
    }
    
    void salvarAlunos(const vector<Aluno>& alunos) {
        // Limpar banco de dados
        const char* sqlClear = "DELETE FROM notas; DELETE FROM alunos;";
        char* errMsg = nullptr;
        sqlite3_exec(db, sqlClear, nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);
        
        // Inserir alunos e notas
        for (const Aluno& a : alunos) {
            // Inserir aluno
            const char* sqlAluno = "INSERT INTO alunos (nome) VALUES (?);";
            sqlite3_stmt* stmtAluno;
            
            int rc = sqlite3_prepare_v2(db, sqlAluno, -1, &stmtAluno, nullptr);
            if (rc == SQLITE_OK) {
                sqlite3_bind_text(stmtAluno, 1, a.nome.c_str(), -1, SQLITE_STATIC);
                sqlite3_step(stmtAluno);
                sqlite3_finalize(stmtAluno);
                
                int alunoId = sqlite3_last_insert_rowid(db);
                
                // Inserir notas
                for (float nota : a.notas) {
                    const char* sqlNota = "INSERT INTO notas (aluno_id, nota) VALUES (?, ?);";
                    sqlite3_stmt* stmtNota;
                    
                    rc = sqlite3_prepare_v2(db, sqlNota, -1, &stmtNota, nullptr);
                    if (rc == SQLITE_OK) {
                        sqlite3_bind_int(stmtNota, 1, alunoId);
                        sqlite3_bind_double(stmtNota, 2, nota);
                        sqlite3_step(stmtNota);
                        sqlite3_finalize(stmtNota);
                    }
                }
            }
        }
    }
};

float calcularMedia(const vector<float>& notas) {
    float soma = 0;
    for (float nota : notas)
        soma += nota;
    return notas.empty() ? 0 : soma / notas.size();
}

int main() {
    vector<Aluno> alunos;
    SistemaAlunos sistema;
    
    if (!sistema.inicializar()) {
        cout << "Erro ao inicializar o sistema!" << endl;
        return 1;
    }
    
    sistema.carregarAlunos(alunos);

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
            
            // Validação de entrada
            if (!(cin >> numNotas) || numNotas <= 0) {
                cout << "Número inválido! Digite um número positivo.\n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue; // Voltar ao menu
            }

            for (int i = 0; i < numNotas; ++i) {
                float nota;
                cout << "Nota " << i + 1 << ": ";
                
                // Validação de entrada
                if (!(cin >> nota) || nota < 0 || nota > 10) {
                    cout << "Nota inválida! Digite um número entre 0 e 10.\n";
                    cin.clear();
                    cin.ignore(10000, '\n');
                    i--; // Tentar novamente
                    continue;
                }
                a.notas.push_back(nota);
            }

            alunos.push_back(a);
            sistema.salvarAlunos(alunos);
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
            
            // Validação de entrada para evitar loop infinito
            if (!(cin >> indice)) {
                cout << "Entrada inválida! Digite um número.\n";
                cin.clear(); // Limpar flags de erro
                cin.ignore(10000, '\n'); // Limpar buffer
            } else if (indice < 1 || indice > alunos.size()) {
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
                    
                    // Validação de entrada
                    if (!(cin >> numNotas) || numNotas <= 0) {
                        cout << "Número inválido! Digite um número positivo.\n";
                        cin.clear();
                        cin.ignore(10000, '\n');
                        continue; // Voltar ao menu
                    }

                    for (int i = 0; i < numNotas; ++i) {
                        float nota;
                        cout << "Nota " << i + 1 << ": ";
                        
                        // Validação de entrada
                        if (!(cin >> nota) || nota < 0 || nota > 10) {
                            cout << "Nota inválida! Digite um número entre 0 e 10.\n";
                            cin.clear();
                            cin.ignore(10000, '\n');
                            i--; // Tentar novamente
                            continue;
                        }
                        a.notas.push_back(nota);
                    }
                    cin.ignore();
                }

                sistema.salvarAlunos(alunos);
                cout << "Aluno atualizado com sucesso!\n";
            }
        } else if (opcao == 4) {
            cout << "\n--- Remover Aluno ---\n";
            for (size_t i = 0; i < alunos.size(); ++i) {
                cout << i + 1 << ". " << alunos[i].nome << "\n";
            }

            int indice;
            cout << "Escolha o número do aluno para remover: ";
            
            // Validação de entrada para evitar loop infinito
            if (!(cin >> indice)) {
                cout << "Entrada inválida! Digite um número.\n";
                cin.clear(); // Limpar flags de erro
                cin.ignore(10000, '\n'); // Limpar buffer
            } else if (indice < 1 || indice > alunos.size()) {
                cout << "Índice inválido!\n";
            } else {
                alunos.erase(alunos.begin() + (indice - 1));
                sistema.salvarAlunos(alunos);
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
