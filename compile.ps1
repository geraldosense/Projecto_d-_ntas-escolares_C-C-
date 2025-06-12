# Script de compilação para Windows PowerShell

Write-Host "Compilando projeto C++ com SQLite..." -ForegroundColor Green

# Criar diretório build se não existir
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" -Force
    Write-Host "Diretório build criado." -ForegroundColor Yellow
}

# Compilar SQLite (como C)
Write-Host "Compilando SQLite..." -ForegroundColor Cyan
gcc -c sqlite/src/sqlite3.c -o build/sqlite3.o -DSQLITE_ENABLE_FTS5 -DSQLITE_ENABLE_JSON1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Erro ao compilar SQLite!" -ForegroundColor Red
    exit 1
}

# Compilar Aluno.cpp
Write-Host "Compilando Aluno.cpp..." -ForegroundColor Cyan
g++ -c Aluno.cpp -o build/Aluno.o -I./sqlite/lib -std=c++11
if ($LASTEXITCODE -ne 0) {
    Write-Host "Erro ao compilar Aluno.cpp!" -ForegroundColor Red
    exit 1
}

# Compilar main.cpp (se existir)
if (Test-Path "main.cpp") {
    Write-Host "Compilando main.cpp..." -ForegroundColor Cyan
    g++ -c main.cpp -o build/main.o -I./sqlite/lib -std=c++11
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Erro ao compilar main.cpp!" -ForegroundColor Red
        exit 1
    }
    $MAIN_OBJ = "build/main.o"
} else {
    $MAIN_OBJ = ""
}

# Linkar tudo
Write-Host "Linkando executável..." -ForegroundColor Cyan
if ($MAIN_OBJ) {
    g++ build/sqlite3.o build/Aluno.o $MAIN_OBJ -o sistema.exe
} else {
    g++ build/sqlite3.o build/Aluno.o -o sistema.exe
}

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilação concluída com sucesso!" -ForegroundColor Green
    Write-Host "Executável: sistema.exe" -ForegroundColor Yellow
} else {
    Write-Host "Erro na linkagem!" -ForegroundColor Red
    exit 1
} 