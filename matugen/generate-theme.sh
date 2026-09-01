#!/usr/bin/env bash
# Generate Matugen MD3 colors and update Dolphin in real-time
set -e

SOURCE_COLOR="${1:-#3b82f6}"
CACHE_DIR="${XDG_CACHE_HOME:-$HOME/.cache}/matugen"
mkdir -p "$CACHE_DIR"

echo "[M3-Dolphin] Generating Material Design 3 tokens for: $SOURCE_COLOR"
matugen color hex "$SOURCE_COLOR" --json hex > "$CACHE_DIR/colors.json"

echo "[M3-Dolphin] Emitting D-Bus reloadColors notification..."
qdbus org.kde.dolphin /M3ColorEngine reloadColors 2>/dev/null || true

echo "[M3-Dolphin] Done. Active widgets updated."
