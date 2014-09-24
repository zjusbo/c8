int strlen(char& a){
	int i;
	for(i=0; *a[i]!=0; i+=1;)
	    ;	
	return i;
}
void strupr(char& a){
int i;
for(i = 0; i < strlen(a);i+=1;){
    if(*a[i]>=97 && *a[i]<= 122)
	   *a[i] -= 32;
}
	return;
}
void strlow(char& a){
int i;
for(i = 0; i < strlen(a); i+=1;){
    if(*a[i]>=65 && *a[i]<= 90)
	    *a[i] += 32;
}
	return;
}

void strcpy(char& a, char& b){
int i;
for(i=0;i <= strlen(b);i+=1;){
	
	*a[i] = *b[i];
}
for(; i <=strlen(a); i+=1;)
	*a[i] = 0;
	return;
}

void swap(char &a, char &b){
    char temp[50];
    strcpy(&temp,b);
    strcpy(b,a);
    strcpy(a,&temp);
	return;
}

int strstr(char &a){
	int aLen;
	char b;
	read b;
	aLen = strlen(a);
	int i;
	i = 0;
	while(i < aLen){
		if(*a[i] == b)
		{
			return i;
		}
		i+=1;
	}
	return -1;
}
void strcat(char& a, char& b){
	int aLen;
	int bLen;
	aLen = strlen(a);
	bLen = strlen(b);
	int i;
	i = aLen;
	int j;
	j = 0;
	while(1){
		if(*b[j] == 0)
		{
			*a[i] = *b[j];
			break;
		}
		*a[i] = *b[j];
		++i;
		++j;
	}
	return;
}
int strcmp(char &a, char &b){
	int aLen;
	int bLen;
	aLen = strlen(a);
	bLen = strlen(b);
	int i;
	i = 0;
	while(1){
		if(i > aLen)
		{
			return 0;
		}
		if(i > bLen)
		{
			return 0;
		}
		if(*a[i] < *b[i])
		{
			return 1;
		}
		if(*a[i] > *b[i])
		{
			return -1;
		}
		if(*a[i] == *b[i])
		{
			++i;
		}
	}
}

int main(){
	int i;
	char c1[10];
	char c2[10];
	char c3[10];
	c1 = "marry";
	print_ "c1: ";
	strcpy(&c3, &c1);
	print c3;
	c2 = "xmas";
    swap(&c2, &c3);
    print c2;
    strupr(&c2);
    print c2;
    print strcmp(&c2,&c1);
    strupr(&c1);
    print strcmp(&c2,&c1);
    print c2;
    print c1;
    strcat(&c2,&c3);
    print c2;
	print c1;
	i = strstr(&c1);
	print i;
	return 1;
}

	
