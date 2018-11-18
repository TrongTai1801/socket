#include <iostream>
#include <stdio.h>
using namespace std;
int main(int argc, char const *argv[])	{

	int value = 100;
	int *ptr = &value;
	int **p_to_p = (int**) calloc(10, sizeof(int));//= &ptr;

    // cout << ptr << endl;
    // cout << &value << endl;
    // cout << &ptr << endl;
    // cout << *ptr << endl;
	cout << p_to_p << endl; //print address of ptr
	cout << *p_to_p << endl; //print address which hold by ptr
	cout << **p_to_p << endl; //print value at address which hold by ptr

	return 0;
}