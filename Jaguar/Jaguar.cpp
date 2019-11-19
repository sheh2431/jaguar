#include <iostream>
#include <ctime>
#include <cmath>
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

struct Jaguar{
	float position;
	double fitness;
	int direction;
	float step;
};

float domain_upper;
float domain_lower;

int calculate_times = 0;
double best = -99999;
Jaguar JA_best;
float init_step;
bool overflow = false;

Jaguar seek_around(Jaguar, float);
//fstream file;
ofstream file;
string output = "answer.csv";

void write_csv(){
	file.open(output.c_str());
}

double fitness(float pos){
	calculate_times++;
	return fabs(pos);
}

float initial(const float upper, const float lower){
	domain_upper = upper;
	domain_lower = lower;
	/* ---- initial step ---- */
	int power = floor(log(domain_upper)/log(2.0)) -11;
	return pow(2, power);
}

void update_best(Jaguar best){
	if(best.fitness < JA_best.fitness){
		JA_best.position = best.position;
		JA_best.fitness = best.fitness;
		JA_best.direction = best.direction;
	}
	file << "JA_best" << "," << JA_best.position << "," << JA_best.fitness << endl;
}

bool cmp_fitness(Jaguar now, Jaguar cmp){
	bool better = false;
	if(now.fitness < JA_best.fitness){
		better = true;
	}
	return better;
}

Jaguar random_pos(){
	Jaguar init;
	float rn = rand();
	//cout << "RANDOM: " << rn << endl;
	//cout << "RAND_MAX: " << RAND_MAX << endl;
	float pos=(rn/(float)RAND_MAX) * (domain_upper - domain_lower) + domain_lower;
	init.position = pos;
	init.fitness = fitness(pos);
	file << "POS" << "," << init.position << "," << "fitness" << "," << init.fitness << "," << "RAND" << "," << rn << endl;
	update_best(init);
	return init;
}

int check_fitness(Jaguar r_f, Jaguar l_f){
	bool better = false;
	Jaguar best;
	int direction = 0;
	if(r_f.fitness < l_f.fitness){
		//better = cmp_fitness(r_f, JA);
		//if(better){
			best = r_f;
			direction = 1;
		//}
	}
	else if(r_f.fitness > l_f.fitness){
		//better = cmp_fitness(l_f, JA);
		//if(better){
			best = l_f;
			direction = -1;
		//}
	}
	else if(r_f.fitness == l_f.fitness){
		//better = cmp_fitness(r_f, JA);
		//if(better){
			best = r_f;
			direction = 1;
		//}
	}
	
	return direction;
}


Jaguar slow_down(Jaguar current){
	file<<  "===== Slow Down =====" << endl;
	Jaguar next, last;
	int direction = current.direction;
	float step = current.step;
	file << "Direction" << "," << direction << endl;
	file << "Current" << "," << current.position << "," << current.fitness << endl;
	last = current;
	step /= 2;
	next.position = last.position + (step * direction);
	if(next.position >= domain_lower && next.position <= domain_upper){
		next.fitness = fitness(next.position);
		file << calculate_times << "," << next.position << "," << next.fitness << ",";
		file << "STEP" << "," << step << endl ;
		if(next.fitness > last.fitness) next = last;
		update_best(next);
	}
	else{
		file << "OUT OF RANGE" << endl;
		file << "STEP" << "," << step << "," <<  "Position: " << "," << next.position << endl;
		next = last;
	}
	
	do{
		step /= 2;
		next = seek_around(next, step);
		update_best(next);
		if(overflow) break;
		file << "NEXT_DIRECTION" << "," << next.direction << endl;
	}while(init_step != step);
	file << "current step" << "," << step << endl;
	file << "========== END of SLOW DOWN =========" << endl;
	
	return next;
}

Jaguar speed_up(Jaguar current, float step){
	Jaguar next, last;
	int direction = current.direction;
	file <<  "===== Speed UP =====" << endl;
	file << "Direction" << "," << direction << endl;
	file << "Current" << "," << current.position << "," << current.fitness << endl;
	bool loop = false;
	do{
		if(loop == false) last = current;
		else last = next;
		step *= 2;
		next.position = last.position + (step * direction);
		if(next.position >= domain_lower && next.position <= domain_upper){
			next.fitness = fitness(next.position);
			file << calculate_times << "," << next.position << "," << next.fitness << ",";
			file << "STEP" << "," << step << endl;
			update_best(next);
			loop = true;
		}
		else{
			file << "OUT OF RANGE" << endl;
			file << "STEP" << "," << step << "," <<  "Position" << "," << next.position << endl;
			next = last;
			break;
		}
	}while(next.fitness <= last.fitness);
	next.fitness = JA_best.fitness;
	next.direction = direction;
	next.step = step;
	
	return next;
}

bool check_pos(float current, float right, float left){
	bool distinguish = true;
	if(current == right && current == left)
		distinguish = false;
	return distinguish;
}

Jaguar seek_around(Jaguar current, float step){
	float pos = current.position;

	file << "===== SEEK AROUND =====" << endl;
	file << "NOW" << "," << current.position << "," <<current.fitness << ",";
	cout << "NOW" << "," << fixed << setprecision(20) << current.position << "," <<current.fitness << ",";
	cout << endl;
	file << "STEP" << "," << step << endl;
	Jaguar right, left, better_dir;
	right.position = pos + step;
	left.position = pos - step;
	
	if(check_pos(pos, right.position, left.position)==false){
		cout << "OVERFLOW" << endl;
		overflow = true;
		return current;
	}
	if(right.position >= domain_lower && right.position <= domain_upper){
		right.fitness = fitness(right.position);
		file << calculate_times << "," << "RIGHT" << "," <<right.position << "," << right.fitness << endl;
		cout << calculate_times << "," << "RIGHT" << "," <<right.position << "," << right.fitness << endl;
	}
	else right.fitness = 9999;
	
	if(left.position >= domain_lower && left.position <= domain_upper){
		left.fitness = fitness(left.position);
		file << calculate_times << "," << "LEFT" << "," << left.position << "," << left.fitness << endl;
		cout << calculate_times << "," << "LEFT" << "," <<left.position << "," << left.fitness << endl;
	}
	else right.fitness = 9999;
	
	int better;
	better = check_fitness(right, left);

	if(better == 1){
		//update_best(right);
		better_dir = right;
		better_dir.direction = 1;
		//speed_up(current, step, 1);
	}
	else if(better == -1){
		//update_best(left);
		better_dir = left;
		better_dir.direction = -1;
		//speed_up(current, step, -1);
	}
	
	if(better_dir.fitness > current.fitness){
		better_dir = current;
		better_dir.direction = 0;
	}
	update_best(better_dir);
	file << "THE BEST DIRECTION" << "," << better_dir.direction << endl;
	
	return better_dir;
}



int main(){
	
	srand(114);
	write_csv();
	file << fixed << setprecision(20);
	Jaguar JA;
	JA_best.fitness = 9999;
	int round = 1;
	for(int i = 0; i< 1; i++){
		init_step = initial(100.0, -100.0);
		//file << "init_step: " << "," << init_step << endl;
		JA = random_pos();
		while(true){
			file << "ROUND" << "," << round << endl;
			JA = seek_around(JA, init_step);
			file << "AFTER SEEK" << endl;
			if(overflow == true){
				file << "BREAK" << endl;
				break;
			}
			if(JA.direction != 0){
				file << "LEFT OR RIGHT" << endl;
				JA = speed_up(JA, init_step);
				JA = slow_down(JA);
			}
			init_step /= 2;
			round += 1;
		}
	}
	system("pause");
	file.close();
}
