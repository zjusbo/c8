void ref(int &b){
	*b = 5;		
	return;
}
void ptr(int &c){
	print *c;
	*c = 20;
	return;
}
int main(){
 	int a;
	a = 10;
	//print &a;
	print a;
	ref(&a);
	print a;
	ptr(&a);
	print a;
	return 0;
}

