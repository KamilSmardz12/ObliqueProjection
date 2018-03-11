// RzutUkosnyZOporem.cpp

#include "stdafx.h"
#define _USE_MATH_DEFINES //https://msdn.microsoft.com/pl-pl/library/4hwaceh6.aspx
#include<cmath>
#include <iostream>
#include<fstream>

using namespace std;

//wspó³czynnik oporu areodynamicznego - zak³adamy jego sta³oœæ i przyjmujemy wartoœæ jak dla kropli wody
double C = 0.6; //wielkoœæ bezwymiarowa, wyznaczana doœwiadczalnie
//gêstoœæ powietrza
double rho = 1.2; //[kg/m^3]
//pole przekroju poprzecznego cia³a
double S = 0; //[m^2]
//masa obiektu
double m = 1;//kg
//przyspieszenie grwaitacyjne Ziemi
double g = -9.8; //m/s
//Promien cia³a
double R = 0; //m - domyœlnie 0, czyli brak oporu
//Dopusczalny blad przy ktorym zakladamy, ¿e liczba mniejsza od epsilon to zero
double epsilon = 0.000000001; //m
//Laczny czas symulacji
double czas_laczny_symulacji = 0;
//Jeœli u¿ytkownik wybierze opcje ze zderzeniem sprezystym to najlepiej okreslic warunek przerwania symulacji na podstawie ilsoc odbic
int po_ilu_zderzeniach_przerwac = 1; //domyœlnie dajemy 1
//Czy u¿ywamy sprawdzania po ilu zderzeniach przerwac
bool sprawdza_ilosc_odbic = false;

double PrzyspieszenieOdOporu(double predkosc) {
	//Wyliczamy przyspieszenie haumuj¹ce tj. ((1/2)*C*rho*S*v^2)/m
	double kierunek = 1;
	if (predkosc>0)
		kierunek = -1;
	return kierunek*0.5*C*rho*S*pow(predkosc, 2)*(1 / m);
}

double PredkoscSkladowaX(double predkosc_pocztkowa, double przedzial_czasu) {
	double przyspieszenie = PrzyspieszenieOdOporu(predkosc_pocztkowa);
	//Czyli wzór v = v0+at
	return predkosc_pocztkowa + przyspieszenie*przedzial_czasu;
}

double PredkoscSkladowaY(double predkosc_pocztkowa, double przedzial_czasu)
{
	double przyspieszenie = g + PrzyspieszenieOdOporu(predkosc_pocztkowa);
	//Czyli wzór v = v0+(g+a)t
	return predkosc_pocztkowa + przyspieszenie*przedzial_czasu;
}

double PolozenieSkladowaX(double polozenie_pocztkowe, double predkosc_pocztkowa, double przedzial_czasu)
{
	double przyspieszenie = PrzyspieszenieOdOporu(predkosc_pocztkowa);
	//Czyli wzór x = x0+v0t+(1/2)*a*t^2
	return polozenie_pocztkowe + predkosc_pocztkowa*przedzial_czasu + 0.5*przyspieszenie*pow(przedzial_czasu, 2);
}

double PolozenieSkladowaY(double polozenie_pocztkowe, double predkosc_pocztkowa, double przedzial_czasu)
{
	double przyspieszenie = g + PrzyspieszenieOdOporu(predkosc_pocztkowa);
	//Czyli wzór x = x0+v0t+(1/2)*(g+a)*t^2
	return polozenie_pocztkowe + predkosc_pocztkowa*przedzial_czasu + 0.5*przyspieszenie*pow(przedzial_czasu, 2);
}

bool CzyZajdzieZderzenieZPowierzchnia(double polozenie_srodka_masy)
{
	if (polozenie_srodka_masy >= R)
		return false;
	return true;
}


void SymulujAzDoZderzenia(double* predkosc_x, double* predkosc_y, double* polozenie_x, double* polozenie_y, double krok_czasowy, fstream* file)
{
	//Puszczamy petle która wykonuje siê do momenty gdy kula zetknie sie z powierzchni¹ y=0

	while (!CzyZajdzieZderzenieZPowierzchnia((*polozenie_y))) {
		double x = PolozenieSkladowaX(*polozenie_x, *predkosc_x, krok_czasowy);
		double y = PolozenieSkladowaY(*polozenie_y, *predkosc_y, krok_czasowy);
		double v_x = PredkoscSkladowaX(*predkosc_x, krok_czasowy);
		double v_y = PredkoscSkladowaY(*predkosc_y, krok_czasowy);

		(*polozenie_x) = x;
		(*polozenie_y) = y;
		(*predkosc_x) = v_x;
		(*predkosc_y) = v_y;
		(*file) << x << " " << y << "\n";
		czas_laczny_symulacji += krok_czasowy;
	};

	(*polozenie_y) = R + epsilon;
}

