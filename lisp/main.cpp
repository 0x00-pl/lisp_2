#include<iostream>
using namespace std;
#include"core.h"
#include"types.h"
#include"table.h"
#include"static_symbol.h"
#include"lambda.h"
#include"lexical.h"
using namespace pl;

class adder: public base_node{
public:
	base_node* eval(table* args){
		int a,b;
		if(convert_to_date((*args)[1],a) && convert_to_date((*args)[2],b))
			return new warp_class<int>::result(float(a+b));
		return base_node::nil();
	}
	string to_string(){return "+";}
	static adder* ins(){
		static adder _add;
		_add.gcst= gc_lvl::_static;
		return &_add;
	}
};

table* default_env_table(){
	static enverment* tb= nullptr;
	if(tb!=nullptr) return tb;


	tb= new enverment();
	tb->gcst= gc_able::gc_lvl::_static;

	tb->set_value(static_symbol_pool::ins()->create("add"), adder::ins());

	return tb;
}

void main(){
	system("title Debug");
	//vector_map<int,string> tvm;
	////O(1) ≤Â»Î
	//tvm.map_insert(1,"s1");
	//tvm.map_insert(2,"s2");
	//tvm.map_insert(3,"s3");
	//tvm.map_insert(4,"s4");
	//tvm.map_insert(5,"bad5.");
	//tvm.map_insert(5,"bad5..");
	//tvm.map_insert(5,"s5");

	////O(log n) ≤È’“
	//cout<<"find 5: "<<tvm.map_find(5)->second<<endl;

	////O(1) ∑√Œ 
	//for(auto i= tvm.begin(); i!= tvm.end(); ++i){
	//	cout<<i->second<<endl;
	//}
	//system("pause");

	table* tb= default_env_table();
	//(1)  
	{
		base_node* t0= new ntp_number(1);
		cout<<t0->to_string()<<endl;
	}
	//(quote (+ 1 1))
	{
		table* t0= new table();
		t0->push_back(base_node::quote());
		
		table* t01= new table();
		t01->push_back(adder::ins());
		t01->push_back(new ntp_number(1));
		t01->push_back(new ntp_number(1));

		t0->push_back(t01);

		base_node* res= tail_eval_node(t0->expl(tb));
		cout<< res->to_string()<< endl;
	}
	//(+ 1 1)
	{
		table* t0= new table();
		t0->push_back(adder::ins());
		t0->push_back(new ntp_number(1));
		t0->push_back(new ntp_number(1));
		base_node* res= tail_eval_node(t0->expl(tb));
		cout<< res->to_string()<< endl;
	}
	//(+ (+ 1 1) 1)
	{
		table* t0= new table();
		t0->push_back(static_symbol_pool::ins()->create("add"));
		
		table* t01= new table();
		t01->push_back(adder::ins());
		t01->push_back(new ntp_number(1));
		t01->push_back(new ntp_number(1));
		
		t0->push_back(t01);
		t0->push_back(new ntp_number(1));

		base_node* res= tail_eval_node(t0->expl(tb));
		cout<< res->to_string()<< endl;
	}
	//((lambda '(a b) '(+ a b)) 1 1)
	{
		table* t_ab= new table();
		t_ab->push_back(static_symbol_pool::ins()->create("a"));
		t_ab->push_back(static_symbol_pool::ins()->create("b"));
		table* t_q_ab= new table();
		t_q_ab->push_back(base_node::quote());
		t_q_ab->push_back(t_ab);

		table* t_pab= new table();
		t_pab->push_back(static_symbol_pool::ins()->create("add"));
		t_pab->push_back(static_symbol_pool::ins()->create("a"));
		t_pab->push_back(static_symbol_pool::ins()->create("b"));
		table* t_q_pab= new table();
		t_q_pab->push_back(base_node::quote());
		t_q_pab->push_back(t_pab);

		table* t_lmd= new table();
		t_lmd->push_back(make_lambda::ins());
		t_lmd->push_back(t_q_ab);
		t_lmd->push_back(t_q_pab);

		table* t0= new table();
		t0->push_back(t_lmd);
		t0->push_back(new ntp_number(1));
		t0->push_back(new ntp_number(1));


		
		cout<< t0->to_string()<< endl;
		base_node* res= tail_eval_node(t0->expl(tb));
		cout<< res->to_string()<< endl;
	}

	while(1){
		//((lambda (quote (a b)) (quote (add a b))) 1 1)
		string input;
		getline(cin,input);
		cout<<input<<endl;
		node_factory nf;
		node_maker nmk(&nf);

		const char* s_beg= input.c_str();
		base_node* exp= nmk.make(s_beg, s_beg+input.size());
		cout<<exp->to_string()<<endl;
		base_node* res= tail_eval_node(exp->expl(tb));
		cout<<res->to_string()<<endl;
	}

	system("pause");
}