#include <NTL/ZZ.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <stdexcept>

using namespace std;
using namespace NTL;

struct PublicKey {
    long q_bits = 0;
    long message_bits = 0;
    ZZ q;
    ZZ g;
    ZZ h;
};

struct PrivateKey {
    long q_bits = 0;
    long message_bits = 0;
    ZZ q;
    ZZ g;
    ZZ f;
    ZZ Fg;
};

static void usage() {
    cerr << "RCPKC CLI - research prototype\n\n"
         << "Commands:\n"
         << "  keygen  --q-bits <bits> --message-bits <bits> --public <file> --private <file>\n"
         << "  encrypt --public <file> --message <integer> --cipher <file> [--r <integer>]\n"
         << "  decrypt --private <file> --cipher <file>\n"
         << "  bench   --public <file> --private <file> --messages <csv> --out <csv>\n\n"
         << "Examples:\n"
         << "  ./rcpkc_cli keygen --q-bits 473 --message-bits 225 --public public.key --private private.key\n"
         << "  ./rcpkc_cli encrypt --public public.key --message 12345 --cipher cipher.txt\n"
         << "  ./rcpkc_cli decrypt --private private.key --cipher cipher.txt\n";
}

static string get_arg(int argc, char** argv, const string& name, const string& def = "") {
    for (int i = 0; i < argc - 1; ++i) {
        if (argv[i] == name) return argv[i + 1];
    }
    return def;
}

static bool has_arg(int argc, char** argv, const string& name) {
    for (int i = 0; i < argc; ++i) if (argv[i] == name) return true;
    return false;
}

static ZZ read_zz_file(const string& path) {
    ifstream in(path);
    if (!in) throw runtime_error("Cannot open file: " + path);
    ZZ x;
    in >> x;
    if (!in) throw runtime_error("Cannot read integer from file: " + path);
    return x;
}

static void write_zz_file(const string& path, const ZZ& x) {
    ofstream out(path);
    if (!out) throw runtime_error("Cannot write file: " + path);
    out << x << "\n";
}

static void write_public_key(const string& path, const PublicKey& pk) {
    ofstream out(path);
    if (!out) throw runtime_error("Cannot write public key: " + path);
    out << "RCPKC_PUBLIC_KEY_V1\n";
    out << "q_bits " << pk.q_bits << "\n";
    out << "message_bits " << pk.message_bits << "\n";
    out << "q " << pk.q << "\n";
    out << "g " << pk.g << "\n";
    out << "h " << pk.h << "\n";
}

static void write_private_key(const string& path, const PrivateKey& sk) {
    ofstream out(path);
    if (!out) throw runtime_error("Cannot write private key: " + path);
    out << "RCPKC_PRIVATE_KEY_V1\n";
    out << "q_bits " << sk.q_bits << "\n";
    out << "message_bits " << sk.message_bits << "\n";
    out << "q " << sk.q << "\n";
    out << "g " << sk.g << "\n";
    out << "f " << sk.f << "\n";
    out << "Fg " << sk.Fg << "\n";
}

static PublicKey read_public_key(const string& path) {
    ifstream in(path);
    if (!in) throw runtime_error("Cannot open public key: " + path);
    string header;
    in >> header;
    if (header != "RCPKC_PUBLIC_KEY_V1") throw runtime_error("Invalid public key header");
    PublicKey pk;
    string label;
    in >> label >> pk.q_bits;
    in >> label >> pk.message_bits;
    in >> label >> pk.q;
    in >> label >> pk.g;
    in >> label >> pk.h;
    return pk;
}

static PrivateKey read_private_key(const string& path) {
    ifstream in(path);
    if (!in) throw runtime_error("Cannot open private key: " + path);
    string header;
    in >> header;
    if (header != "RCPKC_PRIVATE_KEY_V1") throw runtime_error("Invalid private key header");
    PrivateKey sk;
    string label;
    in >> label >> sk.q_bits;
    in >> label >> sk.message_bits;
    in >> label >> sk.q;
    in >> label >> sk.g;
    in >> label >> sk.f;
    in >> label >> sk.Fg;
    return sk;
}

static ZZ encrypt_message(const PublicKey& pk, const ZZ& m, const ZZ& r) {
    if (m < 0 || m >= pk.g) throw runtime_error("Message must satisfy 0 <= m < g");
    ZZ e;
    rem(e, pk.h * r + m, pk.q);
    return e;
}

static ZZ decrypt_ciphertext(const PrivateKey& sk, const ZZ& e) {
    ZZ a, m;
    rem(a, sk.f * e, sk.q);
    rem(m, sk.Fg * a, sk.g);
    if (m < 0) m += sk.g;
    return m;
}

