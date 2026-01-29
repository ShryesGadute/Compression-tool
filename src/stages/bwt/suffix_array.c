#include <stdlib.h>
#include <stdbool.h>
#include "suffix_array.h"

void calc_bkt(unsigned int* bkt, unsigned int K, unsigned char* T, unsigned int n, bool end){
	unsigned int i;
	unsigned int total=0;
	for(i=0; i<K; i++) bkt[i] = 0;
	for(i=0; i<n; i++) bkt[T[i]]++;
	for(i=0; i<K; i++){
		total+=bkt[i];
		bkt[i] = end ? total-1 : total-bkt[i];
	}

}

void put_lms_substring_0(unsigned char* T, unsigned int n, unsigned int* bkt, unsigned int* SA){
	unsigned int i;
	unsigned int pos;
	bool current = 1; //type of the current character, true for L false for S
	bool left; //type of the character to the left

	for(i=0; i<n; i++) SA[i] = 0;

	for(i=n-1; i>0; i--){
		if(T[i-1] > T[i]) left = 1;
		else if(T[i-1] < T[i]) left = 0;
		else left = current;

		if(!current && left){ // condition checks if the character is lms
			pos = bkt[T[i]]--;
			SA[pos] = i;
		}
		current = left;
	}
}

void induce_sort_l_0(unsigned char* T, unsigned int n, unsigned int* SA, unsigned int* bkt, bool erase){
	unsigned int i,j;
	SA[bkt[T[n-1]]++] = n-1; //this is done because of the assumption that there exists a phantom smallest character at the end of the string
	for(i=0; i<n; i++){
		j = SA[i];
		if(j>0 && (T[j-1] >= T[j])){
			SA[bkt[T[j-1]]++] = j-1;
			if(erase) SA[i] = 0;
		}
	}
}

void induce_sort_s_0(unsigned char* T, unsigned int n, unsigned int* SA, unsigned int* bkt, bool erase){
	unsigned int i,j;
	if(erase){
		for(i=n-1; i>0; i--){
			j = SA[i];
			if(j>0 && (T[j-1] <= T[j])){
				SA[bkt[T[j-1]]--] = j-1;
				SA[i] = 0;
			}
		}
	}

	else{
		for(i=n-1; i>0; i--){
			j = SA[i];
			if(j>0 && (T[j-1] < T[j] || (T[j-1] == T[j] && bkt[T[j-1]] < i))){
				SA[bkt[T[j-1]]--] = j-1;
			}
		}
	}
}

void put_in_bkt_l(unsigned int* T1, unsigned int* SA1, unsigned int n1, unsigned int j){
	unsigned int c = T1[j];
	if(SA1[c] == EMPTY){
		if(c != n1-1 && SA1[c+1] == EMPTY){
			SA1[c+1] = j;
			SA1[c] = TWO_31 + 1; //msb of 1 indicates that its a bucket
		}
		else SA1[c] = j;
	}

	else if(SA1[c] < TWO_31){
		unsigned int store;
		unsigned int next = 0;
		unsigned int m = c;
		while(next < TWO_31){
			store = SA1[m];
			SA1[m--] = next;
			next = store;
		}
		SA1[c] = EMPTY;

		if(c != n1-1 && SA1[c+1] == EMPTY){
			SA1[c+1] = j;
			SA1[c] = TWO_31 + 1; //msb of 1 indicates that its a bucket
		}
		else SA1[c] = j;
	}

	else if(SA1[c] > TWO_31){
		unsigned int pos = SA1[c] - TWO_31 + 1;
		if(c+pos > n1-1 || SA1[c+pos] != EMPTY){
			unsigned int m = c;
			while(m!=c+pos-1){
				SA1[m] = SA1[m+1];
				m++;
			}
			SA1[c+pos-1] = j;
		}

		else{
			SA1[c+pos] = j;
			SA1[c] += 1;
		}
	}
}

