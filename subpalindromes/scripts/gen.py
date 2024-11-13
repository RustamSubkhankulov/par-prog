#!/usr/bin/python3

import sys
import string
from random import randrange, choice

# ==================


def parse_args(argv):
    """
    Parse sys.argv for arguments of prog.
    """
    if len(argv) != 4:
        print("Usage: ./gen.py N M filename")
        sys.exit(1)

    N = int(argv[1])
    M = int(argv[2])
    filename = argv[3]

    if N < 1:
        print("Domain error: N < 1")
        sys.exit(1)

    if not (1 <= M <= 26):
        print("Domain error: 1 > M > 26")
        sys.exit(1)

    return (N, M, filename)


# ------------------


def gen_rand_string(len: int, mod: int) -> str:
    return ''.join(choice(string.ascii_lowercase[:mod]) for _ in range(len))


# ==================

N, M, filename = parse_args(sys.argv)

with open(filename, mode='w') as out_file:
    out_file.write(f"{gen_rand_string(N, M)} \n")
