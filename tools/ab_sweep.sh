#!/usr/bin/env bash
# Sweep Phase-A encoder knobs; keep best Total under upstream 62781.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/tools/reports"
mkdir -p "$OUT"
STREAM="$ROOT/streamrecomp/stream_stripped.dat"
TILES="$ROOT/comp2/tiles-64x48x8.dat"
BEST_TOTAL=999999
BEST_TAG=""
SUMMARY="$OUT/sweep_summary.txt"
: > "$SUMMARY"

run_one() {
  local tag="$1"; shift
  local defs=("$@")
  local cflags="-O2 -g -DRESX=64 -DRESY=48 -DBLOCKSIZE=8 -DFRAMECT=6570 -DTARGET_GLYPH_COUNT=256 -DHEADLESS -Wno-unused-result -I$ROOT/common"
  for d in "${defs[@]}"; do
    cflags+=" -D$d"
  done
  echo "=== $tag ===" | tee -a "$SUMMARY"
  gcc -o "$ROOT/vpxtest/vpxtest_ab" "$ROOT/vpxtest/vpxtest.c" -lm $cflags
  local report="$OUT/encode_${tag}.txt"
  ( cd "$ROOT/vpxtest" && ./vpxtest_ab "$STREAM" "$TILES" /dev/null ) > "$report" 2>&1
  local total
  total=$(awk '/^ Total:/{print $2; exit}' "$report")
  local combined tileprob runprob glyphs
  combined=$(awk '/Combined:/{print $NF; exit}' "$report")
  tileprob=$(awk '/\+ Tile Prob:/{print $NF; exit}' "$report")
  runprob=$(awk '/\+  Run Prob:/{print $NF; exit}' "$report")
  glyphs=$(awk '/\+COMPGlyphs:/{print $NF; exit}' "$report")
  echo "  Total=$total Combined=$combined TileProb=$tileprob RunProb=$runprob Glyphs=$glyphs" | tee -a "$SUMMARY"
  if [[ -n "$total" && "$total" -lt "$BEST_TOTAL" ]]; then
    BEST_TOTAL=$total
    BEST_TAG=$tag
    cp "$ROOT/vpxtest/badapple_data.h" "$OUT/best_badapple_data.h"
    echo "  ** new best **" | tee -a "$SUMMARY"
  fi
}

# Defaults reference
run_one default "USE_TILE_CLASSES=13" "RUNCODES_CONTINUOUS=128" "MAXPIXELRUNTOSTORE=9"

# Tile class sweep (mutually exclusive overrides via -D)
for c in 8 9 10 11 12 13 14 15 16; do
  run_one "classes_${c}" "USE_TILE_CLASSES=${c}" "RUNCODES_CONTINUOUS=128" "MAXPIXELRUNTOSTORE=9"
done

# Run-code continuous length
for r in 64 96 112 128 144 160 192; do
  run_one "runcodes_${r}" "USE_TILE_CLASSES=13" "RUNCODES_CONTINUOUS=${r}" "MAXPIXELRUNTOSTORE=9"
done

# Glyph pixel run store
for m in 6 7 8 9 10 12 14 16; do
  run_one "maxpixrun_${m}" "USE_TILE_CLASSES=13" "RUNCODES_CONTINUOUS=128" "MAXPIXELRUNTOSTORE=${m}"
done

echo "BEST_TAG=$BEST_TAG BEST_TOTAL=$BEST_TOTAL" | tee -a "$SUMMARY"
