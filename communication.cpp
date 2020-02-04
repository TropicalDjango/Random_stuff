
#include <Arduino.h>
using namespace std;

const int pin = 13;

/* 
Setup() initalizes the arduinos and opens the Serial ports for data exchange.
*/

void setup() {
    init();
    Serial.begin(9600);
    Serial3.begin(9600);
    pinMode(pin, INPUT);
}

/*
isPrime(uint32_t n) takes a 32 bit unsigned interger as input and outputs true if the 
number is a prime number or false if not 
*/

bool isPrime(uint32_t n) {
    for (uint32_t i = 2; i <= sqrt(n); i++) {
        if (n%i == 0) {
            return false;
        }
    }
    return true;
}

/*
number_gen(uint32_t k_bit) generates a random k-bit number using analogRead() and concatenating
with bitwise OR.
*/

uint32_t number_gen(uint32_t k_bit) {
    uint32_t num = 0;
    for (uint32_t i = 0; i < k_bit ; i++) {
        uint32_t val = analogRead(A1);
        // shift num 1 bit to the left
        // then concatenate a random value of 0 or 1
        // using masking with bitwise OR "|". Because the least significant
        // bit of num is always 0 after the left shift, bitwise OR will change
        // this bit to 0 if val is 0, and 1 if val is 1. The rest of the bit
        // stay the same ("|" with 0)
        num = (num << 1) | (val&1);
        delay(50);
    }
    return num;
}

/*
random_prime_gen(uint32_t k_bit) takes a 32 bit unsigned integer and outputs a random
prime number that is k_bits long.
*/

uint32_t random_prime_gen(uint32_t k_bit) {
    // Generate a random k_bit number p
    // then add 2^k to p so that p is in the range [2^k, 2^k+1)
    // If p is a prime, return p
    // If p is not a prime, add 1 to p, if p exceeds 2^k_bit in this step
    // set p = 2^(k_bit - 1) then continue the loop
    uint32_t p = number_gen(k_bit);
    p += pow(2, k_bit);
    while (!(isPrime(p))) {
        p += 1;
        if (p > pow(2, k_bit+1)) {
            p = pow(2, k_bit);
        }
    }
    return p;
}

/*
modulus(uint32_t p, uint32_t q) takes two 32 bit prime numbers as input and outputs
the product of those two numbers.
*/

uint32_t modulus(uint32_t p, uint32_t q) {
    uint32_t n;
    n = p*q;
    return n;
}

/*
pri_key(uint32_t phi, uint32_t public_key) generates a private key using phi and public_key.
it uses the extended euclidean algorithm to calculate the modular multiplicative inverse
of e*d = 1 mod(phi) to get d.
*/

int32_t pri_key(uint32_t phi, uint32_t public_key) {
    // Uses the Extended Euclidean Algorithm to calcuate the number x such that
    // a*x = 1 (mod b). a is the public key and b is phi
    uint32_t r0 = public_key;
    uint32_t r1 = phi;
    int32_t private_key = 1;
    int32_t s1 = 0;
    while (r1 > 0) {
        // calculate all the new values
        uint32_t q = r0/r1;
        uint32_t r2 = r0 - q*r1;
        int32_t s2 = private_key - q*s1;
        // update old values with new ones to "shift" i by 1
        // i.e r[i-1] => r[i], r[i] => r[i+1] same thing for s
        r0 = r1;
        r1 = r2;
        private_key = s1;
        s1 = s2;
    }
    return private_key;
}

/*
phi(uint32_t p, uint32_t q) calculates the value of phi using 2 random prime numbers.
*/

uint32_t phi(uint32_t p, uint32_t q) {
    uint32_t phi;
    phi = (p - 1)*(q -1);
    return phi;
}

/*
This is the fast euclidian algorithm
*/

uint32_t gcd(uint32_t a, uint32_t b) {
    while (b > 0) {
        a %= b;

        // now swap them
        uint32_t tmp = a;
        a = b;
        b = tmp;
    }
    return a;  // b is 0
}

/*
pub_key(uint32_t phi) takes the value of phi and calculates the value of public key.
It does this by first generating a random 15 bit number and checking if 
gcd(e,phi) == 1. If it isn't then it enters the while loop and generates another 15 bit
number until it finds a value that works
*/

uint32_t pub_key(uint32_t phi) {
    uint32_t e = number_gen(15);
    while (!(gcd(e, phi) == 1)) {
        e = number_gen(15);
    }
    return e;
}

// Give an integer x, possibly negative, return an integer
// in the range 0..m-1 that is congruent to x (mod m)