void put_in_bkt_s(unsigned int* T1, unsigned int* SA1, unsigned int n1, unsigned int j){
	unsigned int c = T1[j];
	if(SA1[c] == EMPTY){
		if(c != 0 && SA1[c-1] == EMPTY){
			SA1[c-1] = j;
			SA1[c] = TWO_31 + 1; //msb of 1 indicates that its a bucket
		}
		else SA1[c] = j;
	}

	else if(SA1[c] < TWO_31){
		unsigned int store;
		unsigned int next = 0;
		unsigned int m = c;
		while(next < TWO_31){
			store = SA1[m];
			SA1[m++] = next;
			next = store;
		}
		SA1[c] = EMPTY;

		if(c != 0 && SA1[c-1] == EMPTY){
			SA1[c-1] = j;
			SA1[c] = TWO_31 + 1; //msb of 1 indicates that its a bucket
		}
			else SA1[c] = j;
	}

	//we don't check for EMPTY beacuse we have already checked it in the first if 
	else if(SA1[c] > TWO_31){
		unsigned int pos = SA1[c] - TWO_31 + 1;
		if(c-pos == EMPTY || SA1[c-pos] != EMPTY){
			unsigned int m = c;
			while(m!=c-pos+1){
				SA1[m] = SA1[m-1];
				m--;
			}
			SA1[c-pos+1] = j;
		}

		else{
			SA1[c-pos] = j;
			SA1[c] += 1;
		}
	}
}

void put_lms_substring_1(unsigned int* T1, unsigned int* SA1, unsigned int n1){
	unsigned int i;
	unsigned int m;
	bool current = 1;
	bool left;

	for(i=0; i<n1; i++) SA1[i] = EMPTY;

	for(i=n1-1; i>0; i--){
		if(T1[i-1] > T1[i]) left = 1;
		else if(T1[i-1] < T1[i]) left = 0;
		else left = current;

		if(!current && left) put_in_bkt_s(T1, SA1, n1, i);
		current = left;
	}

	//shift to right side
	for(i=n1-1; i>0; i--){
		if(SA1[i] > TWO_31 && SA1[i] != EMPTY){
			m = SA1[i] - TWO_31;
			while(m!=0){
				SA1[i] = SA1[i-1];
				i--;
				m--;
			}
			SA1[i] = EMPTY;
			i++;
		}
	}
}

void induce_sort_l_1(unsigned int* T1, unsigned int* SA1, unsigned int n1, bool erase){
	unsigned int i,j,m;
	put_in_bkt_l(T1, SA1, n1, n1-1); //put in bkt the last character i.e. n1-1th into its L type bkt
	for(i=0; i<n1; i++){
		j = SA1[i];
		if(j<TWO_31 && j>0 && T1[j-1] >= T1[j]){
			put_in_bkt_l(T1, SA1, n1, j-1);
			i -= (SA1[i] != j ? 1:0); //SA1[i] could have been shifted left
			if(erase) SA1[i] = 0;
		}
	}
	//shift to left side
	for(i=0; i<n1; i++){
		if(SA1[i] > TWO_31 && SA1[i] != EMPTY){
			j = SA1[i] - TWO_31;
			while(j!=0){
				SA1[i] = SA1[i+1];
				i++;
				j--;
			}
			SA1[i] = EMPTY;
			i--;
		}
	}

	//we do this so all the lms suffixes which where put but put_lms_substring_1 become empty for next step which is induce_sort_s_1	
	if(erase){
		for(i=0; i<n1; i++){
			if(SA1[i] == 0) SA1[i] = EMPTY;
		}
	}
}

void induce_sort_s_1(unsigned int* T1, unsigned int* SA1, unsigned int n1, bool erase){
	unsigned int i,j;
	if(erase){
		for(i=n1-1; i>0; i--){
			j = SA1[i];
			if(j<TWO_31 && j>0 && T1[j-1] <= T1[j]){
				put_in_bkt_s(T1, SA1, n1, j-1);
				i += (SA1[i] != j ? 1:0);
				SA1[i] = 0;
			}
		}

		//shift right side
		for(i=n1-1; i>0; i--){
			if(SA1[i] > TWO_31 && SA1[i] != EMPTY){
				j = SA1[i] - TWO_31;
				while(j!=0){
					SA1[i] = SA1[i-1];
					i--;
					j--;
				}
				SA1[i] = EMPTY;
				i++;
			}
		}

		for(i=0; i<n1; i++){
			if(SA1[i] == 0) SA1[i] = EMPTY;
		}
	}

	else{
		for(i=n1-1; i!=EMPTY; i--){	
			j = SA1[i];
			if(j<TWO_31 && j>0 && (T1[j-1] < T1[j] || T1[j-1]==T1[j] && T1[j-1] > i)){
				put_in_bkt_s(T1, SA1, n1, j-1);
				i += (SA1[i] != j) ? 1:0;
			}
		}
	}
}

