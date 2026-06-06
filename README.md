# RCPKC-Project

Implementation and performance evaluation of the **Random Congruential Public-Key Cryptosystem (RCPKC)** proposed in:

> A. Ibrahim, A. Chefranov, N. Hamad, Y.-A. Daraghmi, A. Al-Khasawneh, J. J. P. C. Rodrigues,
> **"NTRU-Like Random Congruential Public-Key Cryptosystem for Wireless Sensor Networks"**,
> Sensors, 2020.

## Overview

This repository contains a C++ implementation of the RCPKC cryptosystem and a benchmarking framework used to evaluate the encryption and decryption performance of the scheme.

The implementation uses the **NTL (Number Theory Library)** for large integer arithmetic and measures cryptographic operation times using high-resolution performance counters.

The current implementation focuses on:

* RCPKC encryption
* RCPKC decryption
* Large-integer arithmetic using NTL
* Performance benchmarking
* Experimental evaluation over large plaintext datasets

---

## Repository Structure

```text
RCPKC-Project/
│
├── RCPKC/
│   ├── RCPKC.cpp
│   ├── Message_samples3.csv
│   ├── Message_samples4.csv
│   ├── Message_samples5.csv
│   └── ResultsFile.csv
│
├── GetPID/
├── GetTime/
├── MakeDesc/
│
├── doc/
│   └── NTL documentation
│
└── README.txt
```

### Main Components

#### RCPKC.cpp

Main implementation of the cryptosystem.

The program:

1. Loads plaintext samples from a CSV file.
2. Encrypts each plaintext using RCPKC.
3. Decrypts the resulting ciphertext.
4. Measures encryption time.
5. Measures decryption time.
6. Stores results in `ResultsFile.csv`.

---

## Cryptosystem Parameters

The implementation uses the parameter set:

| Parameter              | Value             |
| ---------------------- | ----------------- |
| Message Length (mgLen) | 225 bits          |
| Modulus Length (qLen)  | 473 bits          |
| q                      | 2^473             |
| g                      | 2^225 − 1         |
| f                      | 2^(473−225−1) − 1 |

The public and private key values are initialized directly in the source code for benchmarking purposes.

---

## Encryption

For a plaintext message `m`, encryption is implemented as:

```text
e = (h·r + m) mod q
```

where:

* `h` is the public key
* `r` is a random value
* `q` is the modulus

---

## Decryption

Decryption is performed in two stages:

### Step 1

```text
a = (f·e) mod q
```

### Step 2

```text
m = (Fg·a) mod g
```

where:

* `f` is a private parameter
* `Fg` is the inverse-related decryption parameter
* `g` is the message modulus

---

## Input Data

The implementation supports three benchmark datasets:

| File                 | Number of Messages |
| -------------------- | ------------------ |
| Message_samples3.csv | 10³ messages       |
| Message_samples4.csv | 10⁴ messages       |
| Message_samples5.csv | 10⁵ messages       |

Select the desired dataset by modifying:

```cpp
ifstream in("Message_samples3.csv");
```

inside `RCPKC.cpp`.

---

## Output

The benchmark results are written to:

```text
ResultsFile.csv
```

Each row contains:

```text
Encryption_Time,Decryption_Time
```

measured in nanoseconds using the Windows high-resolution performance counter.

---

## Dependencies

### Required Library

* NTL (Number Theory Library)

Official website:

https://libntl.org

### Compiler

The project was originally developed using:

* Microsoft Visual Studio
* Visual C++

The repository contains:

```text
RCPKC.vcxproj
```

for direct Visual Studio compilation.

---

## Building

### Visual Studio

Open:

```text
RCPKC.vcxproj
```

and build the project.

Ensure that:

* NTL is installed
* NTL include directories are configured
* NTL libraries are linked correctly

### Required Headers

```cpp
#include <NTL/ZZ.h>
#include <Windows.h>
```

---

## Performance Evaluation

The implementation measures:

* Encryption latency
* Decryption latency

for large collections of plaintext messages.

The benchmark framework can be used to:

* Reproduce results from the original paper
* Compare RCPKC against NTRU variants
* Evaluate parameter scalability
* Study computational complexity

---

## Research Applications

This implementation may be useful for research in:

* Post-Quantum Cryptography
* NTRU-like Cryptosystems
* Public-Key Encryption
* Secure Wireless Sensor Networks
* Privacy-Preserving Computation
* Cryptographic Benchmarking
* Lattice-Based Cryptography

---

## Citation

If you use this implementation in academic work, please cite:

```bibtex
@article{ibrahim2020rcpkc,
  title={NTRU-Like Random Congruential Public-Key Cryptosystem for Wireless Sensor Networks},
  author={Ibrahim, Anas and Chefranov, Alexander and Hamad, Nagham and Daraghmi, Yousef-Awwad and Al-Khasawneh, Ahmad and Rodrigues, Joel J.P.C.},
  journal={Sensors},
  volume={20},
  number={16},
  pages={4632},
  year={2020},
  publisher={MDPI}
}
```

---
## Author

**Dr. Anas Melhem (formerly publishing as Anas Ibrahim)**

Research Interests:

* Fully Homomorphic Encryption (FHE)
* Post-Quantum Cryptography (PQC)
* Lattice-Based Cryptography
* Privacy-Enhancing Technologies
* Privacy-Preserving Machine Learning
* Cryptanalysis

