# RCPKC CLI Prototype

This folder contains a command-line version of the RCPKC implementation. It is separate from the original Visual Studio benchmarking code in `RCPKC/`.

The CLI version supports:

- key generation
- encryption
- decryption
- CSV benchmarking
- file-based public/private keys

## Build

Install NTL and GMP, then run:

```bash
make
```

Equivalent manual command:

```bash
g++ -O2 -std=c++17 rcpkc_cli.cpp -o rcpkc_cli -lntl -lgmp -lm
```

## Generate Keys

```bash
./rcpkc_cli keygen --q-bits 473 --message-bits 225 --public public.key --private private.key
```

This writes:

- `public.key`
- `private.key`

## Encrypt

```bash
./rcpkc_cli encrypt --public public.key --message 123456789 --cipher cipher.txt
```

The ciphertext is written to `cipher.txt`.

To provide a fixed encryption randomizer for reproducible experiments:

```bash
./rcpkc_cli encrypt --public public.key --message 123456789 --cipher cipher.txt --r 12345
```

## Decrypt

```bash
./rcpkc_cli decrypt --private private.key --cipher cipher.txt
```

The recovered plaintext is printed to standard output.

## Benchmark

```bash
./rcpkc_cli bench --public public.key --private private.key --messages ../RCPKC/Message_samples3.csv --out ResultsFile_cli.csv
```

The output CSV contains:

```text
message,ciphertext,decrypted,encryption_ns,decryption_ns,correct
```

## Notes

This is a research prototype. It keeps the core arithmetic of the original RCPKC implementation but adds a deployable command-line workflow. It is not intended for production use without independent cryptographic review, parameter validation, secure randomness, serialization hardening, and side-channel analysis.
