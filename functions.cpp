#include "functions.h"
using namespace std;
extern vector<vector<string>> training_data;
extern vector<vector<string>> testing_data;
extern vector<map<string, int>> attribute_value_count;
//read file, load data, get attributes' names
int read_file(fstream &file, vector<string> &attribute_names, int indicator){//indicator determines where the file's data stores.(1: read into training data. 2:read into testing data)
	int total_record = 0;
	string line;

	//read attributes' names
	getline(file,line);
	stringstream ss;
	ss << line;
	string value;
	while(ss >> value){
		attribute_names.push_back(value);
	}
	//read records and store info about how many different values for each attribute in attribute_value_count.
	while(getline(file, line)){
		total_record++;
		vector<string> record;
		stringstream ss;
		ss << line;
		string value;
		int iter = 0;
		while(ss>>value){
			record.push_back(value);		
			//update attribute_value_count
			if(total_record == 1){//the first record
				map<string, int> value_count;
				value_count[value] =1;
				attribute_value_count.push_back(value_count);
			}else{//not the first record	                        
				if(attribute_value_count[iter].find(value)== attribute_value_count[iter].end()){//not found yet
					attribute_value_count[iter][value] = 1;		
				}else{
					attribute_value_count[iter][value]++;
				}
			}
			iter++; 
		}
		//store records
		if(indicator == 1){
			training_data.push_back(record);
		}else{
			testing_data.push_back(record);
		}
	}
	return total_record;
}

//print attributes' names
int print_attribute(vector<string> attribute_names){
	int num =0;
	for(auto i = attribute_names.begin(); i!=attribute_names.end();i++){
		num++;
		cout << num << ". "<< (*i)<<endl;
	}
	return num;
}

//print database 
void print_database(){
	cout << "print database:"<<endl;
	for(auto i = testing_data.begin(); i != testing_data.end(); i++){
		for(auto j = (*i).begin(); j != (*i).end(); j++){
			cout << (*j)<< " ";
		}
		cout << endl;
	}
}
//scan training data to get probabilities of different target value and split database according to different target value
map<string, statistics> get_target_probability(int target_num){
	map<string, statistics> target_statistics; 
	int record_index = 0; 
	for(auto i = training_data.begin(); i!= training_data.end();i++){//scan training dataset
		record_index++;
		if(target_statistics.find((*i)[target_num])== target_statistics.end()){//new target value
			target_statistics[(*i)[target_num]].count = 1;
		}else{
		 	target_statistics[(*i)[target_num]].count++;
		}
		target_statistics[(*i)[target_num]].index.push_back(record_index);//split database
	}
	for(auto &p:target_statistics){
		p.second.probability = log(double(p.second.count)/double(record_index));//set the probability for each target value
	}

	return target_statistics;	
}
//for testing target_statistics
void print_map(map<string, statistics> target_statistics){
	for(auto p: target_statistics){
		cout << p.first <<" database:"<<endl;
		cout << "count:"<< p.second.count<<endl; 
		cout << "index:"<<endl; 
		for(auto j = p.second.index.begin(); j != p.second.index.end(); j++){
			cout << (*j)<<endl;
		}
		cout << "probability: "<<p.second.probability<<endl;
	}
}
//get conditional_probabilities
void get_conditional_probability(map<string,vector<map<string, double>>> &conditional_probability, map<string, statistics> target_statistics, int total_attribute_num){//conditional_probability stores all the statistics on conditional probability
	for(auto p:target_statistics){//for each splitted database
		vector<map<string, double>> conditional_probablity_for_one_db;
		//count for the first record in current splitted database
		auto i = p.second.index.begin();
		for(int j =0; j < total_attribute_num; j++){//scan each attribute
			map<string, double> conditional_probablity_for_one_attribute;
			string value = training_data[(*i)-1][j];
			conditional_probablity_for_one_attribute[value] = 1;
			conditional_probablity_for_one_db.push_back(conditional_probablity_for_one_attribute);	
		}
		if(p.second.index.size()==1){//if only on record in the current splitted database, continue.
			continue;
		}
		//count for the following records
		for(i = i+1; i != p.second.index.end(); i++){//for each record
			for(int j = 0; j < total_attribute_num; j++){
				map<string, double> conditional_probablity_for_one_attribute;
				if(conditional_probablity_for_one_db[j].find(training_data[(*i)-1][j])== conditional_probablity_for_one_db[j].end()){
					conditional_probablity_for_one_db[j][training_data[(*i)-1][j]]=1;
				}else{
					conditional_probablity_for_one_db[j][training_data[(*i)-1][j]]++;	
				}
			}
	
		}
		//get conditional probablity
		int total = p.second.index.size();//records the number of records in current database
		for(auto i = conditional_probablity_for_one_db.begin(); i != conditional_probablity_for_one_db.end(); i++){//for each attribute
			for(auto &k : (*i)){//for each possible value
				k.second= log(double(k.second)/double(total));
			}
		}
		
		conditional_probability[p.first]=conditional_probablity_for_one_db;
	}
}

