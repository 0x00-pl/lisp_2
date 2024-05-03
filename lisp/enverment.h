#ifndef ENVERMENT_H
#define ENVERMENT_H

#include"table.h"
#include"types.h"

namespace pl{


class enverment: public table{
public:
	void add_value(base_node* _symbol_name){
		insert(_symbol_name, new value_address());
	}
	bool get_value(base_node* _symbol_name, base_node*& dest){
		value_address* tmp;
		if(!convert_ptr((*this)[_symbol_name], tmp)) {
			dest= nil();
			return false;
		}

		return value_get(tmp, dest);
	}
	void set_value(size_t _symbol_index, base_node* _value){
		value_address* tmp;
		//查找是否有
		if(_symbol_index >= args.size()){
			tmp= new value_address;
			value_set(tmp, _value);
			insert(_symbol_index, tmp);
			tmp->attached();
		}else{
			if(!convert_ptr(args[_symbol_index], tmp)) debug_call_back("enverment.h set_value() type err");
			value_set(tmp, _value);
		}
	}
	void set_value(base_node* _symbol_name, base_node* _value){
		value_address* tmp;
		//查找是否有
		auto iter= nameing.find(_symbol_name);
		if(iter==nameing.end()){
			tmp= new value_address;
			value_set(tmp, _value);
			insert(_symbol_name, tmp);
			tmp->attached();
		}else{
			if(!convert_ptr(args[iter->second], tmp)) debug_call_back("enverment.h set_value() type err");
			value_set(tmp, _value);
		}
	}
};

}
#endif