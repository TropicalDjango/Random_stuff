
#include <iostream>
#include <iomanip>
using namespace std;

//input() does as the name suggests and 
//cins a 32 bit number from the terminal and returns that number.

unsigned int input(){
	unsigned int n;
	cin >> n;
	return n;
}
//gcd(a,b) is the fast euclidiean algorthim
unsigned int gcd(unsigned int a, unsigned int b) {
  while (b > 0) {
    a %= b;

    // now swap them
    uint32_t tmp = a;
    a = b;
    b = tmp;
  }
  return a; // b is 0
}
/*
bit_test(n) is the function which inputs n and outputs rez which is the 
amount of numbers which gcd(n,rez) == 1 in the range of 2^14 <= rez < 2^15,
 meaning that rez is a 15 bit number. 
*/
double bit_test(unsigned int n){
	unsigned int rez= 0;
	for (int k = 16384; k < 32768; k += 1){ // k is bounded by 2^14 and 2^15
		if(gcd(n,k) == 1){
			rez += 1;
		}
	}
	return rez;
}
/*
phi(n) is the fucntion which takes n and finds the number of relative prime numbers 
to n (i.e gcd(n,k) == 1). The way the algorithim works is that it first assigns the 
value n to result which we will alter as we moves along. Afterwards, we enter 
into a for loop which starts at 2 (since 1 is always a prime number relative to 
everything) and stop at sqrt(n). Then, we find the first relative prime number 
of n and enter the if statment, afterwards we naturally get caught in the while loop. 
In the while loop, we continously divide by the first prime number k until, 
n cannot be divided anymore. After this, it leaves the while loop and updates 
the value of result by first dividing by k (to avoid overflow), then multipliying 
by (k-1).It then repeats this process but, with the new updated value of n. 
Afterwards, if n!= 1, then it multiples by the last prime number which is the 
remaning value of n, so results *= (n-1)/n.
*/
unsigned int phi(unsigned int n){
	unsigned int result = n; 
	for(unsigned int k = 2 ; k*k<=n ; k++){ 
		if (n%k == 0){						// if k is a relative prime
			while (n%k == 0){				//while k is still a relative prime
				n /= k;
			}
			result /= k;
			result *= (k-1);
		}
	}
	if (n != 1){							// multiplies by remaning n value
		result /= n;
		result *= n-1;
	}

	return result;
}
/*
output(n,phi) is the function which takes inputs n and phi 
and returns the formatted output for the values it also calculates phi/n.
*/
void output(unsigned int n, double pi){
	double phi_n = pi/n ;
	cout << fixed << setprecision(5);
	cout << "n           = " << n << '\n';
	cout << "phi(n)      = " << (unsigned int)pi << '\n';
	cout << "phi(n)/n    = " << phi_n << '\n';
	if (n >= 32768){                       //if n is greater than 15-bits
		double bit_t = bit_test(n);
		cout << "15-bit test = " << (float)bit_t/16384 << '\n';
	}
}
// main() just calls all the other functions
int main(){
	unsigned int n = input();
	unsigned int pi = phi(n);
	output(n,pi);
	return 0;
}
