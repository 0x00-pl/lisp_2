#ifndef CORE_H
#define CORE_H
#include<string>
#include<vector>
using namespace std;

namespace pl{

void default_debug_call_back(char* msg){
}
void (*debug_call_back)(char* msg)= &default_debug_call_back;

class gc_able{
public:
	struct gc_lvl{
		typedef char value_type;
		static const value_type _death= 0;
		static const value_type _live= 1;
		static const value_type _creating= 2;
		static const value_type _static= 3;
		static value_type created(value_type from){if(from!=_static)return _live; return from;}
	};

	gc_able(){
		pool.push_back(this);
		creating_pool.push_back(this);
		gc_notify(gc_lvl::_creating);
	}
	static void gc_update_creating_pool(){
		vector<gc_able*> temp(creating_pool.size());
		for(size_t i=0; i<creating_pool.size(); ++i){
			if(creating_pool[i]->gcst >= gc_lvl::_creating)
				temp.push_back(creating_pool[i]);
		}
		creating_pool= move(temp);
	}
	static void gc_update_pool(){
		vector<gc_able*> temp(pool.size());
		for(size_t i=0; i<pool.size(); ++i){
			if(pool[i]->gcst == gc_lvl::_death){
				delete pool[i];
			}else{
				temp.push_back(pool[i]);
			}
		}
		creating_pool= move(temp);
	}
	static void gc_collect(){
		gc_update_creating_pool();
		for(size_t i=0; i<pool.size(); ++i){
			pool[i]->gc_notify(gc_lvl::_death);
		}
		for(size_t i=0; i<creating_pool.size(); ++i){
			creating_pool[i]->gc_notify(gc_lvl::_live);
		}
		gc_update_pool();

	}
	virtual~gc_able(){}
	virtual void gc_notify(gc_lvl::value_type flag){
		gcst= max(gcst,flag);
	}
	void attached(){
		if(gcst!= gc_lvl::_static)
			gcst= gc_lvl::_live;
	}

	gc_lvl::value_type gcst;
	static vector<gc_able*> pool;
	static vector<gc_able*> creating_pool;
	static int sum_of_nodes;
};
vector<gc_able*> gc_able::pool;
vector<gc_able*> gc_able::creating_pool;
int gc_able::sum_of_nodes=0;
class table;
class base_node: public gc_able{
public:
	base_node(){}
	base_node(gc_lvl::value_type _gc_state){gcst= _gc_state;}
	virtual base_node* eval(table* args){return nil();}
	virtual base_node* expl(table* _env){return this;}
	virtual string to_string(){return base_node_name_map(this);}
	static string base_node_name_map(base_node* n){
		if(n==nil()){
			return "nil" ;
		} else if(n==tail()){
			return "tail" ;
		} else if(n==quote()){
			return "quote" ;
		}else{
			return "T?base_node";
		}
	}
	static base_node* nil(){
		static base_node _nil(gc_lvl::_static);
		return &_nil;
	}
	static base_node* tail(){
		static base_node _tail;
		_tail.gcst= gc_lvl::_static;
		return &_tail;
	}
	static base_node* quote(){
		static base_node _quote;
		_quote.gcst= gc_lvl::_static;
		return &_quote;
	}
	static base_node* cond(){
		static base_node _cond;
		_cond.gcst= gc_lvl::_static;
		return &_cond;
	}
};

bool node_eq(base_node* a, base_node* b){
	return a==b;
}

}
#endif