# baddestapple

Fork of [cnlohr/badderapple](https://github.com/cnlohr/badderapple) targeting a smaller packed payload at the same 64×48 / 6570-frame / audio fidelity.

## Result

| Metric | Upstream | This fork | Delta |
|--------|----------|-----------|-------|
| Packed assets (vpxtest Total) | 62,781 B | **61,589 B** | **−1,192 B** |
| FLASH (`.fixedflash` + report) | 62,976 B (99.19%) | **61,784 B (97.32%)** | **−1,192 B** |
| BOOTLOADER | 3,324 B | 3,316 B | −8 B |
| RAM | 8,064 B | 8,128 B | +64 B |

Quality: same glyph dictionary and tile stream (re-encoded from upstream bitstream). No resolution/FPS/audio cuts.

### Winning knobs (lossless re-encode)

In `common/bacommon.h`:

- `USE_TILE_CLASSES` **11** (was 13)
- `RUNCODES_CONTINUOUS` **40** (was 128)
- `MAXPIXELRUNTOSTORE` **8** (was 9)

Smaller probability tables outweigh a slightly larger combined stream.

## Reproduce size report

```bash
# Needs stream + tiles (extract from committed payload once):
make -C tools extract_stream_from_payload
./tools/extract_stream_from_payload comp2/tiles-64x48x8.dat streamrecomp/stream_stripped.dat

make -C vpxtest encode_headless
./tools/size_report.sh vpxtest/encode_report.txt
# or from header:
make -C tools size_report && ./tools/size_report
```

A/B sweeps live under `tools/ab_sweep.sh` and `tools/reports/`.

## Notes

- Song depth sweeps did not beat the committed Huffman reverse-LZSS (~856 B in firmware; size report still counts 673 B `vpxlzss.dat` to match upstream accounting).
- All 256 glyphs are used; Phase B glyph-count reduction was not required after Phase A wins.
- Firmware: profiling GPIO toggles on PD6 removed for a tiny code win; asset shrink is the FLASH headroom.
