#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <vector> 
#include <cstring>
#include <sstream>
#include <math.h>

//////////////////////////////////////////////////////
//	Follow Vanthoor11-part1 , Base on Texas data   s//
//	Out model have no fan, no ventilation and only  //
//	1 way CO2 OUT from TOP						    //
//////////////////////////////////////////////////////

#define A_Flr 78000
#define A_Roof 14040
#define A_Side 0
#define cap_CO2Top 0.4
#define cap_CO2Air 4.7
#define Muy_ExtCO2 720000
#define f_pad 0
#define K_ThScr 0.00025
#define P_InsScr 1
#define T_MeanAir 293
#define Cd 0.65
#define Cw 0.09
#define g 9.8
#define h_Vent 0.97
#define Muy_VentForced 0
#define Muy_Pad 0
#define h_CBuf 1
#define M_CH2O 0.03
#define P_Blow 0
#define v_Wind 2.9
#define p_Mean_Air 2.99
#define c_leakage 0.001
#define n_Side_Thr 0.9
#define n_Side 0.1

double CO2_Air,CO2_Top, CO2_Air_Rate, CO2_Top_Rate,MC_ExtAir,MC_PadAir,MC_BlowAir,MC_AirCan,MC_AirTop,MC_AirOut,MC_TopOut;
double n_heatCO2, U_Blow;
double U_ExtCO2;
double CO2_Out,U_Pad;
double f_ThScr,U_ThScr,T_Air,T_Top,T_Out,p_Air,p_Top,f_VentSide,f_VentForced,U_Roof,U_Side,h_SideRoof;
double n_InsScr, f_leakage, f_VentRoofSide, f_VentRoofSide_A0, U_VentForced, f_VentRoof, ff_VentRoof;

double  rollnum;

using namespace std;

