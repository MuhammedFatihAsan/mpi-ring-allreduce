#!/usr/bin/env bash
# benchmark sweep: writes results/results.csv with one row per timed run.
# usage: ./scripts/test_runner.sh

set -euo pipefail

REPO=$(cd "$(dirname "$0")/.." && pwd)
BIN=$REPO/bin
RESULTS=$REPO/results
CSV=$RESULTS/results.csv
LOG=$RESULTS/runner.log

mkdir -p "$RESULTS"

# rebuild if binaries are missing
if [[ ! -x $BIN/baseline || ! -x $BIN/ring ]]; then
    (cd "$REPO" && make all)
fi

RANKS=(2 4 8 16)
SIZES=(65536 262144 1048576 4194304 16777216 26214400)
RUNS=5
WARMUP=1

echo "algo,np,count,run,comm_time_s,total_time_s" > "$CSV"
: > "$LOG"

parse_time() {
    # pulls the float after a label like "Communication Time: 0.001234 seconds"
    local key="$1" out="$2"
    echo "$out" | awk -v k="$key" -F': ' 'index($0,k)==1 { gsub(" seconds","",$2); print $2; exit }'
}

total_runs=$(( ${#RANKS[@]} * ${#SIZES[@]} * 2 * (RUNS + WARMUP) ))
done_runs=0

for np in "${RANKS[@]}"; do
    for n in "${SIZES[@]}"; do
        for algo in baseline ring; do
            for ((r=1; r<=RUNS+WARMUP; r++)); do
                done_runs=$((done_runs + 1))
                printf "[%d/%d] np=%-2d n=%-9d algo=%-8s run=%d ... " \
                    "$done_runs" "$total_runs" "$np" "$n" "$algo" "$r"

                set +e
                out=$(mpirun --oversubscribe -np "$np" "$BIN/$algo" "$n" 2>&1)
                rc=$?
                set -e

                if [[ $rc -ne 0 ]] || ! echo "$out" | grep -q "Verification: PASS"; then
                    echo "FAIL"
                    {
                        echo "=== FAIL np=$np n=$n algo=$algo run=$r ==="
                        echo "$out"
                    } >> "$LOG"
                    continue
                fi

                comm=$(parse_time "Communication Time" "$out")
                total=$(parse_time "Setup + Communication Time" "$out")
                # baseline has no setup line, so reuse comm
                [[ -z "$total" ]] && total="$comm"

                if (( r > WARMUP )); then
                    run_idx=$((r - WARMUP))
                    echo "$algo,$np,$n,$run_idx,$comm,$total" >> "$CSV"
                    echo "$comm s"
                else
                    echo "(warmup, dropped)"
                fi
            done
        done
    done
done

echo
echo "wrote $CSV"
echo "see $LOG for any failed runs"
