// g++ -O3 -march=native -funroll-loops -fno-strict-aliasing -o a a.cpp
// g++ -O3 -march=native -funroll-loops -ffast-math -Wl,--stack,16777216 -o a.exe a.cpp
//g++ -O3 -march=native -funroll-loops -fno-strict-aliasing,--stack,33554432 -o a.exe a.cpp

#include <bits/stdc++.h>
#define debug(x) cout<<#x<<" = "<<x<<"\n"
using ll = unsigned long long;
using namespace std;

class int1024{
public:
	ll chunk[2048/32];
	int1024(){
		for(int i = 0; i < 2048/32; i++){
			chunk[i] = 0;
		}
	}
};

struct Montgomery_pack{
	int1024 R;
	int1024 R_;
	int1024 N;
	int1024 M;
};

Montgomery_pack init_Montgomery_algorithm(int1024 N);
int1024 ConvertToMontgomeryForm(int1024 a, Montgomery_pack pack);
int1024 ConvertFromMontgomeryForm(int1024 a, Montgomery_pack pack);
int1024 REDC(int1024 T, Montgomery_pack pack);
int1024 fast_montgomery_exponentation(int1024 a, int1024 b, int1024 mod, Montgomery_pack pack);

void print(int1024 a);

int1024 multiply(int1024 a, int1024 b){
	int1024 c;
	int a_len = -1;
	int b_len = -1;
	for(int i = 2048/32-1; i >= 0; i--){
		if(a.chunk[i] != 0){
			a_len = i;
			break;
		}
	}
	for(int i = 2048/32-1; i >= 0; i--){
		if(b.chunk[i] != 0){
			b_len = i;
			break;
		}
	}
	for(int i  = 0; i < min(2048/32,a_len+b_len+2); i++){
		ll res = 0; 
		for(int j = 0; j <= i; j++){
			if(i+1 < 2048/32) c.chunk[i+1]+=c.chunk[i]>>32;
			c.chunk[i]=c.chunk[i]-((c.chunk[i]>>32)<<32);
			c.chunk[i]+=a.chunk[j]*b.chunk[i-j];
		}
		if(i+1 < 2048/32) c.chunk[i+1]+=c.chunk[i]>>32;
		c.chunk[i]=c.chunk[i]-((c.chunk[i]>>32)<<32);	
	}
	return c;
}

int1024 shift(int1024 a, int s){
	int1024 c;
	for(int i = s; i < 2048/32; i++){
		c.chunk[i] = a.chunk[i-s];
	}
	return c;
}

int1024 right_shift(int1024 a, int s){
	int1024 c;
	for(int  i = 2048/32-s-1; i >= 0; i--){
		c.chunk[i] = a.chunk[i+s];
	}
	return c;
}

int1024 subtract(int1024 a, int1024 b){
	int1024 c;
	for(int i = 0; i < 2048/32; i++){
		if(a.chunk[i]  < b.chunk[i]){
			a.chunk[i]+=(ll(1)<<32);
			int j = i+1;
			while(a.chunk[j] == 0){
				a.chunk[j]+=(ll(1)<<32)-1;
				j++;
			}
			a.chunk[j]--;
		}
		c.chunk[i] = a.chunk[i]-b.chunk[i];
	}
	return c;
}

int1024 add(int1024 a, int1024 b){
	int1024 c;
	for(int i = 0; i < 2048/32; i++){
		c.chunk[i] += a.chunk[i]+b.chunk[i];
		if(c.chunk[i] >= (ll(1)<<32)){
			if(i < 2048/32-1){
				c.chunk[i+1] += (c.chunk[i]/(ll(1)<<32));
			}
			c.chunk[i] %= (ll(1)<<32);
		}
	}
	return c;
}

bool isGreaterOrEqual(int1024 &a, int1024 &b){
	for(int i = 2048/32-1; i >= 0; i--){
		if(a.chunk[i] > b.chunk[i]) return true;
		if(a.chunk[i] < b.chunk[i]) return false;
	}
	return true;
}

