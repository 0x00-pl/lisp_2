#ifndef TYPES_H
#define TYPES_H

#include "tools.h"
#include "core.h" 
namespace pl{

template<typename Tdata_type>
class node_brige: public base_node {
public:
	Tdata_type data;
	operator Tdata_type(){return data;}
};

template<typename T>
struct warp_class{
	typedef node_brige<T> result;
};
template<typename T>
bool convert_to_date(base_node* p, T& dest){
	auto ret= dynamic_cast<warp_class<T>::result*>(p);
	if(ret==nullptr){debug_call_back("bad convert");return false;}
	dest= *ret;
	return true;
}
template<typename T>
bool convert_ptr(base_node* from, T*& dest){
	auto ret= dynamic_cast<T*>(from);
	if(ret==nullptr) return false;
	dest= ret;
	return true;
}

//decl type

class pair_node: public base_node{
public:
	virtual string to_string(){
		string ret;
		ret="[";
		ret+=ar->to_string();
		ret+=", ";
		ret+=dr->to_string();
		ret+="]";
	}
	base_node* ar;
	base_node* dr;
};

class ntp_number: public base_node{
public:
	ntp_number(float n){val=n;}
	float val;
	virtual base_node* eval(base_node* args){return this;}
	virtual string to_string(){
		return ss_itoa<char>(val);
	}
	operator float(){return val;}
	operator int(){return static_cast<int>(val);}
};
template<>
struct warp_class<int>{
	typedef ntp_number result;
};



class ntp_string: public base_node{
public:
	ntp_string(string s){val=s;}
	string val;
	virtual base_node* eval(base_node* args){return this;}
	virtual string to_string(){
		return val;
	}
	operator string(){return val;}
};
template<>
struct warp_class<string>{
	typedef ntp_string result;
};








}
#endif