//for testing conditional probability
void print_conditional_probability(map<string, vector<map<string, double>>> conditional_probability, vector<string> attribute_names){
	for(auto p: conditional_probability){
		cout <<"target_value = "<< p.first<<endl;
		int attribute_no =0;
		for(auto j = p.second.begin(); j != p.second.end(); j++){
			cout <<"attribute "<< attribute_names[attribute_no]<<endl;
			for(auto i:(*j)){
				cout <<i.first<<" "<<i.second<<endl;
			}
			attribute_no++;
		}
	}

}
//predict
void predict(map<string, statistics> target_statistics, fstream &output, map<string,vector<map<string, double>>> conditional_probability, int target_num, vector<string> attribute_names){	
	int right = 0;
	int total = 0;
	for(auto i = testing_data.begin();i != testing_data.end(); i++){//for each record in testing file
		map<string, double> probabilities;//represents the probabilities for all possible values
		for(auto p : conditional_probability){//for each possible target value
			double sum = 0;
			if(get_probability(target_statistics,sum, p.first,i, conditional_probability,target_num,attribute_names)){//calculate the probability for the current target value
				probabilities[p.first] = sum;
			}else{
				cout << "failed to predict due to probability calculation."<<endl;
				continue;
			}
		}
		//get the final result by comparing the probability of different target values
		string result = get_final_target_value(probabilities);

		//check whether the prediction is correct
		if(result == (*i)[target_num]){
			right++;
		}
		//write results to a file
		write_to_file(i,result,output);
		total++;
	}
	//calculate the accuracy
	output <<"accuracy : "<< double(right)/double(total)<<endl;
}

bool get_probability(map<string, statistics> target_statistics,double &sum, string target_value,vector<vector<string>>::iterator record, map<string,vector<map<string, double>>> conditional_probability, int target_num, vector<string> attribute_names){
	for(auto p: conditional_probability){
		if(p.first == target_value){//find the match for the specified target value
			//get conditional probabilities
			auto j =record ->begin();
			int num = 0;
			for(auto i = p.second.begin(); i != p.second.end(); i++){//for each attribute
				if(num != target_num){//not target attribute
					if((*i).find(*j)== (*i).end()){//not found in training dataset
						sum +=log(1/(double(target_statistics[p.first].count)+double(attribute_value_count[num].size())));	
					}else{//found in training dataset
						sum += (*i)[(*j)];
					}
				}
				j++;
				num++;	
			}
		//get probability of target value
		double p = 0;
		for(auto k:target_statistics){
			if(k.first == target_value){
				p = k.second.probability; 	
			}
		}
		sum += p;
		return true;
		}	
	}
	return false;
}
//get final prediction result by comparing the probabilities of different target values.
string get_final_target_value(map<string, double> probabilities){
	double max;
	string result;
	int count = 0;

	//find the target value with maximum probability
	for(auto p: probabilities){
		if(count == 0){//if the first iteration
			max = p.second;//take the first probability as its max
			result = p.first;
		}
		if(max < p.second){
			max = p.second;
			result = p.first;
		}
		count++;
	}
	if(result.length() == 0){
		cout << "failed to predict final target value."<<endl;
	}
	return result;
}
//write prediction to a file
void write_to_file(vector<vector<string>>::iterator record,string result,fstream &output){
	for(auto i = record -> begin(); i != record ->end(); i++){
		output << (*i)<< " ";
	}
	output << result <<endl;
}
//write the attributes title to a file
void write_title(fstream &output, vector<string> attribute_names){
	for(auto i=attribute_names.begin(); i !=  attribute_names.end(); i++){
		output << (*i)<< " ";
	}
	output << "prediction"<<endl;
}