int1024 bitshift(int1024 a, ll s){
	int1024 b;
	ll bity = s%32;
	ll czunki = s/32;
	ll carry = 0;
	for(int i = czunki; i < 2048/32; i++){
		b.chunk[i] = (ll(a.chunk[i-czunki])<<ll(bity)) + ll(carry);
		carry = (b.chunk[i]/(ll(1)<<32));
		b.chunk[i]%=(ll(1)<<32);
	}
	return b;
}

int1024 right_bitshift(int1024 a, ll s){
	int1024 b;
	ll bity = s%32;
	ll czunki = s/32;
	int1024 d = bitshift(a,32-bity);
	b = right_shift(bitshift(a,32-bity),czunki+1);
	return b;
}

int1024 fast_divide_by_two(int1024 a){
	int1024 c = a;
	ll carry = 0; 
	ll ocarry = 0;
	for(int i = 2048/32 - 1; i>=0 ;i--){
		carry = a.chunk[i]&ll(1);
		a.chunk[i]>>=1;
		a.chunk[i]|=(ocarry<<31); 
		ocarry = carry;	
	}
	return a;
}

int count_zeroes(int1024 a){
	int i =0;
	while(i < 1024){
		if( (a.chunk[i/32]&(1<<ll(i%32)) ) != 0 ) return i;
		i++;
	}
	return i;
}

int get_bits(int1024 a);

int1024 modulo(int1024 a, int1024 b){
	int a_len = 0;
	int b_len = 0;
	for(int i = 2048-get_bits(b); i>=0; i--){  // do poprawki wkrótce 
		int1024 d = bitshift(b,i);
		if(isGreaterOrEqual(a,d)) {
			a = subtract(a,d);
		}
	}
	return a;
}

struct pair_int1024{
	int1024 fi;
	int1024 se;
};

int get_bits(int1024 a){
	for(int i = 2048-1; i >= 0; i--){
		int czunk = i/32;
		int j = i%32;
		if(a.chunk[czunk]&(ll(1)<<j)) return i+1;
	}
	return 0;
}

pair_int1024 division_with_modulo(int1024 a, int1024 b){ //to też
	int1024 c;
	pair_int1024 res;
	int a_len = 0;
	int b_len = 0;
	for(int i = 2048-get_bits(b); i>=0; i--){
		int1024 d = bitshift(b,i);
		if(isGreaterOrEqual(a,d)) {
			a = subtract(a,d);
			c.chunk[i/32]+=(ll(1)<<(i%32));
		}
	}
	res.fi = c; res.se = a;
	return res;
}

int1024 random_int1024(){
	int1024 a;
	bool ok = true;
	bool ok2 = true;
	for(int i = 1024/32-1; i  >= 0; i-- ){
		for(int j = 31; j>=0; j--){	
				ll r = rand()%2;
				a.chunk[i]+=r*(ll(1)<<j);
		}
	}
	return a;
}

int1024 random_int1024(int1024 n){ 
	int1024 a = modulo(random_int1024(), n);
	return a;
}


int1024 fast_exponentation(int1024 a, int1024 b, int1024 mod){
	int1024 c; c.chunk[0] = 1;
	for(int i = 1024/32 - 1; i >= 0 ; i-- ){
		for(int j = 31; j >= 0; j--){
			int bit = (b.chunk[i]&(1<<j))>>j;
			c = multiply(c,c);
			c = modulo(c,mod);
			if(bit == 1){
				c = multiply(c,a);
				c = modulo(c,mod);
			}
		}	
	}
	return c;
}

bool isEqual(int1024 a, int1024 b){
	if(isGreaterOrEqual(a,b) && isGreaterOrEqual(b,a)) return true;
	return false;
}