static void cmd_keygen(int argc, char** argv) {
    long q_bits = stol(get_arg(argc, argv, "--q-bits", "473"));
    long message_bits = stol(get_arg(argc, argv, "--message-bits", "225"));
    string pub_path = get_arg(argc, argv, "--public", "public.key");
    string priv_path = get_arg(argc, argv, "--private", "private.key");

    if (q_bits <= message_bits + 2) {
        throw runtime_error("q_bits must be greater than message_bits + 2");
    }

    ZZ two = conv<ZZ>(2);
    ZZ q = power(two, q_bits);
    ZZ g = power(two, message_bits) - 1;
    ZZ f = power(two, q_bits - message_bits - 1) - 1;

    ZZ Fq = InvMod(f, q);
    ZZ h;
    rem(h, Fq * g, q);
    ZZ Fg = InvMod(f, g);

    PublicKey pk{q_bits, message_bits, q, g, h};
    PrivateKey sk{q_bits, message_bits, q, g, f, Fg};

    write_public_key(pub_path, pk);
    write_private_key(priv_path, sk);

    cout << "Generated RCPKC key pair\n";
    cout << "Public key:  " << pub_path << "\n";
    cout << "Private key: " << priv_path << "\n";
}

static void cmd_encrypt(int argc, char** argv) {
    string pub_path = get_arg(argc, argv, "--public", "public.key");
    string cipher_path = get_arg(argc, argv, "--cipher", "cipher.txt");
    string msg_str = get_arg(argc, argv, "--message");
    if (msg_str.empty()) throw runtime_error("Missing --message <integer>");

    PublicKey pk = read_public_key(pub_path);
    ZZ m = conv<ZZ>(msg_str.c_str());

    ZZ r;
    string r_str = get_arg(argc, argv, "--r");
    if (!r_str.empty()) {
        r = conv<ZZ>(r_str.c_str());
    } else {
        long r_bits = pk.q_bits - pk.message_bits - 2;
        r = RandomLen_ZZ(r_bits);
    }

    ZZ e = encrypt_message(pk, m, r);
    write_zz_file(cipher_path, e);
    cout << "Ciphertext written to " << cipher_path << "\n";
}

static void cmd_decrypt(int argc, char** argv) {
    string priv_path = get_arg(argc, argv, "--private", "private.key");
    string cipher_path = get_arg(argc, argv, "--cipher", "cipher.txt");
    PrivateKey sk = read_private_key(priv_path);
    ZZ e = read_zz_file(cipher_path);
    ZZ m = decrypt_ciphertext(sk, e);
    cout << m << "\n";
}

static void cmd_bench(int argc, char** argv) {
    string pub_path = get_arg(argc, argv, "--public", "public.key");
    string priv_path = get_arg(argc, argv, "--private", "private.key");
    string messages_path = get_arg(argc, argv, "--messages");
    string out_path = get_arg(argc, argv, "--out", "ResultsFile_cli.csv");
    if (messages_path.empty()) throw runtime_error("Missing --messages <csv>");

    PublicKey pk = read_public_key(pub_path);
    PrivateKey sk = read_private_key(priv_path);

    ifstream in(messages_path);
    if (!in) throw runtime_error("Cannot open messages file: " + messages_path);
    ofstream out(out_path);
    if (!out) throw runtime_error("Cannot write output file: " + out_path);

    out << "message,ciphertext,decrypted,encryption_ns,decryption_ns,correct\n";
    string line;
    long r_bits = pk.q_bits - pk.message_bits - 2;
    while (getline(in, line)) {
        if (line.empty()) continue;
        ZZ m = conv<ZZ>(line.c_str());
        ZZ r = RandomLen_ZZ(r_bits);

        auto t1 = chrono::high_resolution_clock::now();
        ZZ e = encrypt_message(pk, m, r);
        auto t2 = chrono::high_resolution_clock::now();
        ZZ recovered = decrypt_ciphertext(sk, e);
        auto t3 = chrono::high_resolution_clock::now();

        auto enc_ns = chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
        auto dec_ns = chrono::duration_cast<chrono::nanoseconds>(t3 - t2).count();
        out << m << "," << e << "," << recovered << "," << enc_ns << "," << dec_ns << "," << (m == recovered ? 1 : 0) << "\n";
    }

    cout << "Benchmark results written to " << out_path << "\n";
}

int main(int argc, char** argv) {
    if (argc < 2 || has_arg(argc, argv, "--help")) {
        usage();
        return argc < 2 ? 1 : 0;
    }

    try {
        string command = argv[1];
        if (command == "keygen") cmd_keygen(argc, argv);
        else if (command == "encrypt") cmd_encrypt(argc, argv);
        else if (command == "decrypt") cmd_decrypt(argc, argv);
        else if (command == "bench") cmd_bench(argc, argv);
        else {
            usage();
            return 1;
        }
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
