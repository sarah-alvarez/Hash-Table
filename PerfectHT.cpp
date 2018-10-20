// File: PerfectHT.cpp
//
// UMBC CMSC 341 Fall 2017 Project 5
//
// Implementation of HashFunction, SecondaryHT & PerfectHT classes.
//
// Version: 2017-11-30
//

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <vector>

using namespace std ;

#include "PerfectHT.h"
#include "primes.h"


// =====================================================================
// HashFunction class implementation 
// =====================================================================


// By default do not print info to screen
//
bool HashFunction::m_debug = false ;


// Constructor.
// The constructor picks the smallest prime # greater than
// or equal to n for the table size. 
// Default value of n defined in header.
//  
// Initializes other hash function parameters randomly.
//
HashFunction::HashFunction(int n /* =100*/ ) {

  // note: maxPrime defined in prime.h
  //
  if (n > maxPrime) throw out_of_range("HashTable size too big.\n") ;

  m_tsize = findPrime(n) ;
  m_factor = ( rand() % (m_tsize - 1) ) + 1 ;
  m_shift =  rand() % m_tsize ;
  m_multiplier = 33 ;  // magic number from textbook
  m_reboots = 0 ;

}


// Function that maps string to unsigned int.
// Return value can be much larger than table size.
// Uses m_multiplier data member.
// Return value must be unsgined for overflow to work correctly.
//
unsigned int HashFunction::hashCode(const char *str) const {

  unsigned int val = 0 ; 

  int i = 0 ;
  while (str[i] != '\0') {
    val = val * m_multiplier + str[i] ;
    i++ ;
  }
  return val ; 
}


// Getter for table size
//
int HashFunction::tableSize() const {
  return m_tsize ;
}


// Overloaded () operator that makes this a "functor" class.
// Returns the slot in the hash table for str.
// Uses the MAD method: h(x) = (ax + b) % m where 
// the parameters are:
//   a = m_factor
//   b = m_shift
//   m = m_tsize
//
unsigned int HashFunction::operator() (const char *str) const {
  return ( m_factor * hashCode(str) + m_shift ) % m_tsize ;
}


// Pick new parameters for MAD method and the hashCode function.
// Note: can change table size.
//
void  HashFunction::reboot() {

  m_reboots += 1;

  // Implement the reboot function
  // pick new values for m_factor and m_shift each reboot
  //   make sure you get *different* values
   
  int oldFactor = m_factor; // must have 0 < m_factor < m_tsize
  int oldShift = m_shift; // must have 0 <= m_shift < m_tsize
   
  while (m_factor == oldFactor)
    m_factor = rand() % (m_tsize - 1) + 1;
  while (m_shift == oldShift)
    m_shift = rand() % (m_tsize - 1) + 0;
   
  // change m_multiplier to next odd number  every 3rd reboot
   
  if (m_reboots % 3 == 0)
    m_multiplier += 2;
   
  // increase table size to next prime # every 5th reboot

  if (m_reboots % 5 == 0)
    m_tsize = findPrime(m_tsize);
   
  if (m_debug == true) {
    cout << "reboot #" << m_reboots << ": m_tsize = " << m_tsize
	 << " m_multiplier = " << m_multiplier 
	 << " m_factor = " << m_factor
	 << " m_shift = " << m_shift
	 << endl;
  }

}


// Set random seed for the random number generator.
// Call once at the start of the main program.
// Uses srand() and rand() from cstdlib which is
// shared with other code. For compatibility with
// C++98, it does not have a private random number 
// generator (e.g., mt19337).
//
void HashFunction::setSeed(unsigned int seed) {
  srand(seed) ;
}


// Find the smallest prime number greater than or equal to m.  

int binarySearch(const int arr[], int l, int r, int x)
{
  if (r >= l)
    {
      int mid = l + (r - l)/2;
 
      // If the element is present at the middle itself
      if (arr[mid] == x)  return (mid + 1 > numPrimes - 1)? numPrimes - 1 : mid + 1;
 
      // If element is smaller than mid, then it can only be present
      // in left subarray
      if (arr[mid] > x) return binarySearch(arr, l, mid-1, x);
 
      // Else the element can only be present in right subarray
      return binarySearch(arr, mid+1, r, x);
    }
 
  // We reach here when element is not present in array
  return (r + 1 > numPrimes - 1)? numPrimes - 1 : r + 1;
}

int HashFunction::findPrime(int m) {

  // Prime numbers are in global array primes[]
  // included from "primes.h"
  //
  // Use binary search!!
   
  if (m > maxPrime) 
    throw std::out_of_range ("This number is bigger than maxPrime (in findPrime)...");   
  else     
    return primes[(binarySearch(primes, 0, numPrimes - 1, m))];

}


// =====================================================================
// SecondaryHT class implementation 
// =====================================================================


// By default do not print debugging info 
bool SecondaryHT::m_debug = false ;


