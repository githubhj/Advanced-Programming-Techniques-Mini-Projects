// Test the GNU Multiprecision library
// George F. Riley, Georgia Tech, Summer 2003

#include <iostream>
#include <string>
#include <fcntl.h>

#include "gmpxx.h"

using namespace std;


gmp_randclass rng(gmp_randinit_default);

void Init()
{ // Initialize the random number generator
  int dr = open("/dev/random", O_RDONLY);
  if (dr < 0)
    {
      cout << "Can't open /dev/random" << endl;
    }
  unsigned long drValue;
  read(dr, (char*)&drValue, sizeof(drValue));
  //cout << "drValue " << drValue << endl;

  rng.seed(drValue);

}

void GetPrime(mpz_class& p, unsigned long sz, unsigned long c)
{ // Get a prime of "sz" bits, with "c" interations of the primality test
  while(1)
    {
      p = rng.get_z_bits(sz);
      if (mpz_probab_prime_p(p.get_mpz_t(), c)) return;
    }
}
void InitRSA(mpz_class& n, mpz_class& d, mpz_class& e)
{ // Choose appropriate values for RSA encryption
  mpz_class p;
  mpz_class q;
  // Get prime numbers for p and q
  GetPrime(p, 128, 100);
  GetPrime(q, 128, 100);
  n = p * q;

  mpz_class p1 = p - 1;
  mpz_class q1 = q - 1;
  mpz_class phi = p1 * q1;
  // Choose d
  cout << "Choosing d" << endl;
  // Choose d as a prime > max(p,q)
  mpz_class maxpq;
  if (p > q) maxpq = p;
  else       maxpq = q;
  while(1)
    {
      // Possible bug here.  Should try only a limited number of
      // times, and re-choose p/q if can't find one
      GetPrime(d, 128, 100);
      if (d > maxpq) break;
    }
#ifdef OLD_WAY
  while(1)
    {
      d = rng.get_z_bits(128 * 2);
      if (d < phi)
        {
          mpz_class gcd;
          mpz_gcd(gcd.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
          if (gcd == 1) break;
        }
    }
#endif
  cout << "Got d" << endl;
  // Find e, the multiplicative inverse of d
  mpz_invert(e.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
}

void RSAEncrypt(mpz_class& C, mpz_class& M, mpz_class& d, mpz_class& n)
{ // Encrypt M with d and n, encrypted value in C
  mpz_powm(C.get_mpz_t(), M.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
}

int main()
{
  Init();
  mpz_class n;
  mpz_class d;
  mpz_class e;
  InitRSA(n, d, e);
  cout << "n is " << n << endl;
  cout << "d is " << d << endl;
  cout << "e is " << e << endl;
  for (int k = 0; k < 50000; ++k)
    {
      mpz_class M;
      mpz_class C;
      mpz_class Mprime;

      M = rng.get_z_bits(240); // Random message, 240 bits
      RSAEncrypt(C, M, d, n);  // C is encrypted message
      RSAEncrypt(Mprime, C, e, n);  // Mprime is unencrypted
      if (M != Mprime)
        { // Failed, Mprime != M
          cout << "Encrypt/Decrypt failed" << endl;
          cout << "C  is " << C << endl;
          cout << "M  is " << M << endl;
          cout << "M' is " << Mprime << endl;
        }
      else
        {
          cout << "Encrypt/Decrypt worked!" << endl;
        }
      cout << "C  is " << C << endl;
      cout << "M  is " << M << endl;
      cout << "M' is " << Mprime << endl;
    }
}
