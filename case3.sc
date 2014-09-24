int sum(int a, int b, int c, int d){
	return a+b+c+d;
}
int sum(int a, int b, int c){
	return sum(a,b,c,0);
}
int sum(int a, int b){
	return sum(a,b,0);
}
int sum(int a){
	return sum(a,0);
}
int main(){
 	int a;
	int b;
	int c;
	int d;
	a = 5;
	b = 4;
	c = 3;
	d = 2;
	print sum(a,b,c,d);
	print sum(a,b,c);
	print sum(a,b);
	print sum(a);
	print "marry xmas";
	return 0;
}

