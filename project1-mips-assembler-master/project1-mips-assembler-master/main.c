#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int* getspacesite(char* str);
int parse_string(char* str, char** input_array);
void print_in_32binary(int input);
void print_in_26binary(int input);
void print_in_16binary(int input);
void print_in_6binary(int input);
void print_in_5binary(int input);
void print_2op(char* inst, char* arg1, char** t_label_names, int* t_label_addr, int t_label_counter);
void print_3op(char* inst, char* arg1, char* arg2); //, int * d_label_line, char** d_label_names, int d_label_counter);
void print_4op(char* inst, char* arg1, char* arg2, char* arg3, char** t_label_names, int* t_label_addr, int t_label_counter, int* addr_list, int j);
int find_addr(char* arg, char** t_label_names, int* t_label_addr, int t_label_counter);
int remove_m(char* arg);
int im_reg_im(char* arg);
int im_reg_im2(char* arg);
int im_reg_reg(char* arg);
int im_reg_reg2(char* arg);
int use_remove(char* arg);
int atoi_0x(char *p);
int is_hexa(char *p);

int main(int argc, char* argv[]){

	if(argc != 2){
		printf("Usage: ./runfile <assembly file>\n"); //Example) ./runfile /sample_input/example1.s
		printf("Example) ./runfile ./sample_input/example1.s\n");
		exit(0);
	}
	else
	{

		// To help you handle the file IO, the deafult code is provided.
		// If we use freopen, we don't need to use fscanf, fprint,..etc. 
		// You can just use scanf or printf function 
		// ** You don't need to modify this part **
		// If you are not famailiar with freopen,  you can see the following reference
		// http://www.cplusplus.com/reference/cstdio/freopen/

		//test set
		// char *buffer;
		// int size;
		// int count;
		//test set end

		// For input file read (sample_input/example*.s)

		char *file=(char *)malloc(strlen(argv[1])+3);
		strncpy(file,argv[1],strlen(argv[1]));

		/* test 2 start*/
		char ar[100][32];
		// char copy_ar[100][32];
		char* ar2[100][4][8];
		for (int j=0;j<100;j++){
			memset(ar[j], 0, 32);
		}
		int j=0;
		FILE *pFile = NULL;
		pFile = fopen(file , "r");
		if (pFile != NULL){
			while (!feof(pFile)){
				fgets(ar[j], sizeof(ar[j]),pFile);
				if (j>95) break;
				j++;
			}
			fclose(pFile);
		}
		else{
			printf("error");
		}
		int endline = j-1;
		// memcpy(copy_ar, ar, sizeof(ar));


		// line parsing3
		//example string set
		char words[][30] = {"I Want toknow","how to find", " the answer	woww	hat?fdw f"};
		char words2[][16]= {"I can do it", "Really? lol"};
		int* test_ar = getspacesite(words[2]);
		int* test_ar2 = getspacesite(words2[0]);

		// //먼저 배열 만들기 (미리 만든 배열을 함수에 매개변수로 넣기)
		// char **parse_array = malloc(sizeof(char *) * 4);
		// for (int i =0; i < 4; i++){
		// 	parse_array[i] = malloc(sizeof(char)*16);
		// }
		// // 함수로 미리 만든 배열에 입력
		// parse_string(words[0], parse_array);
		// // 결과 확인
		// printf("main test/ parse_array[0]: %s, parse_array[1]: %s, parse_array[2]: %s,parse_array[3]: %s \n", parse_array[0],parse_array[1],parse_array[2],parse_array[3]);

		/* test 2 end*/
		/*parsing*/
		//char words_list[endline][4][16];
		char** words_list[endline];
		for (int i=0; i<endline;i++){
			words_list[i] = malloc(sizeof(char *) * 4);
			for (int j=0; j<4; j++){
				words_list[i][j] = malloc(sizeof(char) * 16);
			}
		}


		int n_words_list[endline]; // 각 줄의 단어 개수 저장
		for (int i=0; i<endline; i++){ // n_words_list 초기화
			n_words_list[i] = 0;
		}
		int one_elm_counter = 0; //count the line with only 1 element.
		int label_list[endline]; // list of 1 element line.
		for (int i =0; i<endline; i++){
			n_words_list[i] = parse_string(ar[i], words_list[i]);
			if (n_words_list[i] == 1){
				label_list[one_elm_counter] = i;
				one_elm_counter++;
			}
		}
		/*parsing end*/
		// make data label list
		int size_data = label_list[1] - label_list[0] - 1;  // (in ex1, (5-1)-(1-1)-1 = 3))
		int d_label_counter = 0;
		char first_d_label_name[16];
		char **d_label_names = malloc(sizeof(char *) * size_data); // data label name list
		for (int i=0; i < size_data; i++){
			d_label_names[i] = malloc(sizeof(char)*16);
		}
		//char d_label_names[size_data][16]; 
		int d_label_line[size_data];  // data label line list (address can be calculated directly from ths list)
		if (size_data < 0) printf("data size error");
		for (int i =0; i<= size_data; i++){
			if (n_words_list[i] == 3){
				d_label_line[d_label_counter] = i;
				strcpy(d_label_names[d_label_counter], words_list[i][0]);
				d_label_names[d_label_counter][strlen(words_list[i][0]) -1] = '\0'; //delete the ':' at the end of label name
				d_label_counter++;
				if (d_label_counter == 1){ // store the first data label name
					strcpy(first_d_label_name, words_list[i][0]);
					first_d_label_name[strlen(words_list[i][0]) -1] = '\0'; // delete the ':' at the end of label name
				}
			}
		}
		// make text label list
		int n_t_lines = endline - label_list[1] - 1; // # of lines of text section (in ex1, main~endline: 27)

		int la_counter = 0;
		int tot_la_counter = 0;
		int t_label_counter = 0; //text section label counter

		int t_label_list[n_t_lines][2]; // first element: line of text section label in order. second element: added binary line due to la; 
		char **t_label_names = malloc(sizeof(char *) * n_t_lines);
		for (int i =0; i < n_t_lines; i++){
			t_label_names[i] = malloc(sizeof(char)*16);
		}

		for (int i = label_list[2]; i< endline; i++){
			if (!(strcmp(words_list[i][0], "la"))){
				if (strcmp(words_list[i][2], first_d_label_name)){ // if the loaded data section is not first data label, it cause binary size up
					la_counter++;
					tot_la_counter++;
				}
			}
			if (n_words_list[i] == 1){
				t_label_list[t_label_counter][0] = i;
				if (t_label_counter>0){
					t_label_list[t_label_counter-1][1] = la_counter;
				}
				la_counter = 0;
				strcpy(t_label_names[t_label_counter], words_list[i][0]);
				t_label_names[t_label_counter][strlen(words_list[i][0])-1] = '\0'; // delet ':'
				t_label_counter++; // t_label_counter는 정확한 갯수임(-1 안한 갯수), index는 (정확한 갯수 -1)까지.
			}
		}
		t_label_list[t_label_counter][0] = endline; // limit 설정 (뒤 loop에서 오류 안나도록)

		int t_label_addr[n_t_lines];
		t_label_addr[0] = 0x400000;
		for (int i = 1; i<t_label_counter; i++){
			t_label_addr[i] = t_label_addr[i-1] + 4*(t_label_list[i][0] - t_label_list[i-1][0] -1 + t_label_list[i-1][1]);
		}

		int addr_list[endline];
		for (int i=0; i< endline; i++){ // addr_list 초기화
			addr_list[i] = 0;
		}

		int addr = 0x400000;
		for (int i = label_list[2]; i< endline; i++){
			if (n_words_list[i]>=2){
				addr_list[i] = addr;
				addr += 4;
				if (!(strcmp(words_list[i][0], "la"))){
					if (strcmp(words_list[i][2], first_d_label_name)){ // if the loaded data section is not first data label, it cause binary size up
						addr += 4;
					}
				}
			}
		}

		//size 계산
		int data_section_size_byte = size_data * 4;
		int text_section_size_byte = (n_t_lines - t_label_counter + tot_la_counter)*4;
		//

		if(freopen(file, "r",stdin)==0){
			printf("File open Error!\n");
			exit(1);
		}
	
		//From now on, if you want to read string from input file, you can just use scanf function.


		// For output file write 
		// You can see your code's output in the sample_input/example#.o 
		// So you can check what is the difference between your output and the answer directly if you see that file
		// make test command will compare your output with the answer
		file[strlen(file)-1] ='o';
		freopen(file,"w",stdout);
		//If you use printf from now on, the result will be written to the output file.
		//print in binary start

		//size print
		print_in_32binary(text_section_size_byte);
		print_in_32binary(data_section_size_byte);

		for (int i =0; i< t_label_counter; i++){

			for (int j=(t_label_list[i][0]+1); j<t_label_list[i+1][0]; j++){
				switch(n_words_list[j]){
					case 2:
						print_2op(words_list[j][0], words_list[j][1], t_label_names, t_label_addr, t_label_counter);
						break;
					case 3:
						if (!strcmp(words_list[j][0], "la")){
							int d_addr = 0x10000000;
							int line = 0;
							int reg1 = remove_m(words_list[j][1]);
							for (int k=0; k < d_label_counter; k++){
								if ( !strcmp(words_list[j][2], d_label_names[k])){
									d_addr += 4*(d_label_line[k]-1);
									line = k;
									break;
								}
							}
							int upper = (d_addr/0x10000);
							int lower = d_addr-(d_addr/0x10000)*0x10000;
							char* upper_c = malloc(sizeof(char)*16);
							char* lower_c = malloc(sizeof(char)*16);
							sprintf(upper_c, "%d", upper);
							sprintf(lower_c, "%d", lower);
							print_3op("lui", words_list[j][1], upper_c); //, d_label_line, d_label_names, d_label_counter);
							
							
							if (lower != 0) print_4op("ori", words_list[j][1], words_list[j][1], lower_c, t_label_names, t_label_addr, t_label_counter, addr_list, j);
						}
						else print_3op(words_list[j][0], words_list[j][1], words_list[j][2]); //, d_label_line, d_label_names, d_label_counter);
						break;
					case 4:
						print_4op(words_list[j][0], words_list[j][1], words_list[j][2],words_list[j][3], t_label_names, t_label_addr, t_label_counter, addr_list, j);
						break;
				}
			}
		}

		for (int i=label_list[0]+1; i<label_list[1]; i++){
			int data;
			if (is_hexa(words_list[i][n_words_list[i]-1])) {
				data = atoi_0x(words_list[i][n_words_list[i]-1]);
				
			}
			else {
				
				data = atoi(words_list[i][n_words_list[i]-1]);
			}
			print_in_32binary(data);
		}

		//free words_list
		for (int i=0; i<endline;i++){
			for (int j=0; j<4; j++){
				free(words_list[i][j]);
			}
		}
		for (int i=0; i<endline;i++){
			free(words_list[i]);
		}

	}
	return 0;
}
int is_hexa(char *p){
	char *num = malloc(sizeof(char)*16);
	strcpy(num, p);
	if (num[0]==48){
		if (num[1] == 120){
			return 1;
		}
	}
	return 0;
}