int32_t reduce_mod(int32_t private_key, int32_t m) {
    if (private_key >= 0) {
        return private_key%m;
    } else {
        private_key %= m;
        return private_key + m;
    }
}

/*
mulmod()'s pupose is to avoid overflowing the modular exponentiation. It does this
by dividing b by two mod(n) and taking the binary expansion of a bits and adding b 
recursivly to ans if a bit is 1. 
*/

uint32_t mulmod(uint32_t a, uint32_t b, uint32_t m) {
    uint32_t ans = 0;

    while (a > 0) {
        if ((a&1) == 1) {
            ans = (ans + b) % m;
        }
        // multiply b by 2, then take mod m
        b = (b << 1) % m;
        // continue to the next bit of a
        a >>= 1;
    }
    return ans;
}

/*
powmod()'s purpose is to calculate modular exponentials in a much faster way than raw
multiplication. It works by taking the binary expansion of the power (pow) and applying
mulmod to every 1 in the binary expansion of a. and applying pow_x with a mulmod to 
account for the reduction in power.  
*/

uint32_t powmod(uint32_t x, uint32_t pow, uint32_t m) {
    uint32_t ans = 1;
    uint32_t pow_x = x;

    while (pow > 0) {
        if ((pow&1) == 1) {
            ans = mulmod(ans, pow_x, m);
        }
        pow_x = mulmod(pow_x, pow_x, m);
        pow >>= 1;  // divides by 2
    }
    return ans;
}

/*
    Encrypts using RSA encryption.

    Arguments:
        c (char): The character to be encrypted
        e (uint32_t): The partner's public key
        m (uint32_t): The partner's modulus

    Return:
        The encrypted character (uint32_t)
*/

uint32_t encrypt(char c, uint32_t e, uint32_t m) {
    return powmod(c, e, m);
}

/*
    Decrypts using RSA encryption.

    Arguments:
        x (uint32_t): The communicated integer
        d (uint32_t): The Arduino's private key
        n (uint32_t): The Arduino's modulus

    Returns:
        The decrypted character (char)
*/

char decrypt(uint32_t x, uint32_t d, uint32_t n) {
    return (char) powmod(x, d, n);
}

/*
Writes an uint32_t to Serial3, starting from the least-significant
and finishing with the most significant byte.
*/

void uint32_to_serial3(uint32_t num) {
    Serial3.write((char) (num >> 0));
    Serial3.write((char) (num >> 8));
    Serial3.write((char) (num >> 16));
    Serial3.write((char) (num >> 24));
}

/*
Reads an uint32_t from  Serial3, staring from the least-significant
and finishing with the most significant byte.
*/

uint32_t uint32_from_serial3() {
    uint32_t num = 0;
    num = num | ((uint32_t) Serial3.read()) << 0;
    num = num | ((uint32_t) Serial3.read()) << 8;
    num = num | ((uint32_t) Serial3.read()) << 16;
    num = num | ((uint32_t) Serial3.read()) << 24;
    return num;
}

/*
Waits for a certain number of bytes on Serial3 or time out
    nbytes: the number of bytes we want
    timeout: timeout period (ms); specifying a negative number
            turns off timeouts (the function waits indefinitely
            if timeouts are turned off).
    returns true if the required number of bytes have arrived.
*/

bool wait_on_serial3(uint8_t nbytes, long timeout) {
  unsigned long deadline = millis() + timeout;
  while (Serial3.available() < nbytes && (timeout < 0 || millis() < deadline)) {
    delay(1);
  }
  return Serial3.available() >= nbytes;
}

/*
These are all of the enumerated states in which the connection could be in
*/

enum ConnectState {
    start, listen, waitACK, waitCR, waitKey, data
};

/*
Serial_clear() is used to clear all of the Serial ports so that no data is corrupted
during communication or connection.
*/

void Serial_clear() {
    while (Serial3.available() || Serial.available()) {
        Serial3.read();
        Serial.read();
    }
}

/*
Connection(uint32_t key, uint32_t mod, uint32_t &okey, uint32_t &omod) takes 2 values
and 2 pointers. It accepts the public key, and modulus of the arduino and updates the 
value of okey and omod which are the other arduinos public information. It doesn't 
return any values as the arduino cannot leave Connection() without actually entering
the data enumerated state, so if the arduino leaves Connection that means it connected.
*/

