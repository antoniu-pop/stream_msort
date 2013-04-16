// Merge sort
// http://www.algorithmist.com/index.php/Merge_sort.cpp

#include <stdio.h>
#include <stdlib.h>
#include <memory.h> // malloc
//#include <conio.h> // getch

#define _DEBUG_THIS
#ifdef _DEBUG_THIS
#define XLOG(...) fprintf (stderr,__VA_ARGS__)
#else
#define XLOG(...) 		/*  */
#endif

typedef struct list{ // defining list node
  int inf;
  struct list * next;
} lista;

void print_list(lista *p){
  printf("List:  ");
  while(p){
    printf("%d ", p->inf);
    p = p->next;
  }
  printf("\n");
  //  getch();
}

// Sorts first n elements of array a[] in non-decreasing order.
void stream_merge_sort(int n, lista **p, int sout __attribute__ ((stream))){
  int i, j, k;
  int x;
  int tmp;
  lista *p1, *p2, *pred, *last, *t1, *t2;

  if(!n){	  // count elements of the list
    p1 = *p;
    while(p1){
      p1 = p1->next;
      n++;
    }
  }

  XLOG ("stream_merge_sort with n:%d\n", n);

  if(n < 2)
#pragma omp task output (sout << x)
    {
      x = 1;
    }
  else
    {

      // split the list on two halves
      int f = n / 2;		// f = number of elements in first half
      pred = *p;
      for(i=0;i<f-1;i++)
	pred = pred->next;
      p2 = pred->next;
      pred->next = NULL; // break the list p into lists p and p2

      // Recursively sort both halves 
      p1 = *p;

      int s1 __attribute__ ((stream));
      int s2 __attribute__ ((stream));
      int v1, v2;

      stream_merge_sort(f, &p1, s1);
      stream_merge_sort(n-f, &p2, s2);

#pragma omp task input (s1>>v1, s2>>v2) output (sout <<x)
      {
	// Merge
	// first element of the result list
	*p = last = NULL; // result list
	t1 = p1;
	t2 = p2;
	if(p1 && p2){ // finds first element for the result list *p
	  if(p1->inf < p2->inf){
	    last = *p = p1;
	    p1 = p1->next;
	  }else{
	    last = *p = p2;
	    p2 = p2->next;
	  }
	}else{
	  if(p1){
	    last = *p = p1;
	    p1 = p1->next;
	  }else{
	    last = *p = p2;
	    p2 = p2->next;
	  }
	}

	// populate result list while none of p1 and p2 lists are empty
	while(p1 && p2)
	  if(p1->inf < p2->inf){
	    last = last->next = p1;
	    p1 = p1->next;
	  }else{
	    last = last->next = p2;
	    p2 = p2->next;
	  }
 
	// concatenates result with the rest of the list, which is not empty, if such exists
	if(p1)
	  last->next = p1;
	else
	  last->next = p2;

	x = v1 + v2;
      }
    }
}
 
// Example of usage: 
int main(){
  int stream __attribute__ ((stream));
  int result;

  int n = 8, i;
  lista *p = NULL, *p1, *last;

  // initialize list/array
  for (i = 0; i < n; i++){
    p1 = (lista *) malloc( sizeof(lista) ); // reserves memory for one list node
    p1->inf = rand();
    p1->next = NULL;
    if(!p)
      last = p = p1; // first created element of the list
    else
      last = last->next = p1; // adds element to the end of the list
  }
  print_list(p);

  // merge
  stream_merge_sort(0, &p, stream);

#pragma omp task input (stream >> result) firstprivate (p)
  {
    // print result
    print_list(p);
    //free memory reserved for the list
    while(p){
      p1 = p;
      p = p->next;
      free(p1);
    }
  }
}
