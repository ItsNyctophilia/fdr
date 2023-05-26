#!/bin/bash

GREEN='\033[32m'
RED='\033[31m'
NC='\033[0m'

# Test 1: Program prints error message in response to
# invocation error

FILENAME=./fdr

# Define the sample files to use as input
FILES="non-existant-argument"

# Define the options to pass to the program
OPTIONS=""

# Define the expected output
# Test may fail if user executes 'make check' from non-project-root
# directory.
EXPECTED_OUTPUT="Usage: $FILENAME"

# Run the program with the sample files and options
$FILENAME $OPTIONS ${FILES[@]} 2> output.txt

# Expected: Program exits with code 1 for INVOCATION_ERROR
# and outputs usage statement
if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "1. Invocation Error Test             : ${GREEN}PASS${NC}"
else
    echo -e "1. Invocation Error Test             : ${RED}FAIL${NC}"
fi

################## SERVER SETUP ##################
# Background fdr process
$FILENAME 2>/dev/null &
# Give server a moment to set up
sleep 1
##################################################

# Test 2: program connects and evaluates 0th Fibonacci number
FILES=""
OPTIONS=""
EXPECTED_OUTPUT="0x0"

# Expected: Program returns Fibonacci of 0
echo -n "F0" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "2. Basic Connection Test             : ${GREEN}PASS${NC}"
else
    echo -e "2. Basic Connection Test             : ${RED}FAIL${NC}"
fi

# Test 3: program connects and evaluates 150th Fibonacci number
FILES=""
OPTIONS=""
# 9969216677189303386214405760200 in decimal
EXPECTED_OUTPUT="0x7dd446c1f95e43f356255be4c8"

# Expected: Program returns Fibonacci of 150
echo -n "F150" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "3. Large Fib Test (F150)             : ${GREEN}PASS${NC}"
else
    echo -e "3. Large Fib Test (F150)             : ${RED}FAIL${NC}"
fi

# Test 4: program connects and evaluates 300th Fibonacci number
FILES=""
OPTIONS=""

# https://oeis.org/A000045/b000045.txt @300 in decimal
EXPECTED_OUTPUT="0x8a4ba39e1a1741497bbbef460a25486ee575f510e921b33e2e10"

# Expected: Program returns Fibonacci of 300
echo -n "F300" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "4. Max Fib Test (F300)               : ${GREEN}PASS${NC}"
else
    echo -e "4. Max Fib Test (F300)               : ${RED}FAIL${NC}"
fi

# Test 5: program connects and evaluates additive roman numeral
FILES=""
OPTIONS=""

EXPECTED_OUTPUT="0x16"

# Expected: Program returns 0x16 (decimal 22)
echo -n "RXXII" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "5. Additive Roman Test (RXII)        : ${GREEN}PASS${NC}"
else
    echo -e "5. Additive Roman Test (RXII)        : ${RED}FAIL${NC}"
fi

# Test 6: program connects and evaluates subtractive roman numeral
FILES=""
OPTIONS=""

EXPECTED_OUTPUT="0x12"

# Expected: Server returns 0x12 (decimal 22)
echo -n "RXIIX" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "6. Subtractive Roman Test (RXIIX)    : ${GREEN}PASS${NC}"
else
    echo -e "6. Subtractive Roman Test (RXIIX)    : ${RED}FAIL${NC}"
fi

# Test 7: program connects and evaluates complex roman numeral
FILES=""
OPTIONS=""

EXPECTED_OUTPUT="0xf9f"

# Expected: Server returns 0x12 (decimal 22)
echo -n "RMMMCMXCIX" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "7. Complex Roman Test (RMMMCMXCIX)   : ${GREEN}PASS${NC}"
else
    echo -e "7. Complex Roman Test (RMMMCMXCIX)   : ${RED}FAIL${NC}"
fi

# Test 8: program connects and performs a basic decimal -> hex conversion
FILES=""
OPTIONS=""

EXPECTED_OUTPUT="0xa"

# Expected: Server returns 0xA
echo -n "D10" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "8. Basic Decimal Test (D10)          : ${GREEN}PASS${NC}"
else
    echo -e "8. Basic Decimal Test (D10)          : ${RED}FAIL${NC}"
fi

# Test 9: program connects and performs a 10^19 decimal -> hex conversion
FILES=""
OPTIONS=""

EXPECTED_OUTPUT="0x8ac7230489e80000"

# Expected: Server returns 0x8AC7230489E80000
echo -n "D10000000000000000000" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "9. Max Decimal Test (D10^19)         : ${GREEN}PASS${NC}"
else
    echo -e "9. Max Decimal Test (D10^19)         : ${RED}FAIL${NC}"
