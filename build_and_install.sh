#!/bin/bash

# ForceACL.kext - Script de Build e Instalação
# Compatível com macOS

set -e

echo "========================================"
echo " ForceACL.kext - Build System"
echo "========================================"
echo ""

# Verificar se estamos no macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Erro: Este script é apenas para macOS."
    exit 1
fi

# Verificar Xcode Command Line Tools
echo "Verificando Xcode Command Line Tools..."
if ! xcode-select -p &> /dev/null; then
    echo "Instalando Xcode Command Line Tools..."
    xcode-select --install
    echo "Aguarde a instalação e pressione Enter..."
    read
fi
echo "Xcode OK: $(xcode-select -p)"
echo ""

# Verificar Makefile
if [[ ! -f "Makefile" ]]; then
    echo "Erro: Makefile não encontrado."
    exit 1
fi

# Verificar/Clonar Lilu
LILU_DIR="Lilu"
if [[ ! -d "$LILU_DIR" ]]; then
    echo "Clonando Lilu SDK..."
    git clone https://github.com/acidanthera/Lilu.git
    cd Lilu
    git submodule update --init --recursive
    cd ..
    echo "Lilu SDK clonado."
else
    echo "Lilu SDK OK."
fi

echo ""
echo "========================================"
echo " Compilando todas as versões..."
echo "========================================"
echo ""

# ========== Debug Build ==========
echo ">>> Compilando Debug (Universal)..."
make clean
make BUILD_TYPE=debug
if [[ -d "build/Debug/ForceACL.kext" ]]; then
    echo "✅ Debug OK"
else
    echo "❌ Debug failed"
    exit 1
fi

# ========== Release Build ==========
echo ""
echo ">>> Compilando Release (Universal)..."
make clean
make BUILD_TYPE=release
if [[ -d "build/Release/ForceACL.kext" ]]; then
    echo "✅ Release OK"
else
    echo "❌ Release failed"
    exit 1
fi

# ========== x86_64 Only ==========
echo ""
echo ">>> Compilando x86_64 only..."
make clean
make ARCHS=x86_64
if [[ -d "build/Release/ForceACL.kext" ]]; then
    echo "✅ x86_64 OK"
else
    echo "❌ x86_64 failed"
    exit 1
fi

# ========== arm64 Only ==========
echo ""
echo ">>> Compilando arm64 only..."
make clean
make ARCHS=arm64
if [[ -d "build/Release/ForceACL.kext" ]]; then
    echo "✅ arm64 OK"
else
    echo "❌ arm64 failed"
    exit 1
fi

# ========== Summary ==========
echo ""
echo "========================================"
echo " Build Summary"
echo "========================================"
echo ""
echo "Arquivos gerados:"
echo "  - build/Debug/ForceACL.kext      (Debug - Universal)"
echo "  - build/Release/ForceACL.kext    (Release - Universal)"
echo ""

# Install option
echo "Deseja instalar o kext? (y/n)"
read -r answer
if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
    echo ""
    echo "Instalando Release version..."
    sudo cp -R build/Release/ForceACL.kext /Library/Extensions/
    sudo touch /Library/Extensions
    echo "✅ Kext instalado!"
    echo "Reinicie o sistema para aplicar."
fi

echo ""
echo "========================================"
echo " Script concluído!"
echo "========================================"