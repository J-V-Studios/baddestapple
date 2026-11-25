#!/usr/bin/env bash
# Parse vpxtest encode stdout (or a saved report) into a comparable size table.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
REPORT="${1:-}"

if [[ -z "$REPORT" ]]; then
  make -C "$ROOT/tools" size_report >/dev/null
  "$ROOT/tools/size_report"
  exit 0
fi

awk '
  /Combined:/ { combined=$NF }
  /\+Tile Class:/ { tileclass=$NF }
  /\+ Tile Prob:/ { tileprob=$NF }
  /\+  Run Prob:/ { runprob=$NF }
  /\+COMPGlyphs:/ { glyphs=$NF }
  /Sound    :/ { sound=$NF }
  /Total:/ {
    total=$2
    printf "Combined payload: %s\n", combined
    printf "Tile Class:       %s\n", tileclass
    printf "Tile Prob:        %s\n", tileprob
    printf "Run Prob:         %s\n", runprob
    printf "COMP Glyphs:      %s\n", glyphs
    printf "Sound:            %s\n", sound
    printf "Total:            %s\n", total
    printf "Upstream:         62781\n"
    printf "Delta:            %+d\n", total - 62781
  }
' "$REPORT"
