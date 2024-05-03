#ifndef VALUE_H
#define VALUE_H

#include"core.h"
#include"types.h"

namespace pl{

class value_address: public base_node{
public:
	value_address(){
		data= nil();
	}
	virtual void gc_notify(gc_lvl::value_type flag){
		gcst= max(gcst, flag);
		data->gc_notify(flag);
	}
	virtual base_node* expl(table* env){return data;}
	virtual string to_string(){
		string ret="&";
		ret+= data->to_string();
		return ret;
	}
	static value_address* nil_address(){
		static value_address ret;
		return &ret;
	}
	base_node* data;
}; 

//make
value_address* make_value(){
	return nullptr;
}
//get
bool value_get(value_address* n, base_node*& dest){
	 dest= n->data;
	 return true;
}
//set
bool value_set(value_address* n, base_node* d){
	 n->data= d;
	 return true;
}


}
#endif