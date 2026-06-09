#!/usr/bin/env bash
set -euo pipefail

# Perf benchmarking script for Forsp
# Usage: ./perf.sh [baseline|compare|clean]
#
# baseline  - Run benchmarks and save results to perf-results/baseline/
# compare   - Run benchmarks and save results to perf-results/current/, then diff
# clean     - Remove perf-results/

RESULTS_DIR="perf-results"
BENCH_EXAMPLE="examples/factorial.fp"
BINARY="./forsp"

perf_run() {
    local label="$1"
    local outdir="$RESULTS_DIR/$label"
    mkdir -p "$outdir"

    echo "=== Running benchmarks: $label ==="
    echo "  $BENCH_EXAMPLE"

    # perf stat
    perf stat \
        -e cycles,instructions,cache-references,cache-misses,branch-misses,task-clock \
        "$BINARY" "$BENCH_EXAMPLE" \
        > "$outdir/perf.txt" 2>&1

    # valgrind callgrind
    valgrind --tool=callgrind --callgrind-out-file="$outdir/callgrind.out" \
        "$BINARY" "$BENCH_EXAMPLE" \
        > /dev/null 2>&1

    # valgrind massif
    valgrind --tool=massif --massif-out-file="$outdir/massif.out" \
        --pages-as-heap=yes \
        "$BINARY" "$BENCH_EXAMPLE" \
        > /dev/null 2>&1

    echo "  Results saved to $outdir/"
}

compare_results() {
    local baseline="$RESULTS_DIR/baseline"
    local current="$RESULTS_DIR/current"

    if [ ! -d "$baseline" ]; then
        echo "ERROR: No baseline found. Run './perf.sh baseline' first."
        exit 1
    fi

    echo "=== Comparing baseline vs current ==="
    echo ""

    local bfile="$baseline/perf.txt"
    local cfile="$current/perf.txt"

    extract() {
        grep "$1" "$2" | awk '{print $1}' | tr -d ','
    }

    b_cycles=$(extract "cycles" "$bfile")
    c_cycles=$(extract "cycles" "$cfile")
    b_instr=$(extract "instructions" "$bfile")
    c_instr=$(extract "instructions" "$cfile")
    b_cache=$(extract "cache-misses" "$bfile")
    c_cache=$(extract "cache-misses" "$cfile")
    b_time=$(extract "seconds time elapsed" "$bfile")
    c_time=$(extract "seconds time elapsed" "$cfile")

    if [ -n "$b_cycles" ] && [ -n "$c_cycles" ]; then
        echo "  cycles:       $b_cycles -> $c_cycles"
    fi
    if [ -n "$b_instr" ] && [ -n "$c_instr" ]; then
        echo "  instructions: $b_instr -> $c_instr"
    fi
    if [ -n "$b_cache" ] && [ -n "$c_cache" ]; then
        echo "  cache-misses: $b_cache -> $c_cache"
    fi
    if [ -n "$b_time" ] && [ -n "$c_time" ]; then
        echo "  time (s):     $b_time -> $c_time"
    fi
    echo ""

    echo "=== Callgrind comparison (top functions) ==="
    local bcall="$baseline/callgrind.out"
    local ccall="$current/callgrind.out"

    if [ -f "$bcall" ] && [ -f "$ccall" ]; then
        echo "  Baseline top 5:"
        callgrind_annotate "$bcall" 2>/dev/null | head -20 | tail -10
        echo "  Current top 5:"
        callgrind_annotate "$ccall" 2>/dev/null | head -20 | tail -10
    fi
}

cmd="${1:-help}"

case "$cmd" in
    baseline)
        make -s "$BINARY"
        perf_run baseline
        ;;
    compare)
        make -s "$BINARY"
        perf_run current
        compare_results
        ;;
    clean)
        rm -rf "$RESULTS_DIR"
        echo "Cleaned $RESULTS_DIR/"
        ;;
    *)
        echo "Usage: $0 {baseline|compare|clean}"
        echo ""
        echo "  baseline  - Build and run benchmarks, save to perf-results/baseline/"
        echo "  compare   - Build, run benchmarks, save to perf-results/current/, then diff"
        echo "  clean     - Remove perf-results/"
        ;;
esac
