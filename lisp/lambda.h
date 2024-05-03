#ifndef LAMBDA_H
#define LAMBDA_H

#include"table.h"
#include"types.h"
#include"static_symbol.h"
#include"enverment.h"

namespace pl{


class lambda: public base_node{
public:
	lambda(table* _bloc, table* _args, enverment* _env){
		arg_size= _args->args.size();
		bloc=_bloc;
		f_env= new enverment();
		f_env->set_value(symbol::self(), this);
		//先放置参数 按顺序
		for(size_t i=0; i<_args->args.size(); ++i){
			f_env->set_value(_args->args[i], nil());
		}
		marge_symbol(_bloc, f_env);

		//link
		for(auto i= f_env->nameing.begin(); i!= f_env->nameing.end(); ++i){
			base_node* temp;
			if(_env->get_value(i->first, temp)){
				f_env->set_value(i->first, temp);
			}
		}
	}
	virtual base_node* eval(table* _args){
		for(size_t i= 1; i<arg_size+1; ++i){
			base_node* temp= (*_args)[i];
			//self at [0]
			f_env->set_value(i, temp);
		}
		return bloc->expl(f_env);
	}
	virtual string to_string(){
		string ret;
		ret="<";
		ret+="lambda ";
		ret+=bloc->to_string();
		ret+=" as ";
		ret+=f_env->to_string_name_only();
		ret+=">";
		return ret;
	}
	size_t arg_size;
	table* bloc;
	enverment* f_env;
};

class make_lambda: public base_node{
public:
	make_lambda(gc_lvl::value_type tp):base_node(tp){}
	virtual base_node* eval(table* args){
		table* lambda_args;
		table* lambda_bloc;
		if(!convert_ptr((*args)[1], lambda_args)) return nil();
		if(!convert_ptr((*args)[2], lambda_bloc)) return nil();

		lambda* ret= new lambda(lambda_bloc, lambda_args, _last_env);
		return ret;
	}
	virtual base_node* expl(table* _env){
		if(!convert_ptr(_env, _last_env)) debug_call_back("bad convert: lambda.h > make_lambda > expl()");
		return this;
	}
	virtual string to_string(){return "lambda";}

	static make_lambda* ins(){
		static make_lambda _make_lambda(gc_lvl::_static);
		return &_make_lambda;
	}
	enverment* _last_env;
};


}
#endif