// Create a secondary hash table using the char * strings
// stored in "words". Makes copy of each item in words.
//

SecondaryHT::SecondaryHT(vector<const char *> words) {
  vector <bool> collisionTB(0, false);
  int index = 0;
  bool collision = false;
  HashFunction tempHash(words.size() * words.size());
  hash = tempHash;
  
  while(m_attempts < maxAttempts) {
    m_attempts += 1;
    vector <bool> collisionTB(hash.tableSize(), true);
    int index;
    
    for (int i=0; i < words.size(); i++){
      index = hash(words[i]);
      
      if (collisionTB[index] == false){
	collisionTB[index] = true;
	collision = false;
      }
      else{
	collision = true;
	break;
      }
    }
    
    if (collision){
      hash.reboot();
    }
    else {
      cout << "HASH SUCCESS. Attempts made = " << m_attempts << endl;
      cout << "HASHTABLESIZE = " << hash.tableSize() << endl;
    }
  }
  if (m_attempts >= maxAttempts){
    very_unlucky unlucky("Too many attempts tried on T2...");
  }/*
     else {
     T2 = new char*[(words.size()*words.size())];
     for (int j = 0; j < words.size(); j++) {
     index = hash(words.at(i));
     T2[index] = strdup((words.at(i)));
     m_size += 1;
     }
     }*/
  
}



// Copy constructor. Remember to strdup.
//
SecondaryHT::SecondaryHT(const SecondaryHT& other) {
}


// Destructor. Use free() to free strings.
//
SecondaryHT::~SecondaryHT() {
}


// Assignment operator. Remember to use strdup() and free()
//
const SecondaryHT& SecondaryHT::operator=(const SecondaryHT& rhs) {
}


// returns whether given word is in this hash table.
//
bool SecondaryHT::isMember (const char *word) const {

  int slot = hash(word) ;

  assert ( 0 <= slot && slot < hash.tableSize() ) ;

  if (T2[slot] == NULL) return false ;

  if ( strcmp(word, T2[slot]) != 0 ) return false ;

  return true ; 
}


// getter. Actually, the hash function has the size.
//
int SecondaryHT::tableSize() const {
  return hash.tableSize() ;
}


// Pretty print for debugging
//
void SecondaryHT::dump() const {
  cout << "=== Secondary Hash Table Dump: ===\n" ;
  cout << "Table size = " << hash.tableSize() << endl ;
  cout << "# of items = " << m_size << endl ;
  cout << "# of attempts = " << m_attempts << endl ;
 
  for (int i=0 ; i < hash.tableSize() ; i++) {
    cout << "T2[" << i << "] = " ;
    if (T2[i] != NULL) cout << T2[i] ;
    cout << endl ;
  }

  cout << "==================================\n" ;
}



// =====================================================================
// PerfectHT class implementation 
// =====================================================================


// by default, do not print debugging info
//
bool PerfectHT::m_debug = false ;


// Create a Perfect Hashing table using the first n strings 
// from the words array.
//
PerfectHT::PerfectHT (const char *words[], int n) {

  // Implement constructor for PerfectHT here.
  //
  // You will need an array of vectors of char * strings.
  // Something like;
  //
  //    vector<const char *> *hold = new vector<const char *>[m] ;
  //
  // Each hold[i] is a vector that holds the strings
  // that hashed to slot i. Then hold[i] can be passed
  // to the SecondaryHT constructor.
  //

   
}


// Copy constructor. Use strdup or SecondaryHT copy constructor.
//
PerfectHT::PerfectHT(const PerfectHT& other) {
}


// Destructor. Remember to use free() for strdup-ed strings.
//
PerfectHT::~PerfectHT() {
}


// Assignment operator. Use strdup() and free() for strings.
//
const PerfectHT& PerfectHT::operator=(const PerfectHT& rhs) {
}


// Returns whether word is stored in this hash table.
//
bool PerfectHT::isMember(const char * str) const {
  int slot = hash(str) ; 

  if (PHT1[slot] == NULL && PHT2[slot] == NULL) return false ;

  if (PHT1[slot] != NULL) return strcmp(str,PHT1[slot]) == 0 ;

  return PHT2[slot]->isMember(str) ;
}


// Pretty print for debugging.
//
void PerfectHT::dump() const {
  int m = hash.tableSize() ;   

  cout << "------------- PerfectHT::dump()  -------------\n" ;
  cout << "table size = " << hash.tableSize() << endl ;
  cout << endl ;
  for (int j=0 ; j < m ; j++) {
    cout << "[" << j << "]:  " ;
    if (PHT1[j] != NULL) cout << PHT1[j] ;
    if (PHT2[j] != NULL) {
      cout << endl ;
      PHT2[j]->dump() ;
    }
    cout << endl ;
  }

  cout << "----------------------------------------------\n" ;
}
