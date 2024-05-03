#ifndef STATIC_SYMBOL_H
#define STATIC_SYMBOL_H
#include<map>
#include<string>
using namespace std;
#include"core.h"
#include"table.h"
#include"value.h"
#include"enverment.h"

namespace pl{
class symbol;
class static_symbol_pool{
private:
	static_symbol_pool(){}
	//下面这一段都是实现 单件 的
	static static_symbol_pool* m_ins;
	typedef static_symbol_pool* (static_symbol_pool::f_get_ins_type)();
	static f_get_ins_type* _impl_get_ins;
	static static_symbol_pool* _get_ins_1st(){
		_impl_get_ins= &_get_ins;
		m_ins= new static_symbol_pool();
		return m_ins;
	}
	static static_symbol_pool* _get_ins(){
		return m_ins;
	}
	//(实现 单件) 到此为止
public:
	static static_symbol_pool* ins(){
		return (*_impl_get_ins)();
	}

	symbol* create(string name);

	map<string,symbol*> name_to_symbol;
	map<symbol*,string> symbol_to_name;
};
static_symbol_pool* static_symbol_pool::m_ins= nullptr;
static_symbol_pool::f_get_ins_type* static_symbol_pool::_impl_get_ins= &static_symbol_pool::_get_ins_1st;


//符号类
class symbol: public base_node{
	friend static_symbol_pool;
	symbol(){
		gcst= gc_lvl::_static;
	}
public:
	virtual base_node* eval(table* args){return nil();}
	virtual base_node* expl(table* env){
		//env must be class: enverment
		enverment* tenv= static_cast<enverment*>(env);
		base_node* ret= nil();
		tenv->get_value(this, ret);
		return ret;
	}
	virtual string to_string(){
		return static_symbol_pool::ins()->symbol_to_name[this];
	}
	static symbol* self(){
		return static_symbol_pool::ins()->create("self");
	}
};
symbol* static_symbol_pool::create(string name){
	auto i_ns= name_to_symbol.find(name);
	if(i_ns!= name_to_symbol.end()){
		return i_ns->second;
	}

	auto s= new symbol();

	name_to_symbol[name]= s;
	symbol_to_name[s]= name;
	return s;
}

//统计符号
void marge_symbol(table* from, enverment* dest){
	for(size_t i=0; i< from->args.size(); ++i){
		symbol* s;
		table* t;
		if(convert_ptr((*from)[i], s)){
			dest->add_value(s);
		}else if(convert_ptr((*from)[i], t)){
			marge_symbol(t, dest);
		}
	}
}

}
#endif