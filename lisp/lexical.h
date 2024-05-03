#ifndef LEXICAL_H
#define LEXICAL_H
#include<string>
#include"table.h"
#include"static_symbol.h"
#include"types.h"
using namespace std;

namespace pl{


class node_factory{
public:
	virtual base_node* make_number(string s){
		return new ntp_number(atof(s.c_str()));
	}
	virtual base_node* make_symbol(string s){
		if(s=="quote") return base_node::quote();
		else if(s=="nil") return base_node::nil();
		else if(s=="tail") return base_node::tail();
		else if(s=="cond") return base_node::cond();
		else if(s=="lambda") return make_lambda::ins();
		
		return static_symbol_pool::ins()->create(s);
	}
	virtual base_node* make_string(string s){
		return new ntp_string(s);
	}
	virtual table* make_table(){return new table();}
};

class node_maker{
public:
	node_maker(node_factory* fac){
		factor= fac;
	}
	const char* token_end(const char * iter){
		while(*iter!=' '&&
			*iter!=')'&&
			*iter!='\r'&&
			*iter!='\n'&&
			*iter!='\t'&&
			*iter!='('){
				++iter;
		}
		return iter;
	}
	const char* space_end(const char* iter){
		while(*iter==' '||
			*iter=='\r'||
			*iter=='\n'||
			*iter=='\t'){
				++iter;
		}
		return iter;
	}
	const char* string_end(const char* iter){
		if(*iter!='\"') return iter;
		++iter;//jump '\"'
		while(iter[0]!='\"' && iter[-1]!='\\'){
			++iter;
		}
		return iter;
	}
	base_node* make(const char*& iter, const char* end){
		switch(*iter){
			//string
		case '\"':
			{
				const char* str_end= string_end(iter);
				base_node* ret= factor->make_string(string(iter, str_end));
				iter= space_end(++str_end);//jump '\"' jump space
				return ret;
			}
			//list
		case '(':
			{
				table* tb= factor->make_table();
				++iter;
				while(*iter!=')'){
					tb->push_back(make(iter,end));
				}
				++iter;
				iter= space_end(iter);
				return tb;
			}
			break;
			//number
		case '+':case '-':
			if(iter[1]>='0' && iter[1]<='9'){
			}else{
				//not number
				base_node* ret= factor->make_symbol(string(iter,iter+1));
				++iter;
				iter= space_end(iter);
				return ret;
			}
		case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
			{
				const char* itend= token_end(iter);
				base_node* ret= factor->make_number(string(iter,itend));
				iter= space_end(itend);
				return ret;
			}
			//symbol
		default:
			{
				const char* itend= token_end(iter);
				base_node* ret= factor->make_symbol(string(iter,itend));
				iter= space_end(itend);
				return ret;
			}
		}
	}
	node_factory* factor;
};

}
#endif