int atoi_0x(char *pp){
	char *p = malloc(sizeof(char)*16);
	strcpy(p, pp+2);
	int num = 0;
	while(*p){
		if (*p >= 'a' && *p <= 'f'){
			num = num * 16 + *p - 'a' + 10;
		} else if (*p >= '0' && *p <= '9'){
			num = num * 16 + *p - '0';
		} else {
			break;
		}
		p++;
	}
	return num;
}

int use_remove(char* arg){
	int num = remove_m(arg);
	printf("use_remove number: %d\n", num);
	return num;
}

int remove_m(char* name){
	char *arg = malloc(sizeof(char)*16);
	char *arg2 = malloc(sizeof(char)*16);
	char *arg3 = malloc(sizeof(char)*16);
	strcpy(arg, name);
	if (arg[0] == 36){
		strcpy(arg2, arg+1);
		if (arg2[(int)strlen(arg2)-1] == 41 || arg2[(int)strlen(arg2)-1] == 44){
			arg2[(int)strlen(arg2)-1] = '\0';
		}
		int num = atoi(arg2);
		return num;
	}

	return 0;
}

int im_reg_im(char* arg){
	char* im_c = malloc(sizeof(char)* 16);
	strcpy(im_c, arg);
	int im=0;
	im_c = strtok(im_c, "(");
	im = atoi(im_c);
	return im;
}



