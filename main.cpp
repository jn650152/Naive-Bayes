#include <stdio.h>
#include "functions.h"
#include <ctime>
using namespace std;
vector<vector<string>> training_data;
vector<vector<string>> testing_data;
vector<map<string, int>> attribute_value_count;//string: attribute name. int: the number of different values for that attribute
int main(){
	const long double time_start = time(0);
	//read testing and training files' names
	cout << "Please enter a training file: "<<endl;
	string training_file_name;
	cin >> training_file_name;
	cout << "Please enter a testing file: "<<endl;
	string testing_file_name;
	cin >> testing_file_name;

	//check whether training file and testing file exist
	fstream training_file;
	training_file.open(training_file_name);
	if(!training_file.good()){//check whether the file exists
		cout << "not found training file"<<endl;
		return 0;
	}
	fstream test_file;
	test_file.open(testing_file_name);
	if(!test_file.good()){//check whether the file exists
		cout << "not found testing file"<<endl;
		return 0;
	}
	
	//read data from the training file
	vector<string> attribute_names;
	int total_training_record = read_file(training_file, attribute_names,1);

	//choose target attribute
	cout << "Please choose an attribute (by number):"<<endl;
	int total_attribute_num = print_attribute(attribute_names);
	int target_num = 0;
	cout << "your choice:"<<endl;
	cin >> target_num;
	if(target_num >total_attribute_num||target_num <= 0){
		cout << " the choice is not valid"<<endl;
		return 0;
	}
	target_num--;
	
	//print_database();
	

	cout << "before trainging"<<endl;
	//training
	//scan training data to get probabilities of different target value
	map<string, statistics> target_statistics = get_target_probability(target_num);
	//print_map(target_statistics);

	//get conditional_probabilities
	map<string, vector<map<string, double>>> conditional_probability;
	get_conditional_probability(conditional_probability,target_statistics, total_attribute_num);
	//print_conditional_probability(conditional_probability,attribute_names);
	//testing
	//read test file
	vector<string> attribute_names_2;
	int total_testing_record = read_file(test_file,attribute_names_2,2);

	//predict testing data and save results in a file
	fstream output;
	output.open("Results");
	//write titles to output
	write_title(output, attribute_names_2);
	//predict and write results to ouput 	
	predict(target_statistics,output,conditional_probability,target_num, attribute_names_2);
	
	//close file
	training_file.close();	
	test_file.close();
	output.close();

	const long double time_end = time(0);
	cout << "start: "<< time_start<<endl;
	cout << "end: "<< time_end<<endl;
	cout << "time taken: "<<1000*(time_end - time_start)<<"milliseco"	<<endl;
	return 0;


}