unsigned int get_lms_length(unsigned char* T, unsigned int n, unsigned int lms_pos, unsigned char level){
	unsigned int i = 0;
	unsigned int next_char;
	unsigned int current_char = get_char(lms_pos);
	bool found = false;
	
	while(!found){
		next_char = get_char(lms_pos+i+1);
		if(current_char > next_char) found = true;
		current_char = next_char;
		i++;
	}
	unsigned int checkpoint = i;
	found = false;

	while(!found && (lms_pos+i) < n-1){
		next_char = get_char(lms_pos+i+1);
		if(current_char < next_char) found = true;
		else if(current_char > next_char) checkpoint = i+1;
		current_char = next_char;
		i++;
	}
	unsigned int next_lms_pos = ((lms_pos+i == n-1) ? n : checkpoint+lms_pos);

	return (next_lms_pos-lms_pos+1);
}

void name_lms_substrings(unsigned char* T, unsigned int* T1, unsigned int* SA1, unsigned int n, unsigned int n1, unsigned char level){
	unsigned int name = 0;
	unsigned int current_lms_len;
	unsigned int prev_lms_len = 0;//lengths of strings to be compared
	unsigned int current_pos, prev_pos;
	unsigned int i, j, iter = 0;
	bool equal = false;	
	for(i=0; i<n1; i++){
		current_pos = SA1[i];
		SA1[i] = 0;
		current_lms_len = get_lms_length(T, n, current_pos, level);
		if(current_lms_len == prev_lms_len && prev_pos+prev_lms_len-1 != n && current_pos+current_lms_len-1 != n){
			for(iter=0; iter < prev_lms_len; iter++){
				if(get_char(current_pos+iter) != get_char(prev_pos+iter)) break;
			}
			if(iter == prev_lms_len) equal = true;
		}
		
		if(!equal) name = i;
		

		SA1[n1+current_pos/2] = name;
		SA1[name]++;
		equal = false;
		prev_pos = current_pos;
		prev_lms_len = current_lms_len;
	}

	iter = 0;
	i = n-1;
	
	//collect names and make Z1 adjacent to T1
	while(iter<n1){
		j = SA1[i];
		if(j != EMPTY){
			SA1[i] = EMPTY;
			T1[n1-1-iter] = j;
			iter++;
		}
		i--;
	}

	//Make the final string in T1
	bool current = 1;
	bool left;
	for(i=n1-1; i>0; i--){
		if(T1[i-1] > T1[i]) left = 1;
		else if(T1[i-1] < T1[i]) left = 0;
		else left = current;
		current = left;
		if(!left && SA1[T1[i-1]] != 1){
			SA1[T1[i-1]+1] += 1; //this helps in checking if all the names are unique later
			T1[i-1] += (SA1[T1[i-1]]-1);
		}
	}	
}


bool check_names(unsigned int* SA1, unsigned int n1){ //function checks if all the names are unique
	bool unique = true;
	unsigned int i=0;
	unsigned int name_count;
	while(unique && i<n1){ 
		name_count = SA1[i];
		if(name_count > 2 || (name_count == 2 && SA1[i+1] != 1)) unique = false;
		i += name_count;
	}

	return unique;
}


void put_lms_suffix_0(unsigned char* T, unsigned int* SA, unsigned int* T1, unsigned int* bkt, unsigned int n1, unsigned int n){
	unsigned int i,j;
	bool current, left;
	current = 1;
	j=0;

	//collect lms indexes of T into T1
	for(i=n-1; i>0; i--){
		if(T[i-1] > T[i]) left = 1;
		else if(T[i-1] < T[i]) left = 0;
		else left = current;

		if(!current && left) {T1[n1-1-j] = i; j++;}
		current = left;
	}
	
	//get sorted indexes
	for(j=0; j<n1; j++) SA[j] = T1[SA[j]];
	

	for(i=n1; i<n; i++) SA[i] = 0;

	//put sorted lms suffixes into their bucket	
	for(i=n1-1; i!=EMPTY; i--){ //here empty is used as MAXIMUM 32bit integer
		j = SA[i];
		SA[i] = 0;
		SA[bkt[T[j]]--] = j;
	}
}

