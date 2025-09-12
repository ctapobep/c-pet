#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "la/vector.h"

#define NAME "Clara"
#define SECONDS_IN_YEAR (3600 * 24 * 365)

/*
 * ARRAYS vs POINTERS vs STRINGS
 *
 * 1. It's possible to find out the size of the array: sizeof(a) / sizeof(a[0]), BUT ONLY IN THE FUNCTION WHERE IT WAS DEFINED!
 * 2. Incrementing a pointer increments it by the number of bytes in the variable
 * 3. We can't reassign an array variable because then the previous array isn't referenced
 */

void dealRandomHand(void);
void readUntilNewline(void);
void printArray(int arr[], int len);
void arrayLen(int arr[]);
void variableLenArray(int n, const int arr[n]);
void quicksort(int array[], int len);

int sum1(const int *array, int n) {
    int sum = 0;
    for(int i = 0; i < n; i++)
        sum += *array++;
    return sum;
}
int sum2(const int array[], int n) {
    int sum = 0;
    for(int i = 0; i < n; i++)
        sum += array[i];
    return sum;
}
int sum3(const int *array, int n) {
    int sum = 0;
    const int *p = array;
    while(p < &array[n])
        sum += *p++;
    return sum;
}
int sum4(const int array[], int n) {
    int sum = 0;
    for(const int *p = array; p < array+n; p++)
        sum += *p;
    return sum;
}
bool search(const int *a, size_t n, int key) {
    for(const int *p = a; p < &a[n]; p++)
        if(*p == key)
            return true;
    return false;
}
void storeZeros(int *a, int n) {
    for(int *p = a; p < a+n; p++)
        *p = 0;
}
int findMiddle(int *a, int n) {
    return *(a+n/2);
}
void findTwoLargest(int a[], int n, int *first, int *second) {
    int f = 0, s = 0;
    for(int *p = a; p < &a[n]; p++) {
        if(*p > s) {
            s = *p;
            if(s > f) {
                int tmp = s;
                s = f;
                f = tmp;
            }
        }
    }
    *first = f;
    *second = s;
}
void identity(int *ident, size_t n, int m) {
    for(int *p = ident, i = 0; p < ident + n*m; p++) {
        *p = i == 0 ? 1 : 0;
        if(i++ == m)
            i = 0;
    }
}



void assertEqualsDouble(double expected, double actual) {
    if(expected != actual) {
        printf("Expected: %f,\n  Actual: %f\n", expected, actual);
        assert(false);
    }
}
int main(void) {
    Vector v = {.dims = 3, .components = (double[]){1, 2, 3}};
    assertEqualsDouble(14, dot(&v, &v));
    assertEqualsDouble(-14, dot(&v, &(Vector) {.dims = 5, .components = (double[]) {-1, -2, -3}}));
    assertEqualsDouble(0, dot(&v, &(Vector) {.dims=3, .components=(double[]) {0, 0, 0}}));

    char str1[] = "hello brave new world";
    char str2[(sizeof str1) / (sizeof str1[0])];
//    str2 = str1;
//    strcpy(str2, str1);
    printf("%s\n", str1);
    printf("%s\n", str2);
    int n=3;
    int m=3;
    int ident[n][m];
    identity(&ident[0][0], n, m);
    for(int i = 0; i < n; i++)
        printArray(ident[i], m);
    int a[] = {1, 3, 4, 5};

    int temperatures[7][24] = {{0,1,3,4,5,6,-1}, {31, 31, 30, 24,1,-12,0}};
    printf("Found=%d\n", search(&temperatures[0][0], 7*24, 31));

    return EXIT_SUCCESS;
}