int im_reg_reg(char* arg){
	char* im_c = malloc(sizeof(char)*16);
	strcpy(im_c, arg);
	strtok(im_c, "(");
	im_c = strtok(NULL, "(");
	int im = remove_m(im_c);
	return im;
}

int find_addr(char* arg, char** t_label_names, int* t_label_addr, int t_label_counter){
	int num=-1;
	for (int i=0; i<t_label_counter; i++){
		if (!strcmp(arg, t_label_names[i])){
			num = i;
			break;
		}
	}
	return t_label_addr[num];
}


void print_2op(char* inst, char* arg1, char** t_label_names, int* t_label_addr, int t_label_counter){
	if (!strcmp(inst, "j")){
		print_in_6binary(0x2);
		int addr = find_addr(arg1, t_label_names, t_label_addr, t_label_counter);
		print_in_26binary(addr/4);
	}
	else if (!strcmp(inst, "jal")){
		print_in_6binary(0x3);
		int addr = find_addr(arg1, t_label_names, t_label_addr, t_label_counter);
		print_in_26binary(addr/4);
	}
	else if (!strcmp(inst, "jr")){
		int num = remove_m(arg1);
		print_in_6binary(0);
		print_in_5binary(num);
		print_in_5binary(0);
		print_in_5binary(0);
		print_in_5binary(0);
		print_in_6binary(0x8);
	}
	else printf("2op error");
}
void print_3op(char* inst, char* arg1, char* arg2){ //, int * d_label_line, char** d_label_names, int d_label_counter){
	if (!strcmp(inst, "lui")){
		int num1 = remove_m(arg1);
		int num2;
		if (is_hexa(arg2)) num2 = atoi_0x(arg2);
		else num2 = atoi(arg2);
		print_in_6binary(0xf);
		print_in_5binary(0);
		print_in_5binary(num1);
		print_in_16binary(num2);
	}
	if (!strcmp(inst, "lw")){
		int im = im_reg_im(arg2);
		int reg2 = im_reg_reg(arg2);
		int reg1 = remove_m(arg1);
		print_in_6binary(0x23);
		print_in_5binary(reg2);
		print_in_5binary(reg1);
		print_in_16binary(im);
	}
	if (!strcmp(inst, "sw")){
		int im = im_reg_im(arg2);
		int reg2 = im_reg_reg(arg2);
		int reg1 = remove_m(arg1);
		print_in_6binary(0x2b);
		print_in_5binary(reg2);
		print_in_5binary(reg1);
		print_in_16binary(im);
	}

}
void print_4op(char* inst, char* arg1, char* arg2, char* arg3, char** t_label_names, int* t_label_addr, int t_label_counter, int* addr_list, int j){
	// I format
	if (!strcmp(inst, "addiu")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int im;
		if (is_hexa(arg3)) im = atoi_0x(arg3);
		else im = atoi(arg3);
		print_in_6binary(0x9);
		print_in_5binary(reg2);
		print_in_5binary(reg1);
		print_in_16binary(im);
	}
	if (!strcmp(inst, "andi")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int im;
		if (is_hexa(arg3)) im = atoi_0x(arg3);
		else im = atoi(arg3);
		print_in_6binary(0xc);
		print_in_5binary(reg2);
		print_in_5binary(reg1);
		print_in_16binary(im);
	}

	if (!strcmp(inst, "beq")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int label_addr = find_addr(arg3, t_label_names, t_label_addr, t_label_counter);
		int current_addr = addr_list[j];
		int relative_addr = (label_addr- (current_addr + 4));
		print_in_6binary(0x4);
		print_in_5binary(reg1);
		print_in_5binary(reg2);
		print_in_16binary(relative_addr/4);
	}
	if (!strcmp(inst, "bne")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int label_addr = find_addr(arg3, t_label_names, t_label_addr, t_label_counter);
		int current_addr = addr_list[j];
		int relative_addr = (label_addr- (current_addr + 4));
		print_in_6binary(0x5);
		print_in_5binary(reg1);
		print_in_5binary(reg2);
		print_in_16binary(relative_addr/4);		
	}
	if (!strcmp(inst, "ori")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int im;
		if (is_hexa(arg3)) im = atoi_0x(arg3);
		else im = atoi(arg3);
		print_in_6binary(0xd);
		print_in_5binary(reg2);
		print_in_5binary(reg1);
		print_in_16binary(im);
	}
	if (!strcmp(inst, "sltiu")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int im;
		if (is_hexa(arg3)) im = atoi_0x(arg3);
		else im = atoi(arg3);
		print_in_6binary(0xb);
		print_in_5binary(reg2);
		print_in_5binary(reg1);
		print_in_16binary(im);
	}
	// R format
	if (!strcmp(inst, "and")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int reg3 = remove_m(arg3);
		print_in_6binary(0);
		print_in_5binary(reg2);
		print_in_5binary(reg3);
		print_in_5binary(reg1);
		print_in_5binary(0);
		print_in_6binary(0x24);
	}
	if (!strcmp(inst, "addu")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int reg3 = remove_m(arg3);
		print_in_6binary(0);
		print_in_5binary(reg2);
		print_in_5binary(reg3);
		print_in_5binary(reg1);
		print_in_5binary(0);
		print_in_6binary(0x21);
	}
	if (!strcmp(inst, "nor")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int reg3 = remove_m(arg3);
		print_in_6binary(0);
		print_in_5binary(reg2);
		print_in_5binary(reg3);
		print_in_5binary(reg1);
		print_in_5binary(0);
		print_in_6binary(0x27);
	}
	if (!strcmp(inst, "or")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int reg3 = remove_m(arg3);
		print_in_6binary(0);
		print_in_5binary(reg2);
		print_in_5binary(reg3);
		print_in_5binary(reg1);
		print_in_5binary(0);
		print_in_6binary(0x25);
	}
	if (!strcmp(inst, "subu")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int reg3 = remove_m(arg3);
		print_in_6binary(0);
		print_in_5binary(reg2);
		print_in_5binary(reg3);
		print_in_5binary(reg1);
		print_in_5binary(0);
		print_in_6binary(0x23);
	}
	if (!strcmp(inst, "sltu")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int reg3 = remove_m(arg3);
		print_in_6binary(0);
		print_in_5binary(reg2);
		print_in_5binary(reg3);
		print_in_5binary(reg1);
		print_in_5binary(0);
		print_in_6binary(0x2b);				
	}
	//shift operation
	if (!strcmp(inst, "sll")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int shamt = atoi(arg3);
		print_in_6binary(0);
		print_in_5binary(0);
		print_in_5binary(reg2);
		print_in_5binary(reg1);
		print_in_5binary(shamt);
		print_in_6binary(0);
	}
	if (!strcmp(inst, "srl")){
		int reg1 = remove_m(arg1);
		int reg2 = remove_m(arg2);
		int shamt = atoi(arg3);
		print_in_6binary(0);
		print_in_5binary(0);
		print_in_5binary(reg2);
		print_in_5binary(reg1);
		print_in_5binary(shamt);
		print_in_6binary(2);
	}
	
}