void put_lms_suffix_1(unsigned int* T, unsigned int* T1, unsigned int* SA, unsigned int n, unsigned int n1){
	unsigned int i, j;
	bool current, left;
	j=0;
	current = 1;
	for(i=n-1; i>0; i--){
		if(T[i-1] < T[i]) left = 0;
		else if(T[i-1] > T[i]) left = 1;
		else left = current;

		if(!current && left){T1[n1-1-j] = i; j++;}
		current = left;
	}

	for(j=0; j<n1; j++) SA[j] = T1[SA[j]];
	for(j=0;j<n1;j++) T1[j] = EMPTY;
	//for(i=n1; i<n; i++) SA[i] = EMPTY;


	unsigned int next_pos; //this variable tells us in which bkt offset of the lms suffix.
			      //it only works here because we know there are only lms suffixes and they are sorted.
	unsigned int prev = EMPTY;
	unsigned int curr;

	//next_pos always indicates the position that we put element in that iteration into
	for(i=n1-1; i!=EMPTY; i--){
		curr = SA[i];
		SA[i] = EMPTY;
		next_pos = (T[curr] == prev ? (next_pos-1) : T[curr]);
		SA[next_pos] = curr;
		prev = T[curr];
	}

}

void clear_lms_suffix(unsigned int* T1, unsigned int* SA1, unsigned int n1){
	unsigned int i;
	unsigned int j;
	for(i=0; i<n1; i++){
		j = SA1[i];
		if(j<n1-1 && (T1[j]<T1[j+1] || (T1[j] == T1[j+1] && T1[j] >= i))) SA1[i] = EMPTY;
	}
}

void saca_k(unsigned char* T, unsigned int* SA, unsigned int K, unsigned int n, unsigned int elements_left, unsigned char level){
	unsigned int* bkt = NULL;
	//sort all the lms substrings
	if(level == 0){
		//allocate bkt
		bkt = malloc(K * sizeof(unsigned int));
		calc_bkt(bkt, K, T, n, true);
		put_lms_substring_0(T, n, bkt, SA);
		calc_bkt(bkt, K, T, n, false);
		induce_sort_l_0(T, n, SA, bkt, true);
		calc_bkt(bkt, K, T, n, true);
		induce_sort_s_0(T, n, SA, bkt, true);
	}

	else{
		put_lms_substring_1((unsigned int*)T, SA, n);
		induce_sort_l_1((unsigned int*)T, SA, n, true);
		induce_sort_s_1((unsigned int*)T, SA, n, true);
	}
	//collect all the lms substring
	unsigned int n1,i;
	unsigned int temp_store;
	n1=0;
	for(i=0; i<n; i++){
		temp_store = SA[i];
		SA[i] = EMPTY;
		if(temp_store != (level ? EMPTY:0)) SA[n1++] = temp_store;
	}
	
	unsigned int* T1 = SA+elements_left-n1;
	unsigned int* SA1 = SA;
       	unsigned int elements_left1 = elements_left-n1;	
	unsigned int K1 = 0;//dummy value doesn't matter

	if(n1!=0){
		//name_lms_substrings((unsigned char*)T, T1, SA, n, n1, level);
		name_lms_substrings((unsigned char*)T, T1, SA, n, n1, level);

		bool unique = check_names(SA1, n1);
	
		if(unique){
			//directly compute SA1;
			for(unsigned int i=0; i<n1; i++) SA[T1[i]] = i;
		}
		else saca_k((unsigned char*)T1, SA1, K1, n1, elements_left1, level+1);
	}

	if(level == 0){
		calc_bkt(bkt, K, T, n, true);
		put_lms_suffix_0(T, SA, T1, bkt, n1, n);
		calc_bkt(bkt, K, T, n, false);
		induce_sort_l_0(T, n, SA, bkt, false);
		calc_bkt(bkt, K, T, n, true);
		induce_sort_s_0(T, n, SA, bkt, false);
		free(bkt);
	}

	if(level != 0){
		put_lms_suffix_1((unsigned int*)T, T1, SA, n, n1);
		induce_sort_l_1((unsigned int*)T, SA, n, false);
		clear_lms_suffix((unsigned int*)T, SA, n);
		induce_sort_s_1((unsigned int*)T, SA, n, false);
	}
}

