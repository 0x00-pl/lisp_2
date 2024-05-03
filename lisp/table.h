#ifndef TABLE_H
#define TABLE_H

#include<vector>
#include<map>
using namespace std;

#include"core.h"
#include"tools.h"
#include"value.h"

namespace pl{
base_node* tail_eval_node(base_node* exp);
void make_tail(table*& exp);
//表 集合
class table:public base_node{
public:
	virtual void gc_notify(gc_lvl::value_type flag){
		gcst= max(gcst, flag);
		for(auto i=args.begin(); i!=args.end(); ++i){
			(*i)->gc_notify(flag);
		}
	}
	string to_string_name_only(){
		string ret="<";
		for(auto i= nameing.begin(); i!= nameing.end(); ++i){
			ret+= i->first->to_string();
			ret+= " ";
		}
		ret+=">";
		return ret;
	}
	virtual string to_string(){
		string ret;
		ret="[";
		if(nameing.empty()){
			//[a b c]
			for(size_t i=0; i<args.size()-1; ++i){
				ret+= args[i]->to_string();
				ret+= " ";
			}
			ret+= args[args.size()-1]->to_string();
		}else{
			//[arg1:1 arg2:2 3:3]
			vector<string> names(args.size());
			//fill
			for(size_t i= 0; i<args.size(); ++i){
				names[i]= pl::ss_itoa<char>(i);
			}
			for(auto i= nameing.begin(); i!= nameing.end(); ++i){
				names[i->second]=i->first->to_string();
			}
			//push
			for(size_t i= 0; i< args.size()-1; ++i){
				ret+= names[i];
				ret+= ":";
				ret+= args[i]->to_string();
				ret+= " ";
			}
			ret+= names[args.size()-1];
			ret+= ":";
			ret+= args[args.size()-1]->to_string();
		}
		ret+="]";
		return ret;
	}
	virtual base_node* eval(table* n_index){
		//返回第n个元素
		int index;
		if(convert_to_date((*n_index)[1], index))
			return (*this)[index];
		return (*this)[n_index];
	}
	virtual base_node* expl(table* env){
		//if quote
		if(args[0]==quote()){
			base_node* temp= args[1];
			return temp;
		}
		//if cond
		if(args[0]==cond()){
			for(size_t i=1; i<args.size(); i+=2){
				if(tail_eval_node(args[i]->expl(env))!=nil()){
					return tail_eval_node(args[i+1]->expl(env));
				}
			}
			return nil();
		}
		//递归解释
		table* ret= new table();
		for(size_t i=0; i<args.size(); ++i){
			ret->push_back(tail_eval_node((*this)[i]->expl(env)));
		}

		make_tail(ret);
		return ret;
	}
	bool push_back(base_node* n){
		n->attached();
		args.push_back(n);
		return true;
	}
	void pop_back(){
		args.pop_back();
	}
	bool insert(base_node* name, base_node* n){
		n->attached();
		auto i= nameing.find(name);
		if(i!=nameing.end()){
			args[i->second]= n;
			return true;
		}
		nameing[name]= args.size();
		args.push_back(n);
		return true;
	}
	bool insert(size_t index, base_node* n){
		n->attached();
		args.insert(args.begin()+index, n);
		//for_each  fix nameing_map
		for(auto i= nameing.begin(); i!= nameing.end();++i){
			if(i->second >= index){
				++i->second;
			}
		}
		return true;
	}
	bool erase(size_t index){
		if(index >= int(args.size())) return false;
		args.erase(args.begin()+index);
		//for_each fix nameing_map
		for(auto i= nameing.begin(); i!= nameing.end();){
			if(i->second == index){
				i= nameing.erase(i);
				continue;
			}else if(i->second > index){
				--i->second;
			}
			++i;
		}
		return true;
	}
	bool erase(base_node* name){
		auto i= nameing.find(name);
		if(i== nameing.end()) return false;
		int index= i->second;
		return erase(index);
	}
	
	//此函数不安全
	base_node* operator[](size_t index){
		return args[index];
	}
	//此函数不安全
	base_node* operator[](base_node* symbol){
		auto i= nameing.find(symbol);
		if(i!=nameing.end()){
			return args[i->second];
		}
		return base_node::nil();
	}


	vector<base_node*> args;
	map<base_node*, size_t> nameing;
};

//尾递归相关
void make_tail(base_node*& exp){
	table* temp_exp;
	if(!convert_ptr(exp, temp_exp)) return;
	make_tail(temp_exp);// mark tail
	exp= temp_exp;
}
void make_tail(table*& exp){
	exp->args.push_back(base_node::tail());// mark tail
}
bool off_tail(base_node*& exp){
	table* temp_exp;
	if(!convert_ptr(exp, temp_exp)) return false;
	//check tail
	if(!node_eq(temp_exp->args.back(),base_node::tail())){
		//if not tail
		return false;
	}
	temp_exp->pop_back();
	exp= temp_exp;
	return true;
}
base_node* tail_eval_node(base_node* exp){
	table* tb;
	if(!convert_ptr(exp,tb)){
		return exp;
	}

	base_node* ret= tb;

	while(off_tail(ret)){
		tb= dynamic_cast<table*>(ret);
		ret= (*tb)[size_t(0)]->eval(tb);
		tb->gcst= gc_able::gc_lvl::created(tb->gcst);
	}

	return ret;
}


//lambda相关

}
#endif