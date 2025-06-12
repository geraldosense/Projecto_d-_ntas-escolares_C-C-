# SQLite Integration

Esta pasta contém os arquivos do SQLite necessários para o projeto.

## Estrutura

```
sqlite/
├── src/
│   └── sqlite3.c      # Arquivo fonte principal do SQLite
├── lib/
│   └── sqlite3.h      # Arquivo de cabeçalho do SQLite
└── README.md          # Este arquivo
```

## Sistema de Alunos Integrado

O projeto agora usa SQLite em vez de arquivo de texto para armazenar dados dos alunos.

### Funcionalidades:
- ✅ Cadastrar alunos
- ✅ Adicionar notas
- ✅ Ver boletim com médias
- ✅ Atualizar dados dos alunos
- ✅ Remover alunos
- ✅ Persistência de dados no banco SQLite

## Como usar

### Método 1: Compilação Manual (Testado e Funcionando)

```bash
# 1. Compilar SQLite como C (importante!)
gcc -c sqlite/src/sqlite3.c -o sqlite3.o -DSQLITE_ENABLE_FTS5 -DSQLITE_ENABLE_JSON1

# 2. Compilar seu código C++
g++ -c Aluno.cpp -o Aluno.o -I./sqlite/lib -std=c++11

# 3. Linkar tudo
g++ sqlite3.o Aluno.o -o sistema_sqlite.exe
```

### Método 2: Usando o Script PowerShell

```powershell
# Execute o script de compilação
.\compile.ps1
```

### Método 3: Testar o Sistema

```powershell
# Execute o script de teste
.\testar_sistema.ps1
```

## ⚠️ Importante

- **SQLite deve ser compilado como C**, não como C++
- Use `gcc` para compilar `sqlite3.c`
- Use `g++` para compilar seus arquivos `.cpp`
- Inclua `-I./sqlite/lib` para encontrar o cabeçalho

## Arquivos do Sistema

- **`Aluno.cpp`** - Sistema principal com SQLite integrado
- **`sistema_sqlite.exe`** - Executável do sistema
- **`alunos.db`** - Banco de dados SQLite (criado automaticamente)

## Estrutura do Banco de Dados

```sql
-- Tabela de alunos
CREATE TABLE alunos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nome TEXT NOT NULL
);

-- Tabela de notas
CREATE TABLE notas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    aluno_id INTEGER,
    nota REAL NOT NULL,
    FOREIGN KEY (aluno_id) REFERENCES alunos (id)
);
```

## Exemplo de uso no código

```cpp
#include "sqlite3.h"

// Seu código aqui...
```

## Versão

SQLite 3.50.1 (amalgamation)

## Status

✅ **Testado e funcionando** - O sistema de alunos está integrado com SQLite!
✅ **Interface mantida** - Mesma interface do sistema original
✅ **Persistência** - Dados salvos no banco SQLite 