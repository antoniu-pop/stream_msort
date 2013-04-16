// Merge sort
// http://www.algorithmist.com/index.php/Merge_sort.cpp

#include <stdio.h>
#include <stdlib.h>
#include <memory.h> // malloc
//#include <conio.h> // getch

//#define _DEBUG_THIS
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
void stream_merge_sort(int n, lista **p, lista* sout __attribute__ ((stream))){
  int i, j, k;
  lista *x;
  int tmp;
  lista *p1, *p2, *pred, *last, *t1, *t2;

  if(!n){	  // count elements of the list
    p1 = *p;
    while(p1){
      p1 = p1->next;
      n++;
    }
  }

  lista *stmp = *p;
  XLOG ("[%d][msort_main]task started: stream_merge_sort\n", n);
  XLOG ("[%d][msort_main]*p is %p\n", n, *p);
  
  if(n < 2)
#pragma omp task output (sout << x)
    {
      XLOG ("[%d][msort_atomic]tmp is %p\n",n,stmp);
      x = stmp;
    }
  else
    {

      XLOG ("[%d][msort_main]*p is %p\n",n, *p);

      // split the list on two halves
      int f = n / 2;		// f = number of elements in first half
      pred = *p;
      for(i=0;i<f-1;i++)
	pred = pred->next;
      p2 = pred->next;
      pred->next = NULL; // break the list p into lists p and p2

      // Recursively sort both halves 
      p1 = *p;

      lista *s1 __attribute__ ((stream));
      lista *s2 __attribute__ ((stream));
      lista *v1, *v2;
      lista *ptmp = *p;

      stream_merge_sort(f, &p1, s1);
      stream_merge_sort(n-f, &p2, s2);

#pragma omp task input (s1>>v1, s2>>v2) output (sout <<x) firstprivate (p1,p2)
      {
	XLOG ("[%d][task]merging started...\n",n);
	XLOG ("[%d][task]v1:%p v2:%p \n",n,v1,v2);
	XLOG ("[%d][task]p1:%p p2:%p\n",n,p1,p2);
	p1=v1;p2=v2;
	// Merge
	// first element of the result list
	ptmp = last = NULL; // result list
	t1 = p1;
	t2 = p2;
	if(p1 && p2){ // finds first element for the result list ptmp
	  if(p1->inf < p2->inf){
	    last = ptmp = p1;
	    p1 = p1->next;
	  }else{
	    last = ptmp = p2;
	    p2 = p2->next;
	  }
	}else{
	  if(p1){
	    last = ptmp = p1;
	    p1 = p1->next;
	  }else{
	    last = ptmp = p2;
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

	XLOG ("[%d][task]",n);
	x = ptmp;
      }
    }
}
 
// Example of usage: 
int main(){
  lista* stream __attribute__ ((stream));
  lista *result;

  int n = 150, i;
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

  // merge
  stream_merge_sort(0, &p, stream);

#pragma omp task input (stream >> result) 
  {
    // print result
    p=result;
    print_list(p);
    //free memory reserved for the list
    while(p){
      p1 = p;
      p = p->next;
      free(p1);
    }
  }
}