double double_rand(double min, double max)
{
	double scale = rand() / (double)RAND_MAX; /* [0, 1.0] */
	return min + scale * (max - min);      /* [min, max] */
}
void read_record()
{
	ifstream fin("Data.csv");

	// Open an existing file 

	// Get the roll number 
	// of which the data is required 
	double roll2, count = 0;
	cout << "Enter the time: ";
	cin >> rollnum;

	// Read the Data from the file 
	// as String Vector 
	vector<string> row;
	string line, word, temp;
	if (fin.is_open())cout << "OK" << endl;
	else cout << "FAIL" << endl;
	while (!fin.eof()) {

		row.clear();

		// read an entire row and 
		// store it in a string variable 'line' 
		getline(fin, line);

		// used for breaking words 
		stringstream s(line);

		// read every column data of a row and 
		// store it in a string variable, 'word' 
		while (getline(s, word, ','))
		{

			// add all the column data 
			// of a row to a vector 
			row.push_back(word);
		}

		// convert string to integer for comparision 
		roll2 = stoi(row[0]);
		// Compare the roll number 
		if (roll2 == rollnum)
		{

			// Print the found data 
			count = 1;
			CO2_Air = stod(row[1]);
			CO2_Out = stod(row[2]);
			CO2_Top = stod(row[3]);
			break;
		}
	}

	if (count == 0)
		cout << "Record not found\n" << endl;
	
}
int main() {
	// File pointer 
	
	read_record();
	U_Blow = double_rand(0,1.0);
	U_ExtCO2 = double_rand(0, 1.0);
	U_Roof = double_rand(0, 1.0);
	U_Side = double_rand(0, 1.0);
	U_ThScr = double_rand(0, 1.0);

	p_Air = 3;
	p_Top = 2.987;
	T_Air = 295;
	T_Top = 291;
	T_Out = 288.9;

	if (v_Wind < 0.25)f_leakage = 0.25*c_leakage;
	else f_leakage = v_Wind * c_leakage;

	n_InsScr = P_InsScr * (2 - P_InsScr);

	if (n_Side > n_Side_Thr)f_VentSide = n_InsScr * f_VentRoofSide_A0 + 0.5*f_leakage;
	else f_VentSide = n_InsScr * (U_ThScr*f_VentRoofSide_A0 + (1 - U_ThScr)*f_VentRoofSide*n_Side) + 0.5*f_leakage;
	
	f_VentForced = n_InsScr * U_VentForced*Muy_VentForced / A_Flr;
	f_ThScr = U_ThScr * K_ThScr*pow((T_Air - T_Top), 0.66) + (1 - U_ThScr)*pow(((g*(1 - U_ThScr) / (2 * p_Mean_Air))*abs(p_Air - p_Top)),0.5);
	f_VentRoofSide = (Cd / A_Flr)*pow((((U_Roof*U_Roof*U_Side*U_Side*A_Roof*A_Roof*A_Side*A_Side) / (pow(U_Roof, 2)*pow(A_Roof, 2) + pow(U_Side, 2)*pow(A_Side, 2)))*(2 * g*h_SideRoof*(T_Air - T_Top) / T_MeanAir) + pow((U_Roof*A_Roof + U_Side * A_Side) / 2, 2)*Cw*pow(v_Wind, 2)), 1 / 2);
	f_VentRoofSide_A0 = (Cd / A_Flr)*pow((((U_Roof*U_Roof*U_Side*U_Side * 0 * 0 * A_Side*A_Side) / (pow(U_Roof, 2)*pow(0, 2) + pow(U_Side, 2)*pow(A_Side, 2)))*(2 * g*h_SideRoof*(T_Air - T_Top) / T_MeanAir) + pow((U_Roof * 0 + U_Side * A_Side) / 2, 2)*Cw*pow(v_Wind, 2)), 1 / 2);
	f_VentRoof = (Cd*U_Roof*A_Roof / (2 * A_Flr))*pow((g*h_Vent*(T_Air - T_Out) / (2 * T_MeanAir)) + Cw * pow(v_Wind, 2), 0.5);
	
	//Ex2
	MC_AirOut = (f_VentSide + f_VentForced)*(CO2_Air - CO2_Out);
	MC_PadAir = (U_Pad*Muy_Pad / A_Flr)*(CO2_Out - CO2_Air);
	MC_AirTop = f_ThScr * (CO2_Air - CO2_Top);
	MC_ExtAir = U_ExtCO2 * Muy_ExtCO2 / A_Flr;
	MC_BlowAir = n_heatCO2*U_Blow*P_Blow/A_Flr;
	MC_TopOut = (n_InsScr * f_VentRoof +0.5 *f_leakage) * (CO2_Top - CO2_Out);
	MC_AirCan = M_CH2O * h_CBuf*100;
	CO2_Air_Rate = (MC_BlowAir + MC_ExtAir + MC_PadAir - MC_AirCan - MC_AirTop - MC_AirOut) / cap_CO2Air;
	CO2_Top_Rate = (MC_AirTop - MC_TopOut) / cap_CO2Top;
	//Ex3
	cout << " __________________________________ "		<< endl;
	cout << "| ---------------dx--------------- |"		<< endl;
	cout << "|  System Have No Fan,Heater,Side   "		<< endl;
	cout << "| CO2 AIR Rate      = " << CO2_Air_Rate	<< endl;
	cout << "| CO2 TOP Rate      = " << CO2_Top_Rate	<< endl;
	cout << "| CO2 External->Air = " << MC_ExtAir		<< endl;
	cout << "| CO2 Heater  ->Air = " << MC_BlowAir		<< endl;
	cout << "| CO2 Pad     ->Air = " << MC_PadAir		<< endl;
	cout << "| CO2 Air -> Canopy = " << MC_AirCan		<< endl;
	cout << "| CO2 Air -> Top    = " << MC_AirTop		<< endl;
	cout << "| CO2 Air -> Out    = " << MC_AirOut		<< endl;
	cout << "| CO2 Top -> Out    = " << MC_TopOut		<< endl;
	cout << "|__________________________________|"		<< endl;
	//Ex4a
	double CO2_Air_t_h_Ex4a, CO2_Top_t_h_Ex4a;
	double CO2_Air_t=0, CO2_Top_t=0;
	double h;
	cout << "Please Input h: ";
	cin >> h;
	//We have just combine 12 min as 12 testcases from GITHUB so that
	while (h > (12 - rollnum)) {
		cout << "Error-Please input another:" << endl;
		cin >> h;
	}

	//Euler
	CO2_Air_t_h_Ex4a = CO2_Air + CO2_Air_Rate * h;
	CO2_Top_t_h_Ex4a = CO2_Top + CO2_Top_Rate * h;

	//Runge-Kutta

	double CO2_Air_t_h, CO2_Top_t_h, CO2_Air_t_5, CO2_Top_t_5, CO2_Air_t_10, CO2_Top_t_10;
	double k1, k2, k3, k4;
	double k_1, k_2, k_3, k_4;
	//Because CO2_Air_Rate & CO2_Top_Rate can be found as a const so de differentiation = 0 for all derivatives funcs
	k1 = CO2_Air_Rate;
	k2 = CO2_Air_Rate;
	k3 = CO2_Air_Rate;
	k4 = CO2_Air_Rate;
	
	k_1 = CO2_Top_Rate;
	k_2 = CO2_Top_Rate;
	k_3 = CO2_Top_Rate;
	k_4 = CO2_Top_Rate;

	CO2_Air_t_h = CO2_Air + (h / 6)*(k1 + (2 * k2) + (2 * k3) + k4);
	CO2_Top_t_h = CO2_Top + (h / 6)*(k_1 + (2 * k_2) + (2 * k_3) + k_4);
	CO2_Air_t_5 = CO2_Air + CO2_Air_Rate * 5;
	CO2_Top_t_5 = CO2_Top + CO2_Top_Rate * 5;
	CO2_Air_t_10 = CO2_Air + CO2_Air_Rate * 10;
	CO2_Top_t_10 = CO2_Top + CO2_Top_Rate * 10;

	cout << "CO2_Air at t+h = " << CO2_Air_t_h << endl;
	cout << "CO2_Top at t+h = " << CO2_Top_t_h << endl;
	//with h = 5mins - EUler Method
	
	cout << "CO2_Air at t + 5 = " << CO2_Air_t_5<< endl;
	cout << "CO2_Top at t + 5 = " << CO2_Top_t_5 << endl;
	//with h = 10 mins - Euler Method
	
	cout << "CO2_Air at t + 10 = " << CO2_Air_t_10 << endl;
	cout << "CO2_Top at t + 10 = " << CO2_Top_t_10 << endl;
}