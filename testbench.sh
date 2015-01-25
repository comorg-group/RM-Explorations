# Test all trace for Baseline Cache simulator

cd trace
for filename in *; do ../simulator/Debug/simulator "${filename}" > "../results/${filename}" & done