fi

# Test 10: program connects and performs a 10^20 decimal -> hex conversion
FILES=""
OPTIONS="-d"

# Bring server down and set back up with new option -d
pkill fdr
$FILENAME $OPTIONS 2>/dev/null &
sleep 1

EXPECTED_OUTPUT="0x56bc75e2d63100000"

# Expected: Server returns 0x12 (decimal 22)
echo -n "D100000000000000000000" | nc -u -w1 localhost $UID > output.txt

if grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "10. Max Decimal Test (D10^20)        : ${GREEN}PASS${NC}"
else
    echo -e "10. Max Decimal Test (D10^20)        : ${RED}FAIL${NC}"
fi

# Test 11: program connects and does not respond to query outside of
# legal bounds for Fib (Max 300, Min 0)

FILES=""
OPTIONS=""

EXPECTED_OUTPUT=""

# Expected: Server returns nothing
echo -n "F301" | nc -u -w1 localhost $UID > output.txt

if [ ! -s output.txt ]; then
    echo -e "11a. Fibonacci Clamp Test (F301)     : ${GREEN}PASS${NC}"
else
    echo -e "11a. Fibonacci Clamp Test (F301)     : ${RED}FAIL${NC}"
fi

echo -n "F-1" | nc -u -w1 localhost $UID > output.txt

if [ ! -s output.txt ]; then
    echo -e "11b. Fibonacci Clamp Test (F-1)      : ${GREEN}PASS${NC}"
else
    echo -e "11b. Fibonacci Clamp Test (F-1)      : ${RED}FAIL${NC}"
fi

# Test 12: program connects and does not respond to query outside of
# legal bounds for Dec (Max 10^19, Min 0, Max w/ option 10^20)

FILES=""
OPTIONS=""

# Bring server down and set back up without options
pkill fdr
$FILENAME $OPTIONS 2>/dev/null &
sleep 1

EXPECTED_OUTPUT=""

echo -n "D-1" | nc -u -w1 localhost $UID > output.txt

if [ ! -s output.txt ]; then
    echo -e "12a. Decimal Clamp Test (D-1)        : ${GREEN}PASS${NC}"
else
    echo -e "12a. Decimal Clamp Test (D-1)        : ${RED}FAIL${NC}"
fi

# Expected: Server returns nothing
echo -n "D10000000000000000001" | nc -u -w1 localhost $UID > output.txt

if [ ! -s output.txt ]; then
    echo -e "12b. Decimal Clamp Test (D10^19 + 1) : ${GREEN}PASS${NC}"
else
    echo -e "12b. Decimal Clamp Test (D10^19 + 1) : ${RED}FAIL${NC}"
fi

# Bring server down and set back up with option -d
pkill fdr
OPTIONS="-d"
$FILENAME $OPTIONS 2>/dev/null &
sleep 1

echo -n "D10000000000000000001" | nc -u -w1 localhost $UID > output.txt

# This test passes if it receives a response from the server,
# unlike the others, as it's testing the -d option's bounds
if [ -s output.txt ]; then
    echo -e "12c. Decimal Clamp Test (w/ option)  : ${GREEN}PASS${NC}"
else
    echo -e "12c. Decimal Clamp Test (w/ option)  : ${RED}FAIL${NC}"
fi

echo -n "D100000000000000000001" | nc -u -w1 localhost $UID > output.txt

if [ ! -s output.txt ]; then
    echo -e "12d. Decimal Clamp Test (D10^20 + 1) : ${GREEN}PASS${NC}"
else
    echo -e "12d. Decimal Clamp Test (D10^20 + 1) : ${RED}FAIL${NC}"
fi

# Test 13: program connects and does not respond to query outside of
# legal bounds for Roman (Max 4000, Min 1)

FILES=""
OPTIONS=""

EXPECTED_OUTPUT=""

# Expected: Server returns nothing
echo -n "RMMMMI" | nc -u -w1 localhost $UID > output.txt

if [ ! -s output.txt ]; then
    echo -e "13. Roman Clamp Test (RMMMMI)        : ${GREEN}PASS${NC}"
else
    echo -e "13. Roman Clamp Test (RMMMMI)        : ${RED}FAIL${NC}"
fi

# There is no lower bound test because there is no character for zero,
# so any test attempting to go lower than I would not receive a response
# for using an invalid character rather than because it was too low.

#################### CLEANUP ####################
# Delete temp file
rm output.txt

# Close backgrounded server process
pkill fdr
#################################################