bool RabinMiller(int1024 p, int k){
	Montgomery_pack pack = init_Montgomery_algorithm(p);
	if(p.chunk[0]%2 == 0) return false; 
	int1024 c; int1024 jeden; jeden.chunk[0] = 1;
	c = subtract(p,jeden);
	int s= count_zeroes(c);
	int1024 d = right_bitshift(c,s);
	while(k--){
		debug(k);
		int1024 a = random_int1024(c);
		int1024 jeden; jeden.chunk[0] = 1;
		bool ok = true;	
		int1024 res = fast_montgomery_exponentation(a,d,p,pack);
		for(int r = 0; r < s; r++){
			int1024 q = bitshift(d,r);
			res = fast_montgomery_exponentation(a,q,p,pack);
			if(r == 0){
				if(isEqual(res,jeden)) ok = false;
				if(!ok) break;
			}
			if(isEqual(res,c)) ok = false;
			if(!ok) break;
		}
		if(!ok) continue;
		if(ok) return false;
	}
	return true;
}

pair_int1024 Extended_Euclidean_Algorithm(int1024 &a , int1024 &b);

pair_int1024 Binary_Euclidean_Algorithm2(int1024 a , int1024 b);

pair_int1024 Binary_Euclidean_Algorithm(int1024 a , int1024 b){
	return Binary_Euclidean_Algorithm2(a ,  b);
	int1024 zero;
	int1024 x;
	int1024 y;
	pair_int1024 result;
	const ll jed = 1;

	if(isEqual(a,zero) ){
		y.chunk[0]=1;
		result.fi = x;
		result.se = y;
		return result;
	}
	if(isEqual(b,zero)){
		x.chunk[0]=1;
		result.fi = x;
		result.se = y;
		return result;
	}
	if(a.chunk[0]&jed){
		if(b.chunk[0]&jed){
			// nieparzyste a,b
			if(isGreaterOrEqual(a,b)){
				//a>b
				pair_int1024 r = Binary_Euclidean_Algorithm(subtract(a,b),b);
				r.se = add(r.fi,r.se);
				return r;

			}else{
				pair_int1024 r = Binary_Euclidean_Algorithm(a,subtract(b,a));
				r.fi = add(r.fi,r.se);
				return r;
			}
		}else{
			//b parzyste, a nieparzyste
			int1024 pol = fast_divide_by_two(b);
			pair_int1024 r = Binary_Euclidean_Algorithm(a,pol);
			if(r.se.chunk[0]&jed){
				r.fi =add(r.fi,pol);
				r.se =fast_divide_by_two(add(r.se,a));
				return r;
			}else{
				//r.se parzyste
				r.se  = fast_divide_by_two(r.se);
				return r;
			}
		}
	}else{
		if(b.chunk[0]&jed){
			// a parzyste, b nieparzyste
			int1024 pol = fast_divide_by_two(a);
			pair_int1024 r = Binary_Euclidean_Algorithm(pol,b);
			if(r.fi.chunk[0]&jed){
				r.fi =fast_divide_by_two(add(r.fi,b));
				r.se =add(r.se,pol);
				return r;
			}else{
				//r.se parzyste
				r.fi  = fast_divide_by_two(r.fi);
				return r;
			}
		}else{	
			return Binary_Euclidean_Algorithm(fast_divide_by_two(a),fast_divide_by_two(b));
		}
	}
	assert(0==5);
	return result;
}

int1024 tab[5000];
int1024 tab2[5000];


