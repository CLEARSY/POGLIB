 #!/bin/bash

testdir="$1"
id="$2"

echo "testdir: $testdir"
echo "id: $id"

set -x

cd "$testdir"

. ./setenv.sh

echo "program: $program"

inpdir="$testdir/input/$id"
refdir="$testdir/output/reference/$id"
outdir="$testdir/output/result/$id"
rm -rf "$outdir"
mkdir -p "$outdir"

$program "$inpdir/input.pog" > "$outdir/output.pog" 2> "$outdir/stderr"
echo $? > "$outdir/exitcode"

diff "$outdir/exitcode" "$refdir/exitcode"
if [ $? -ne 0 ]; then
    echo "Test failed: exitcode differs"
    exit 1
fi

diff "$outdir/stderr" "$refdir/stderr"
if [ $? -ne 0 ]; then
    echo "Test failed: stderr differs"
    exit 1
fi

diff "$outdir/output.pog" "$refdir/output.pog"
if [ $? -ne 0 ]; then
    echo "Test failed: output.pog differs"
    exit 1
fi

echo "Test passed"
exit 0