int* getspacesite(char* str){ 
	int *space_array = malloc(sizeof(int)*5);
	for (int i=0; i<5; i++){
		space_array[i] = 99;
	}
	int counter = 0;
	for (int i=0; i<=strlen(str); i++){
		if (isspace(str[i]) != 0){
			space_array[counter] = i;
			counter++;
		}
		if (counter > 5) break;
	}
	if (counter <= 4){
		space_array[counter] = strlen(str);
	}
	return space_array;
}

int parse_string(char* str, char** input_array){
	int* space_array = getspacesite(str); // array of location of empty space
	int size_array[4] = {0,0,0,0}; // array of words(length between the empty space)
	
	int n_words = 4; //initialize the size of size_array
	if (space_array[0] == 0){ // 첫번째 칸 공백
		for (int i=0; i<=3; i++){
			size_array[i] = space_array[i+1] - space_array[i] - 1; // 문제의 코드
			if (size_array[i] == -1 || size_array[i] == 0){
				n_words = i; // if element of size_array[i] = 0, then the string has i words.
				break;
			}
		}
	}
	else if (space_array[0] != 0){  //첫번째 칸 글자  "I Want toknow"
		size_array[0] = space_array[0];
		for (int i=1; i<=3; i++){
			size_array[i] = space_array[i] - space_array[i-1] - 1; // 문제의 코드
			if (size_array[i] > 50 || size_array[i] == 0){
				n_words = i; // if element of size_array[i] = 0, then the string has i words.
				break;
			}
		}
	}
	// parsing using the arrays
	if (space_array[0] != 0){ // string의 첫 글자가 charcter. (공백이 아님)
		if (n_words >= 1){
			strncpy(input_array[0], str, size_array[0]);
			input_array[0][size_array[0]] = '\0';

		}
		else printf("size error");
		for (int i=1; i< n_words; i++){ // i_th word
			strncpy(input_array[i], str+space_array[i-1]+1, size_array[i]);
			input_array[i][size_array[i]] = '\0';
		}
	}
	else if (space_array[0] ==0){ // string의 첫 글자가 공백.
		for (int i=0; i < n_words; i++){
			strncpy(input_array[i], str+space_array[i]+1, size_array[i]);
			input_array[i][size_array[i]] = '\0';
		}
	}

	return n_words;
}

