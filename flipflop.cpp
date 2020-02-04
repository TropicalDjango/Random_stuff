
#include <math.h>
#include <iostream>
#include <unistd.h>
using namespace std;

/*
reverse() was a function that I was going to use to sort the last indexes of the array.
I soon learned that this method kinda sucked. It remains because I may need it in the future.
*/

void reverse(uint32_t a[],uint32_t n) {
uint32_t temp = 0;
	for(uint32_t i = 0; i < n/2 ; i++) {
		temp = a[i];
		a[i] = a[n-i -1];
		a[n-i -1] = temp;
	}
}

/*
swap() just swaps two given indexes of an array
*/

void swap(uint32_t &a,uint32_t &b) {
uint32_t c = a;
	a = b;
	b = c;
}

/*
flipFlopSort() takes a pointer to an array and the length of the array and sorts the
array in ~= O(n^2.7) time.
*/

void flipFlopSort(uint32_t *p, uint32_t length) {

	if (length == 2){
		if(*(p) > *(p+1)){
			swap(*(p),*(p+1));
		}
	}
	else if (length == 1) {
		return;
	}

	else {
		uint32_t nf = (2*length+3-1)/3;
		
		flipFlopSort(p, nf);
		flipFlopSort(p+length-nf, nf);
		flipFlopSort(p, nf);

		
	}
	
}


int main() {

	uint32_t n;
	cin >> n;
	uint32_t array[n];
	uint32_t* p = array;

  // This part is to get the input and to initalize everything
	for (uint32_t i = 0; i < n ; i++) {
		cin >> array[i];
	}

	// Runs flipsort to sort the array
  flipFlopSort(p, n);

	// This outputs the array
	 for (uint32_t i = 0; i < n ; i++){
	 	cout << array[i] << ' ';
	 }	
   
	return 0;
}
