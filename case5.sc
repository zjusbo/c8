//Function override and recursive
int fun(int a, int b){
 	 print a;
	print b;
	return 1;
}
int fun(int c){
	print c;
	return fun(12, 13);
}
int main(){
	int rr;
	rr = 15;
	rr = fun(rr);
	int a;
	int b;
	a = 5;
	b = 6;
	rr = fun(a, b);
	for(b=0; b<=5;b++;)
		print b;
	return 0;
}

