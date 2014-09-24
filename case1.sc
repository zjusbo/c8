int m;
void funa(){
	print m;
	char str[32];
	str = "m += 1";
	print str;
	m += 1;
	print m;
	return;
}
void funb(){
	print m;
	char str[32];
	str = "m += 5";
	print str;
	m += 5;
	print m;
	return;
}
void func(){
	print m;
	char str[32];
	str = "m -= 3";
	print str;
	m -= 3;
	print m;
	return;
}
int main(){
 	 m = 17;
	char str[32];
	str = "m intialization";
	print str;
	print m;
	funa();
	funb();
	func();
	return 0;
}