pair_int1024 Binary_Euclidean_Algorithm2(int1024 a , int1024 b){
	int1024 zero;
	int1024 x;
	int1024 y;
	pair_int1024 result;
	const ll jed = 1;
	stack<pair<int,int> > stos;
	vector<int1024> vec;
	int t = 0;
	while(!isEqual(a,zero) && !isEqual(b,zero)){
		if(a.chunk[0]&jed){
			if(b.chunk[0]&jed){
				if(isGreaterOrEqual(a,b)){
					//a>b
					a = subtract(a,b);
					stos.push({1,0});
				}else{
					b = subtract(b,a);
					stos.push({2,0});
				}
			}else{
				b = fast_divide_by_two(b);
				t++; tab[t] = b; tab2[t] = a;
				stos.push({3,t});
			}
		}else{
			if(b.chunk[0]&jed){
				a = fast_divide_by_two(a);
				t++; tab[t] = a; tab2[t] = b;
				stos.push({4,t});
			}else{	
				a = fast_divide_by_two(a);
				b = fast_divide_by_two(b);
				stos.push({5,0});
			}
		}
	}
	if(isEqual(a,zero)) y.chunk[0]=1;
	if(isEqual(b,zero))	x.chunk[0]=1;

	while(!stos.empty()){
		pair<int,int> wierzch = stos.top();
		switch(wierzch.first){
			case 1:
					y = add(x,y);
				break;
			case 2:
					x = add(x,y);
				break;
			case 3:
					if(y.chunk[0]&jed){
						x =add(x,tab[wierzch.second]);
						y =fast_divide_by_two(add(y,tab2[wierzch.second]));
					}else{
						y  = fast_divide_by_two(y);
					}
				break;
			case 4:
					if(x.chunk[0]&jed){
						x = fast_divide_by_two(add(x,tab2[wierzch.second]));
						y = add(y,tab[wierzch.second]);
					}else{
						x  = fast_divide_by_two(x);
					}
				break;
			case 5:
					// nic
				break;
		}
		stos.pop();
	}
	//debug(t);
//	assert(0==5);
	result.fi = x;
	result.se = y;
	return result;
}

pair_int1024 Extended_Euclidean_Algorithm(int1024 &a , int1024 &b){ //musimy znalezc takie x,y > 0, że ax-by=gcd(a,b)
	int1024 zero;
	int1024 x;
	int1024 y;
	pair_int1024 result;
	if(isEqual(a,zero) ){
		y.chunk[0]=1;
		result.fi = x;
		result.se = y;
		return result;
	}
	if(isEqual(b,zero)){
		x.chunk[0]=1;
		result.fi = x;
		result.se = y;
		return result;
	}
	if(isGreaterOrEqual(a,b)){
		pair_int1024 d = division_with_modulo(a,b);
		pair_int1024 r = Extended_Euclidean_Algorithm(d.se,b);
		x = r.fi;
		y = add(r.se,multiply(d.fi,r.fi));
		result.fi = x;
		result.se = y;
		return result;
	}
	if(isGreaterOrEqual(b,a)){
		pair_int1024 d = division_with_modulo(b,a);
		pair_int1024 r = Extended_Euclidean_Algorithm(a,d.se);
		x =  add(r.fi,multiply(d.fi,r.se));
		y = r.se;
		result.fi = x;
		result.se = y;
		return result;
	}
	assert(0==4); //nic nie powinno tu dojść
	return result;
}

void print(int1024 a){
	int1024 dzies; dzies.chunk[0] = 10;
	int1024 zero;
	string s;
	if(isEqual(a,zero)){
		s = "0";
		cout<<s<<"\n";
		return;
	}
	while(!isEqual(a,zero)){
		pair_int1024 d = division_with_modulo(a,dzies);
		a = d.fi;
		s += char(d.se.chunk[0]+'0');
	}
	reverse(s.begin(),s.end());
	cout<<s<<"\n";
}

struct public_key{
	int1024 e;
	int1024 n;
};

struct private_key{
	int1024 d;
	int1024 n;
};

struct key{
	public_key publickey;
	private_key privatekey;
};

