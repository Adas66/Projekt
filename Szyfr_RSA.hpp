#ifndef Szyfr_RSA_H
#define Szyfr_RSA_H

#ifndef bits_stdcpp_H
#define bits_stdcpp_H
#include <bits/stdc++.h>
#endif
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


struct public_key{
public:
	int1024 e;
	int1024 n;
};

struct private_key{
public:
	int1024 d;
	int1024 n;
};

class key{
public:
	public_key publickey;
	private_key privatekey;
};




class Szyfr_RSA{
private:
int1024 tab[5000];
int1024 tab2[5000];

class Montgomery_pack{
public:
	int R_wykladnik;
	int1024 R;
	int1024 R_;
	int1024 N;
	int1024 M;
};
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
int1024 modulo(int1024 a, int1024 b){
	int a_len = 0;
	int b_len = 0;
	for(int i = 2048-get_bits(b); i>=0; i--){  
		int1024 d = bitshift(b,i);
		if(isGreaterOrEqual(a,d)) {
			a = subtract(a,d);
		}
	}
	return a;
}
class pair_int1024{
public:
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
pair_int1024 division_with_modulo(int1024 a, int1024 b){ 
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
	Montgomery_pack pack = init_Montgomery_algorithm(p,512);
	if(p.chunk[0]%2 == 0) return false; 
	int1024 c; int1024 jeden; jeden.chunk[0] = 1;
	c = subtract(p,jeden);
	int s= count_zeroes(c);
	int1024 d = right_bitshift(c,s);
	while(k--){
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



pair_int1024 Binary_Euclidean_Algorithm(int1024 a , int1024 b){
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
				break;
		}
		stos.pop();
	}
	result.fi = x;
	result.se = y;
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

inline int1024 fast_divide(int1024 A, int b){
	return right_shift(A,b/32);
}

int1024 fast_modulo(int1024 A, int b){
	int1024 C;
	int czunki = b/32;
	for(int i = 0; i < czunki; i++) C.chunk[i] = A.chunk[i];
	return C;
}

Montgomery_pack init_Montgomery_algorithm(int1024 N, int wykladnik){
	Montgomery_pack pack;
	pack.R_wykladnik = wykladnik;
	pack.R.chunk[wykladnik/32]=1; 
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

int1024 REDC(int1024 T, Montgomery_pack pack){
	int1024 S;
	int1024 m = fast_modulo(multiply(fast_modulo(T,pack.R_wykladnik),pack.M),pack.R_wykladnik);
	S = fast_divide(add(T,multiply(m,pack.N)),pack.R_wykladnik);
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



public:



key RSA(){
	int1024 Z; Z.chunk[512/32] = 1;
	int1024 p,q;
	p = random_int1024(Z);
	q = random_int1024(Z);
	int1024 jeden; jeden.chunk[0] = 1;
	int1024 dwies; dwies.chunk[0] = 510510;
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
		pair_int1024 ab = Binary_Euclidean_Algorithm(e,phi);
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
	string res;
	int dlug = s.size();
	const int czunk_size = 124;
	int czunki = dlug/(czunk_size);
	Montgomery_pack pack = init_Montgomery_algorithm(klucz.n,1024);
	for(int i = 0; i < czunki+1; i++){
		int1024 c;
		for(int j = 0; j < czunk_size; j++){
			if(i*czunk_size+j < s.size()) c.chunk[j/4]+=((ll(s[i*czunk_size+j]))<<(8*(j%4)));
		}
		c = fast_montgomery_exponentation(c,klucz.e,klucz.n,pack);
		for(int j = 0; j < 256; j++){
			res += char(70+((c.chunk[j/8]&(ll(15)<< ((j%8)*4)))>>((j%8)*4))); 
		}
	}
	return res;
}

string RSA_decode(string s, private_key klucz){
	string res;
	int dlug = s.size();
	const int czunk_size = 256;
	int czunki = dlug/(czunk_size); 
	Montgomery_pack pack = init_Montgomery_algorithm(klucz.n,1024);
	for(int i = 0; i < czunki; i++){
		int1024 c;
		for(int j = 0; j < 256; j++){
			c.chunk[j/8] += ((ll(s[i*256+j])-ll(70))<<((j%8)*4));
		}
		
		c = fast_montgomery_exponentation(c,klucz.d,klucz.n,pack);
		for(int j = 0; j < 124; j++){
			res += char(( (c.chunk[j/4])&(  255<<(8*(j%4))  ) )>>(8*(j%4)) ); 
		}
	}
	return res;
}

string print_hex(int1024 a){
	string res;
	for(int i = 0; i < 2048/32; i++){
		for(int j = 0; j < 32; j+=4){
			res += char('A'+((a.chunk[i]&(ll(15)<<j))>>j));
		}
	}
	return res;
}

int1024 read_hex(string a){
	int1024 res;
	for(int i = 0; i < 2048/32; i++){
		for(int j = 0; j < 32; j+=4){
			res.chunk[i] += ll(a[8*i+j/4]-'A')<<j;   
		}
	}
	return res;

}

};


#endif























