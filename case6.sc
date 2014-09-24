int fun(int a){
  if(a == 0 || a == 1)
      return 1;
  else{
      a = fun(a-1) * a;
      return a;    
      }
}
int main(){
  int a;
  int b;
  int c;
  read c;
  for(b = 0;b < c;b+=1;){
     a = fun(b);
     print a;
  }
  return 1;
}