void SymulujZderzenieSprezyste(double* predkosc_x, double* predkosc_y, double* polozenie_x, double* polozenie_y, double krok_czasowy, fstream* file)
{
	int ilosc_odbic = 0;
	while (ilosc_odbic <= po_ilu_zderzeniach_przerwac) {
		++ilosc_odbic;
		SymulujAzDoZderzenia(predkosc_x, predkosc_y, polozenie_x, polozenie_y, krok_czasowy, file);
		(*predkosc_y) = (-1)*(*predkosc_y);
	}
}

void SymulujZderzenieNiesprezyste(double* predkosc_x, double* predkosc_y, double* polozenie_x, double* polozenie_y, double krok_czasowy, fstream* file)
{

	SymulujAzDoZderzenia(predkosc_x, predkosc_y, polozenie_x, polozenie_y, krok_czasowy, file);
}

int main()
{
	double predkosc_x = 0, predkosc_y = 0, polozenie_x = 0, polozenie_y = 0;
	cout << "\nPodaj promien kuli [m]: ";
	cin >> R;
	if (R<0) {
		cout << "\nBlad: Promien ciala nie moze byc ujemny.";
        return 0;
	}
	cout << "\nPodaj mase ciala [kg]: ";
	cin >> m;
	if (m <= 0) {
		cout << "\nBlad: Masa ciala musi byc liczba rzeczywista dodatnia.";
		return 0;
	}
	cout << "\nPodaj predkosc pocz¹tkowa wzdloz osi x [m/s]: ";
	cin >> predkosc_x;
	cout << "\nPodaj predkosc pocz¹tkowa wzdluz osi y [m/s]: ";
	cin >> predkosc_y;
	cout << "\nPodaj polozenie pocz¹tkowa wzdluz osi x [m]: ";
	cin >> polozenie_x;
	cout << "\nPodaj polozenie pocz¹tkowa wzdluz osi y [m]: ";
	cin >> polozenie_y;
	//Nie pozwalam na takie poloze w który polozenie + promien przecinaja lub stykaja siê z ziemia
	if (polozenie_y <= 0 || (abs(R - polozenie_y)<epsilon)) {
		cout << "\nBlad: Cialo nie moze przecinac powierzchni y =0 oraz nie moze znajdowac sie ponizje y=0";
		return 0;
	}
	cout << "\n Jesli chcesz symulowac zderzenie sprezyste wpisz 0 a jesli niesprezyste to 1 : ";
	int opcja = 1;
	cin >> opcja;
	if (opcja != 0 && opcja != 1) {
		cout << "Blad: Wybrano niepoprawna opcje. Dopuszczalne wartosci to 0 i 1.";
		return 0;
	}

	if (opcja == 0) {
		cout << "\nPo ilu odbiciach przerwac symulacje : ";
		int ile = 0;
		cin >> ile;
		if (ile<1) {
			cout << "Blad: Ilosc odbic musi byc liczba calkowita nie mniejsza niz 1.";
			return 0;
		}
		po_ilu_zderzeniach_przerwac = ile;
		sprawdza_ilosc_odbic = true;
	}


	double krok_czasowy = 0.0001; //[s] - krok w milisekundach

	//Przekrój poprzeczny kuli czyli pole ko³a
	S = M_PI*pow(R, 2);

	cout << "\nProsze czekac trwa symulacja ...\n";


	double* p_polozenie_x = &polozenie_x;
	double* p_polozenie_y = &polozenie_y;
	double* p_predkosc_x = &predkosc_x;
	double* p_predkosc_y = &predkosc_y;

	fstream file;
	file.open("rzut.txt", ios::out);
	fstream* p_file = &file;
	switch (opcja) {
	case 0:
		SymulujZderzenieSprezyste(p_predkosc_x, p_predkosc_y, p_polozenie_x, p_polozenie_y, krok_czasowy, p_file);
		break;
	case 1:
		SymulujZderzenieNiesprezyste(p_predkosc_x, p_predkosc_y, p_polozenie_x, p_polozenie_y, krok_czasowy, p_file);
		break;
	}

	file.close();

	cout << "\nSymulacja zakonczona";
	cout << "\nPolozenie kocowe (x,y) = (" << polozenie_x << " m, " << polozenie_y << " m)";
	cout << "\nCzas symulacji : " << czas_laczny_symulacji << " s\n";
	system("pause");
	return 0;
}