key RSA(){
	int1024 Z; Z.chunk[512/32] = 1;
	int1024 p,q;
	p = random_int1024(Z);
	q = random_int1024(Z);
	int1024 jeden; jeden.chunk[0] = 1;
	int1024 dwies; dwies.chunk[0] = 210;
	pair_int1024 dziel;
	dziel = division_with_modulo(p,dwies);
	p = add(multiply(dwies,dziel.fi),jeden);
	dziel = division_with_modulo(q,dwies);
	q = add(multiply(dwies,dziel.fi),jeden);
	while(!RabinMiller(p,10)){
		p = subtract(p,dwies);
	}
	while(!RabinMiller(q,10)){
		q = subtract(q,dwies);
	}
	int1024 n = multiply(p,q);
	int1024 phi = multiply(subtract(p,jeden),subtract(q,jeden));
	int1024 e = random_int1024(phi);
	int1024 d;
	bool ok = false;
	while(!ok){
		pair_int1024 ab = Extended_Euclidean_Algorithm(e,phi);
		int1024 ax = multiply(ab.fi,e);
		int1024 by = multiply(ab.se,phi);
		int1024 diff;
		if(isGreaterOrEqual(ax,by)) diff = subtract(ax,by);
		else diff = subtract(by,ax);
		if(isEqual(diff,jeden)) ok = true;
		d = modulo(ab.fi,phi);
		if(ok){
			if(!isEqual(modulo(multiply(d,e),phi),jeden)){
				d = modulo(multiply(d,subtract(phi,jeden)),phi);
			}
			break;
		}
		e = random_int1024(phi);
	}
	key klucz;
	klucz.privatekey.d = d;
	klucz.privatekey.n = n;
	klucz.publickey.e = e;
	klucz.publickey.n = n;
	return klucz; 
}


string RSA_encode(string s, public_key klucz){
	string res = s;
	
	return res;
}


inline int1024 fast_divide(int1024 A, int b){
	return right_shift(A,b/32);
}

int1024 fast_modulo(int1024 A, int b){
	int1024 C;
	int czunki = b/32;
	for(int i = 0; i < czunki; i++) C.chunk[i] = A.chunk[i];
	return C;
}



Montgomery_pack init_Montgomery_algorithm(int1024 N){
	
	Montgomery_pack pack;
	pack.R.chunk[512/32]=1; //potrzeba więcej przy szyfrowaniu -> 1024, 512 dla Millera-rabina
	pack.N = N;
	
	pair_int1024 para = Binary_Euclidean_Algorithm(pack.R,N);

	int1024 ax = multiply(pack.R,para.fi);
	int1024 by = multiply(N,para.se);

	if(!isGreaterOrEqual(ax,by)){
		para.fi = subtract(N,para.fi);
		para.se = subtract(pack.R,para.se);
	} 
	pack.R_ = para.fi;
	pack.M = para.se;
	return pack;
}

inline int1024 ConvertToMontgomeryForm(int1024 a, Montgomery_pack pack){
	return modulo(multiply(a,pack.R),pack.N);
}

inline int1024 ConvertFromMontgomeryForm(int1024 a, Montgomery_pack pack){
	return modulo(multiply(a,pack.R_),pack.N);
}

int1024 REDC(int1024 T, Montgomery_pack pack){ //N*M = -1 mod R, R > N, gcd(R,N) = 1, T < RN, return S = TR^-1 mod N
	int1024 S;
	int1024 m = fast_modulo(multiply(fast_modulo(T,512),pack.M),512);
	S = fast_divide(add(T,multiply(m,pack.N)),512);
	if(isGreaterOrEqual(S,pack.N)) return subtract(S,pack.N);
	return S;
}


int1024 fast_montgomery_exponentation(int1024 a, int1024 b, int1024 mod, Montgomery_pack pack){
	int1024 c; c.chunk[0] = 1;
	a = ConvertToMontgomeryForm(a,pack);
	c = ConvertToMontgomeryForm(c,pack);
	bool ok = false;
	for(int i = 1024/32 - 1; i >= 0 ; i-- ){
		for(int j = 31; j >= 0; j--){
			int bit = (b.chunk[i]&(1<<j))>>j;
			if(bit) ok =true;
			if(!ok) continue;
			c = multiply(c,c);
			c = REDC(c,pack);
			if(bit == 1){
				c = multiply(c,a);
				c = REDC(c,pack);
			}
		}
		
	}
	c = ConvertFromMontgomeryForm(c,pack);
	return c;
}





int main(){
	srand(time(NULL));	
	int1024 a,b,c,d,e;
	key klucz = RSA();
	string s = "lubie kwiatki";
	print(klucz.privatekey.d);
	print(klucz.publickey.e);
	print(klucz.publickey.n);
	s = RSA_encode(s,klucz.publickey);
	cout<<s<<"\n";

	return 0;
}