void Connection(uint32_t key, uint32_t mod, uint32_t &okey, uint32_t &omod) {
    ConnectState state = start;
    char ACK, CR;
    Serial_clear();
    if (digitalRead(pin) == HIGH) {  // server
        state = listen;
        while (state == listen) {
            // A single character 'C' takes 1 byte of space, so we wait for 1 byte.
            if (wait_on_serial3(1, 1000)) {
                state = waitKey;
                CR = Serial3.read();
                if (CR != 'C') {
                	// This flush is here because client should have
                	// sent a C so the data must be corrupted or server read junk
                    state = listen;
                    Serial3.flush();
                }
                // If the CR is valid and the server is
                // waiting for a key, loop continuously
                while ((CR == 'C') && (state == waitKey)) {
                // The mod and key are both 32 bit integers meaning in total
                // they are 8 bytes worth of data so, we wait for that much bytes
                    if (wait_on_serial3(8, 1000)) {
                        okey = uint32_from_serial3();
                        omod = uint32_from_serial3();
                        // Now we send the ACK and public information to other arduino.
                        Serial3.write('A');
                        uint32_to_serial3(key);
                        uint32_to_serial3(mod);
                        state = waitACK;
                        if (wait_on_serial3(1, 1000)) {
                            ACK = Serial3.read();
                            if (ACK == 'A') {
                            // if arduino receives ACK then data exchange
                                state = data;
                            } else if (ACK == 'C') {
                                // If arduino recieves a CR instead of ACK
                                // then wait for key.
                                state = waitKey;
                                Serial_clear();
                            } else {
                                // if ACK is something random,
                                // listen and flush junk
                                state = listen;
                                Serial_clear();
                            }
                        } else {
                            state = listen;  // If arduino receives no ACK then listen.
                        }
                    } else {
                        state = listen;  // If the arduino recieves a CR but no Key
                    }  // and Mod then return to listen state.
                }
            }
        }
    } else if (digitalRead(pin) == LOW) {  // client
        while (state == start) {  // Only way to leave Connection is to enter data state
            Serial3.write('C');
            uint32_to_serial3(key);
            uint32_to_serial3(mod);
            state = waitACK;
            if (wait_on_serial3(9, 1000)) {
                ACK = Serial3.read();
                okey = uint32_from_serial3();
                omod = uint32_from_serial3();
            // If no ACk, Key and mod are recieved go back to start
            } else {
                state = start;
            }
            if (ACK == 'A') {  // This part isn't really needed but it's here
                Serial3.write('A');  // Just in case some junk is in Serial3
                state = data;
            } else {
                state = start;
                Serial_clear();
            }
        }
    }
}

/*
Keygen() takes 3 pointer values mod, public_key, and private_key and calculates the
mod, public key and private key of the arduino. it first calls random_prime_gen()
in order to generate a 14 bit and a 15 bit prime number. it then uses these values
to find phi and mod. using phi it then calculates public key and then private after
that.
*/

void Keygen(uint32_t &mod, uint32_t &public_key, int32_t &private_key) {
    uint32_t p = random_prime_gen(14);
    uint32_t q = random_prime_gen(15);
    mod = p*q;
    uint32_t totient = phi(p, q);
    public_key = pub_key(totient);
    private_key = pri_key(totient, public_key);
    private_key = reduce_mod(private_key, totient);
}

void communication(uint32_t d, uint32_t n, uint32_t e, uint32_t m) {
    // Consume all early content from Serial3 to prevent garbage communication
    Serial_clear();

    // Enter the communication loop
    while (true) {
        // Check if the other Arduino sent an encrypted message.
        if (Serial3.available() >= 4) {
            // Read in the next character, decrypt it, and display it
            uint32_t read = uint32_from_serial3();
            Serial.print(decrypt(read, d, n));
        }

        // Check if the user entered a character.
        if (Serial.available() >= 1) {
            char byteRead = Serial.read();
            // Read the character that was typed, echo it to the serial monitor,
            // and then encrypt and transmit it.
            if ((int) byteRead == '\r') {
                // If the user pressed enter, we send both '\r' and '\n'
                Serial.print('\r');
                uint32_to_serial3(encrypt('\r', e, m));
                Serial.print('\n');
                uint32_to_serial3(encrypt('\n', e, m));
            } else {
                Serial.print(byteRead);
                uint32_to_serial3(encrypt(byteRead, e, m));
            }
        }
    }
}

/*
main() just calls the other functions which do all of the work.
*/

int main() {
    setup();

    uint32_t key, mod, omod, okey;
    int32_t pkey;
    Keygen(mod, key, pkey);
    Connection(key, mod, okey, omod);
    Serial.println("Welcome to Arduino chat");
    communication(pkey, mod, okey, omod);

    return 0;
}
