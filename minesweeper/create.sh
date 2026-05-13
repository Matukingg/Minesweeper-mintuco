
# Ruta del proyecto (asume que este script está en la raíz de mi_buscaminas)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
EXECUTABLE="$BUILD_DIR/bin/debug/buscaminas"

echo "=== 0. Limpiando build anterior ==="
if [[ -d "$BUILD_DIR" ]]; then
  
  echo "✔ Build anterior eliminado."
else
  echo "✘ No se encontró un build anterior, continuando..."
fi

echo "=== 1. Generando Makefiles con Premake4 ==="
premake4 --file="$PROJECT_ROOT/premake4.lua" gmake

echo "=== 2. Compilando (Debug) ==="
make -k -C "$BUILD_DIR" config=debug

