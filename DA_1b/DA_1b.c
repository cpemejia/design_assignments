/*
 * DA_1b_C.c
 *
 * Created: 2/20/2021 3:06:16 AM
 * Author : ElmerOMejia
 */

#include <stdio.h>
#define MAX 112        // define max constant
#define INITIAL 33     // define initial value constant

int STARTADDS[MAX];    // array to store each integer
int EVENS[MAX/2];      // array to store each even integer
int ODDS[MAX/2];       // array to store each odd integer

int main() {

// integer variables for process
  int i, x, y;        
  int counter;
  int total;
  int evenTotal;
  int oddTotal;
  int temp;
  
// initialize variables 
  counter = INITIAL;
  total = 0;          
  evenTotal = 0;
  oddTotal = 0;
  temp = 0;
  x = 0;
  y = 0;

        for (i = 0; i < MAX; i++){                   
          STARTADDS[i] = counter;                         // store each int in STARTADDS array
          total = total + counter;                        // running sum of total
	        temp = counter;                                 // copy counter into a temporary int
	        if (temp % 2 == 0){                             // check for even    
              EVENS[x] = counter;                         // store in EVEN array            
	            evenTotal = evenTotal + counter;            // running sum of even int
              x++;                                        // increment x
	       }
	       else {                                           // else it is odd
              ODDS[y] = counter;                          // store in ODD array
              oddTotal = oddTotal + counter;              // running sum of odd int             
	            y++;                                        // increment y
	       }
          counter++;                                      // increment counter
        }

        return 0;
}
