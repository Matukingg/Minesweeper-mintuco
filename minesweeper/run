PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
EXECUTABLE="$BUILD_DIR/bin/debug/buscaminas"

echo "=== 69. Que corra manito ==="
if [[ -x "$EXECUTABLE" ]]; then
  echo "✔ Ejecutable activo puñeta, arrancando Buscaminas…"
  exec "$EXECUTABLE"
else
  echo "✘ No hay ejecutable. Revisa la wea bien."
  exit 1
fi