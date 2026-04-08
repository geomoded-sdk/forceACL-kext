#!/bin/bash

# Script de instalação e compilação do ForceACL.kext
# Compatível com macOS

set -e  # Para o script em caso de erro

echo "=== ForceACL.kext - Script de Instalação e Compilação ==="
echo "Este script instala dependências e compila o projeto."
echo ""

# Verificar se estamos no macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Erro: Este script é apenas para macOS."
    exit 1
fi

# Verificar e instalar Xcode Command Line Tools
echo "Verificando Xcode Command Line Tools..."
if ! xcode-select -p &> /dev/null; then
    echo "Instalando Xcode Command Line Tools..."
    xcode-select --install
    echo "Aguarde a instalação do Xcode Command Line Tools e pressione Enter para continuar..."
    read
else
    echo "Xcode Command Line Tools já instalado."
fi

# Verificar se o diretório do projeto existe
if [[ ! -f "Makefile" ]]; then
    echo "Erro: Execute este script no diretório raiz do projeto ForceACL.kext."
    exit 1
fi

# Instalar Lilu SDK se não existir
LILU_DIR="Lilu"
if [[ ! -d "$LILU_DIR" ]]; then
    echo "Clonando Lilu SDK..."
    git clone https://github.com/acidanthera/Lilu.git
    cd Lilu
    git submodule update --init --recursive
    cd ..
    echo "Lilu SDK clonado."
else
    echo "Lilu SDK já existe."
fi

# Configurar caminho do Lilu no Makefile (se necessário)
echo "Configurando Makefile..."
# O Makefile já tem LILU_PATH?= $(HOME)/Lilu, mas vamos garantir
if ! grep -q "LILU_PATH.*Lilu" Makefile; then
    echo "Adicionando LILU_PATH ao Makefile..."
    sed -i '' '1a\
LILU_PATH ?= ./Lilu
' Makefile
fi

# Compilar o projeto
echo "Compilando ForceACL.kext..."
make clean
make

# Verificar se compilou com sucesso
if [[ -d "build/ForceACL.kext" ]]; then
    echo ""
    echo "✅ Compilação concluída com sucesso!"
    echo "Kext gerado em: build/ForceACL.kext"
    echo ""
    echo "Para instalar:"
    echo "  sudo cp -r build/ForceACL.kext /Library/Extensions/"
    echo "  sudo kextcache -i /"
    echo ""
    echo "Para usar no OpenCore:"
    echo "  - Adicione ForceACL.kext em Kernel -> Add"
    echo "  - Defina Kernel -> Quirks -> DisableIoMapper = Yes"
else
    echo "❌ Erro na compilação. Verifique os logs acima."
    exit 1
fi

echo "=== Script concluído ==="