void readUntilNewline(void) {
    char ch;
    do {
        scanf("%c", &ch);
    } while(ch != '\n');
    // or a better one:
    while(getchar() != '\n')/*empty*/;
}
const char *bit_rep[16] = {
        [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
        [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
        [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
        [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};
void printBinary(uint32_t/*the signed value keeps its binary representation when cast to unsigned*/ x) {
    printf("%s%s%s%s%s%s%s%s",
           bit_rep[x>>28], bit_rep[x>>24 & 0xF], bit_rep[x>>20 & 0xF], bit_rep[x>>16 & 0xF],
           bit_rep[x>>12 & 0xF], bit_rep[x>>8 & 0xF], bit_rep[x>>4 & 0xF], bit_rep[x & 0xF]
    );
}

void printRepresentations(void) {
    int x = -2;
    printf("Decimal: %d, hex: %x, oct: %o, bin: ", x, x, x);
    printBinary(x);
    printf("\n");
}
void createZeroArray(void) {
    int a[10] = {0};// fills the array with 0's
    int b[10] = {1, 2, [9]=0, [3]=12};// fills the array with 0's
    printf("%d", a[0] + b[0]);
    memcpy(a/*target*/, b/*source*/, sizeof(a));
}

void experimentWithTypes(void) {
    int age = 0;
    printf("How old are you? ");
//    scanf("%d", &age);

    printf("My name is %20.20s, and I'm %d years old (%d sec) and have %.2f$ in my account\n",
           NAME, age, age * SECONDS_IN_YEAR, 100.5F);

    int i = 40;
    printf("|%d|%5d|%-5d|%5.32d|\n", i,i,i,i);

    _Bool b = 1;
    bool a = true;
    printf("%d, %d\n", a, b);

    long long l = 1ULL,
                       m = LLONG_MAX;
    printf("typical: %lld, largest: %lld\n", l, m);

    long double ld = 1.1232L;
    printf("Long double with an escape number\030: %Lf\n", ld);
    printf("Hello, %c\n", toupper('w'));

    uint32_t k = 15;
    size_t size = sizeof(k);
    printf("The size of %d is %zu bytes\n", k, size);
    printRepresentations();
}


// RANDOM CARD HAND
#define NUM_SUITS 4
#define NUM_RANKS 13

const char SUITS[NUM_SUITS] = {'c', 'd', 'h', 's'};
const char RANKS[NUM_RANKS] = {'2', '3', '4', '5', '6', '7', '8', '9', 't',
                               'J', 'Q', 'K', 'A'};

int requestHandSize(void) {
    int handSize;
    printf("How many cards in the hand: ");
    scanf("%d", &handSize); // NOLINT(*-err34-c)
    return handSize;
}

void dealRandomHand(void) {
    bool alreadyDealt[NUM_SUITS][NUM_RANKS] = {false};
    int handSize = requestHandSize();

    srand(time(NULL)); // NOLINT(*-msc51-cpp)
    printf("Your hand: ");
    for(int i = 0; i < handSize; i++) {
        int suit = rand() % NUM_SUITS;// NOLINT(cert-msc30-c, cert-msc50-cpp)
        int rank = rand() % NUM_RANKS;// NOLINT(cert-msc30-c, cert-msc50-cpp)
        if(alreadyDealt[suit][rank])
            continue;
        alreadyDealt[suit][rank] = true;
        printf("%c%c ", RANKS[rank], SUITS[suit]);
    }
    printf("\n");
}
void arrayLen(int arr[]) {
    printf("Array-as-parameter: array byte len: %lu length: %lu\n", sizeof(arr), sizeof(arr) / sizeof(arr[0]));
}
void variableLenArray(int n, const int arr[n]) {
    printf("Number of elements in the variable-length array (VLA): %d\n", n);
    printf("Number of elements in the variable-length array (VLA) using sizeof: %lu\n", sizeof(arr)/sizeof(arr[0]));
}

#define QS_ARRAY_LEN 10

void swap(int array[], int i, int j);
void printQuicksortArrayWithPivots(int arr[], int len, int pivot, int hi, int asterisk0Idx, int asterisk1Idx);
void quicksort0(int ar[], int lo, int hi);

void quicksort(int ar[], int len) {
    quicksort0(ar, 0, len-1);
}

void quicksort0(int ar[], int lo, int hi) {
    if(lo >= hi)
        return;
    int pivot = ar[lo];
    int i = lo+1,
            j = hi;
    while(true) {
        for(; ar[i] < pivot && i < j; i++)/*just increment*/;
        for(; ar[j] > pivot && i <= j; j--)/*just increment*/;
        if(i >= j)
            break;
        swap(ar, i, j);
    }
    swap(ar, lo, j);
    quicksort0(ar, lo, j-1);
    quicksort0(ar, j+1, hi);
}

// xterm colors: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
#define RED "\033[0;31m"
#define RESET "\033[0m"
#define BPURPBLE "\033[1;35m"
#define BBLACK "\033[1;30m"
#define ON_YELLOW "\033[43m"

void printQuicksortArrayWithPivots(int arr[], int len, int pivot, int hi, int asterisk0Idx, int asterisk1Idx) {
    for (int i = 0; i < len; ++i) {
        if(i == asterisk1Idx || i == asterisk0Idx)
            printf(RED);
        else if(i == pivot)
            printf(BPURPBLE);
        if(pivot <= i && i <= hi)
            printf(ON_YELLOW);
        printf("%d "RESET, arr[i]);
    }
    printf("\n");
}
void swap(int array[], int i, int j) {
    int tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
    printArray(array, QS_ARRAY_LEN);
}

void printArray(int arr[], int len) {
    for (int i = 0; i < len; ++i)
        printf("%d ", arr[i]);
    printf("\n");
}


int charNum(int num) {
    return (int)((ceil(log10(num))+1)*sizeof(char));
}