void print_in_32binary(int input){
	for (int i = 31; i >= 0; i--){
		int mask = 1 << i;
		printf("%d", input & mask ? 1 : 0);
	}
}
void print_in_26binary(int input){
	for (int i = 25; i >= 0; i--){
		int mask = 1 << i;
		printf("%d", input & mask ? 1 : 0);
	}
}
void print_in_16binary(int input){
	for (int i = 15; i >= 0; i--){
		int mask = 1 << i;
		printf("%d", input & mask ? 1 : 0);
	}
}
void print_in_6binary(int input){
	for (int i = 5; i >= 0; i--){
		int mask = 1 << i;
		printf("%d", input & mask ? 1 : 0);
	}
}
void print_in_5binary(int input){
	for (int i = 4; i >= 0; i--){
		int mask = 1 << i;
		printf("%d", input & mask ? 1 : 0);
	}
}

int im_reg_im2(char* arg){
	char* im_c = malloc(sizeof(char)* 16);
	strcpy(im_c, arg);
	int im;
	im_c = strtok(arg, "(");
	printf("im_c: %s \n", im_c);
	im = atoi(im_c);
	printf("test im_reg_im: %d\n", im);
	return im;
}

int im_reg_reg2(char* arg){
	char* im_c;
	strcpy(im_c, arg);
	strtok(im_c, "(");
	im_c = strtok(NULL, "(");
	int im;
	if (im_c[0] == '$'){
		char* num;
		if (im_c[strlen(im_c)-1] == ','){
			arg[strlen(im_c)-1] = '\0';
		}
		strcpy(num, im_c+1);
		im = atoi(num);
	}
	else printf("fail \n");
	//remove_m 끝

	return im;
}