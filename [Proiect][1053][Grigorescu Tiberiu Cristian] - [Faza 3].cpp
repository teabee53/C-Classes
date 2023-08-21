#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <list>
#include <map>
using namespace std;

//class Telefon, Procesor (Telefon has a Procesor), ProdusMagazin, Furnizor, Client
//Procesor: char* model (alocat dinamic), const int nrNuclee (4, 6 sau 8), float* NucleeGHz ( > 0 ) (VALORILE DIN VECTOR VOR FI EGALE)(alocat static)
//Telefon: char* model (alocat dinamic), float* steleRecenzii (INTRE 0 SI 5) (alocat static), int nrRecenzii, Procesor
//ProdusMagazin: char* model (alocat dinamic), int* nrVanzariLunareAnPrec ( > 0 )(alocat static), float pret, static float pretMinim, is a Telefon (idk yet)
//Furnizor: char nume (alocat static), int* nrBucati ( > 0 )(alocat dinamic)
//Client: char nume (alocat static), float* sumePlatite ( > 0 )(alocat dinamic)

class IFile
{
public:
	virtual void fromTextFileToBinFile(ifstream& f) = 0; //fisierele sunt cele text primite ca argumente pentru main
	virtual void fromCSVFileToBinFile(ifstream& f) = 0; //fisiere csv
	virtual void writeToBinFile(fstream& f) = 0; //se scriu in fisiere binare datele introduse in program
};

class Procesor :public IFile 
{
private:
	char* model = nullptr;
	const int nrNuclee;
	float nucleeGHz[8] = { 0 };
public:

	void fromTextFileToBinFile(ifstream& f)
	{
		fstream fout("procesor.bin", ios::out | ios::binary);
		char model[20];
		int i = 0;
		while (true)
		{
			int nrnuclee;
			if (this->model != nullptr)
				delete[] this->model;
			if (i != 0)
				f.ignore();
			f.getline(model, 20);
			this->model = new char[strlen(model) + 1];
			strcpy(this->model, model);
			if (f >> nrnuclee)
				{ }
			else break;
			int lg = sizeof(this->model) + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(this->model, lg);
			fout.write((char*)&nrnuclee, sizeof(int));
			float nucleeGHz;
			for (int i = 0; i < nrnuclee; i++) //am facut asta ca sa se citeasca TOATE valorile ramase de la acest Procesor
				f >> nucleeGHz;
			for (int i = 0; i < nrnuclee; i++)
			{
				this->nucleeGHz[i] = nucleeGHz;
				fout.write((char*)&nucleeGHz, sizeof(float));
			}
			i++;
		}
		fout.close();
	}

	void fromCSVFileToBinFile(ifstream& f)
	{
		fstream fout("procesorCSV.bin", ios::out | ios::binary);
		char model[20];
		while (true)
		{
			string nrnuclee;
			if (this->model != nullptr)
				delete[] this->model;
			f.getline(model, 20);
			this->model = new char[strlen(model) + 1];
			strcpy(this->model, model);
			if (getline(f, nrnuclee, ','))
			{
			}
			else
			{
				break;	
			}
			int lg = strlen(this->model);
			fout.write((char*)&lg, sizeof(int));
			fout.write(this->model, lg);
			lg = nrnuclee.length() + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(nrnuclee.data(), lg);
			int nrnucl = stoi(nrnuclee);
			string nucleeGHz;
			for (int j = 0; j < (nrnucl - 1); j++)
			{
				getline(f, nucleeGHz, ',');
				lg = nucleeGHz.length() + 1;
				fout.write((char*)&lg, sizeof(int));
				fout.write(nucleeGHz.data(), lg);
			}
			getline(f, nucleeGHz, '\n');
			lg = nucleeGHz.length() + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(nucleeGHz.data(), lg);
		}
		fout.close();
	}

	void writeToBinFile(fstream& f)
	{
		int lg = sizeof(this->model) + 1;
		f.write((char*)&lg, sizeof(int));
		f.write(this->model, lg);
		f.write((char*)&this->nrNuclee, sizeof(int));
		for (int i = 0; i < this->nrNuclee; i++)
			f.write((char*)&this->nucleeGHz[i], sizeof(float));
	}

	Procesor(char* model, int nrNuclee, float* nucleeGHz) :nrNuclee(nrNuclee)
	{
		this->model = new char[strlen(model) + 1];
		strcpy(this->model, model);
		for (int i = 0; i < this->nrNuclee; i++)
			if (nucleeGHz[i] > 0)
				this->nucleeGHz[i] = nucleeGHz[i];
	}

	Procesor(int nrNuclee) :nrNuclee(nrNuclee) {}

	Procesor(const Procesor& p) :nrNuclee(p.nrNuclee)
	{
		if (p.model != nullptr) {
			this->model = new char[strlen(p.model) + 1];
			strcpy(this->model, p.model);
		}
		if (this->nrNuclee > 0) {
			for (int i = 0; i < this->nrNuclee; i++)
				this->nucleeGHz[i] = p.nucleeGHz[i];
		}
	}

	Procesor& operator=(const Procesor& p)
	{
		if (this != &p)
		{
			if (this->model != nullptr)
				delete[] this->model;
			if (p.model != nullptr)
			{
				this->model = new char[strlen(p.model) + 1];
				strcpy(this->model, p.model);
			}
			else this->model = nullptr;
			if (p.nrNuclee > 0)
				for (int i = 0; i < this->nrNuclee; i++)
					this->nucleeGHz[i] = p.nucleeGHz[0];
		}
		return *this;
	}

	float& operator[](int index)
	{
		if (index >= 0 && index < this->nrNuclee)
			return this->nucleeGHz[index];
	}

	Procesor operator+(int GHz)
	{
		Procesor rez = *this;
		for (int i = 0; i < rez.nrNuclee; i++)
			rez.nucleeGHz[i] += GHz;
		return rez;
	}

	Procesor& operator++() //pre
	{
		for (int i = 0; i < this->nrNuclee; i++)
			this->nucleeGHz[i] += 1;
		return *this;
	}

	Procesor operator++(int x) //post
	{
		Procesor copie = *this;
		for (int i = 0; i < this->nrNuclee; i++)
			this->nucleeGHz[i] += 1;
		return copie;
	}

	operator int()
	{
		return this->nucleeGHz[0];
	}

	bool operator!()
	{
		if (this->nucleeGHz[0] == 0)
			return true;
		return false;
	}

	bool operator>(const Procesor& p)
	{
		if (this->nrNuclee > p.nrNuclee)
			return true;
		return false;
	}

	bool operator==(const Procesor& p)
	{
		if ((strcmp(this->model, p.model) != 0) || (this->nrNuclee != p.nrNuclee))
			return 0;
		for (int i = 0; i < this->nrNuclee; i++)
			if (this->nucleeGHz[i] != p.nucleeGHz[i])
				return 0;
		return 1;
	}

	char* getModel()
	{
		if (this->model != nullptr)
			return this->model;
	}

	void setModel(char* model)
	{
		if (this->model != nullptr)
			delete[] this->model;
		this->model = new char[strlen(model) + 1];
		strcpy(this->model, model);
	}

	const int getNrNuclee()
	{
		return this->nrNuclee;
	}

	float* getNucleeGHz()
	{
		return this->nucleeGHz;
	}

	void setNucleeGHz(float GHz)
	{
		if (GHz > 0)
			for (int i = 0; i < this->nrNuclee; i++)
				this->nucleeGHz[i] = GHz;
		else throw new exception("GHz trebuie sa fie pozitiv");
	}
	
	friend istream& operator>>(istream& in, Procesor& p)
	{
		if (p.model != nullptr)
			delete[] p.model;
		cout << "\nIntroduceti numele modelului de Procesor: ";
		//in.ignore();
		char model[20];
		in.getline(model, 20);
		p.model = new char[strlen(model) + 1];
		strcpy(p.model, model);
		cout << "\nIntroduceti frecventa/viteza Procesorului: ";
		float GHz;
		in >> GHz;
		for (int i = 0; i < p.nrNuclee; i++)
			p.nucleeGHz[i] = GHz;
		return in;
	}

	friend ostream& operator<<(ostream& out, const Procesor& p)
	{
		if (p.model != nullptr)
			out << "\nModel: " << p.model;
		out << "\nNumar nuclee : " << p.nrNuclee;
		out << "\nViteza nuclee:";
		for (int i = 0; i < p.nrNuclee; i++)
			out << p.nucleeGHz[i] << "GHz ";
		return out;
	}

	~Procesor()
	{
		delete[] model;
	}
};

//Telefon: char* model (alocat dinamic), float* steleRecenzii (INTRE 0 SI 5) (alocat static), int nrRecenzii, Procesor
class Telefon :public IFile{
private:
	char* model = nullptr;
	float steleRecenzii[20] = { 0 }; // 0 < x < 5
	int nrRecenzii = 0; // > 0
	Procesor cpu;

public:

	void fromTextFileToBinFile(ifstream& f) { }

	void fromCSVFileToBinFile(ifstream& f) { }

	void writeToBinFile(fstream& f)
	{
		int lg = sizeof(this->model) + 1;
		f.write((char*)&lg, sizeof(int));
		f.write(this->model, lg);
		f.write((char*)&this->nrRecenzii, sizeof(int));
		for (int i = 0; i < this->nrRecenzii; i++)
			f.write((char*)&this->steleRecenzii[i], sizeof(float));
		int lg2 = sizeof(this->cpu.getModel()) + 1;
		f.write(this->cpu.getModel(), lg2);
		int nrnuclee = this->cpu.getNrNuclee();
		f.write((char*)&nrnuclee, sizeof(int));
		float* nucleeGHz = this->cpu.getNucleeGHz();
		for (int i = 0; i < nrnuclee; i++)
			f.write((char*)&nucleeGHz[i], sizeof(float));
	}

	Telefon(char* model, int nrRecenzii, float* steleRecenzii, Procesor cpu) :cpu(cpu)
	{
		this->model = new char[strlen(model) + 1];
		strcpy(this->model, model);
		this->nrRecenzii = nrRecenzii;
		for (int i = 0; i < 20; i++)
			if (steleRecenzii[i] > 0)
				this->steleRecenzii[i] = steleRecenzii[i];
		this->cpu = cpu;
	}

	Telefon(Procesor cpu): cpu(cpu) {}

	Telefon(const Telefon& t) :cpu(t.cpu)
	{
		if (t.model != nullptr) {
			this->model = new char[strlen(t.model) + 1];
			strcpy(this->model, t.model);
		}
		this->nrRecenzii = t.nrRecenzii;
		if (this->nrRecenzii > 0) {
			for (int i = 0; i < this->nrRecenzii; i++)
				this->steleRecenzii[i] = t.steleRecenzii[i];
		}
	}

	Telefon& operator=(const Telefon& t)
	{
		if (this != &t)
		{
			if (this->model != nullptr)
				delete[] this->model;
			if (t.model != nullptr)
			{
				this->model = new char[strlen(t.model) + 1];
				strcpy(this->model, t.model);
			}
			else
				this->model = nullptr;
			if (t.nrRecenzii > 0)
			{
				this->nrRecenzii = t.nrRecenzii;
				for (int i = 0; i < this->nrRecenzii; i++)
					this->steleRecenzii[i] = t.steleRecenzii[i];
			}
			this->cpu = t.cpu;
		}
		return *this;
	}

	float& operator[](int index)
	{
		if (index >= 0 && index < this->nrRecenzii)
			return this->steleRecenzii[index];
	}

	Telefon operator+(int recenzie)
	{
		Telefon rez = *this;
		rez.nrRecenzii++;
		for (int i = 0; i < this->nrRecenzii; i++)
			rez.steleRecenzii[i] = this->steleRecenzii[i];
		rez.steleRecenzii[rez.nrRecenzii - 1] = recenzie;
		return rez;
	}

	Telefon& operator++() //pre
	{
		if (this->steleRecenzii[this->nrRecenzii - 1] < 4)
			this->steleRecenzii[this->nrRecenzii - 1]++;
		else
			this->steleRecenzii[this->nrRecenzii - 1] = 5;
		return *this;
	}

	Telefon operator++(int x) //post
	{
		Telefon copie = *this;
		if (this->steleRecenzii[this->nrRecenzii - 1] < 4)
			this->steleRecenzii[this->nrRecenzii - 1]++;
		else
			this->steleRecenzii[this->nrRecenzii - 1] = 5;
		return copie;
	}

	operator int()
	{
		return this->steleRecenzii[this->nrRecenzii - 1];
	}

	bool operator!()
	{
		if (nrRecenzii == 0)
			return true;
		return false;
	}

	bool operator>(const Telefon& t)
	{
		if (this->nrRecenzii > t.nrRecenzii)
			return true;
		return false;
	}

	bool operator==(const Telefon& t)
	{
		if ((strcmp(this->model, t.model) != 0) || (this->nrRecenzii != t.nrRecenzii))
			return 0;
		for (int i = 0; i < this->nrRecenzii; i++)
			if (this->steleRecenzii[i] != t.steleRecenzii[i])
				return 0;
		if (!(this->cpu == t.cpu))
			return 0;
		return 1;
	}

	char* getModel()
	{
		if (this->model != nullptr)
			return this->model;
	}

	void setModel(char* model)
	{
		if (this->model != nullptr)
			delete[] this->model;
		this->model = new char[strlen(model) + 1];
		strcpy(this->model, model);
	}

	int getNrRecenzii()
	{
		return this->nrRecenzii;
	}

	float* getSteleRecenzii()
	{
		return this->steleRecenzii;
	}

	void setSteleRecenzii(float* steleRecenzii, int nrRecenzii)
	{
		this->nrRecenzii = nrRecenzii;
		for (int i = 0; i < this->nrRecenzii; i++)
			if (steleRecenzii[i] >= 0 && steleRecenzii[i] <= 5)
				this->steleRecenzii[i] = steleRecenzii[i];
			else throw new exception("Recenziile trebuie sa fie intre 0 si 5");
	}

	Procesor getCpu()
	{
		return this->cpu;
	}

	void setCpu(Procesor cpu)
	{
		this->cpu = cpu;
	}

	friend istream& operator>>(istream& in, Telefon& t)
	{
		if (t.model != nullptr)
			delete[] t.model;
		in.ignore();
		cout << "Introduceti numele modelului de Telefon: ";
		char model[20];
		in.getline(model, 20);
		t.model = new char[strlen(model) + 1];
		strcpy(t.model, model);
		cout << "Introduceti numarul de recenzii: ";
		in >> t.nrRecenzii;
		cout << "Introduceti recenziile: ";
		for (int i = 0; i < t.nrRecenzii; i++)
		{
			cout << "steleRecenzii[" << i << "]: ";
			in >> t.steleRecenzii[i];
		}
		cout << "Introduceti Procesorul: ";
		in >> t.cpu;
		return in;
	}

	friend ostream& operator<<(ostream& out, const Telefon& t)
	{
		if(t.model!=nullptr)
			out << "\nModel: " << t.model;
		out << "\nNumar recenzii : " << t.nrRecenzii;
		out << "\nRecenzii:";
		for (int i = 0; i < t.nrRecenzii; i++)
			out << t.steleRecenzii[i] << " ";
		out << "\nProcesor: ";
		out << t.cpu;
		return out;
	}

	friend bool operator<(const Telefon& t1, const Telefon& t2)
	{
		if (t1.nrRecenzii < t2.nrRecenzii)
			return true;
		return false;
	}

	~Telefon()
	{
		delete[] model;
	}
};
//ProdusMagazin: char* model (alocat dinamic), int* nrVanzariLunareAnPrec ( > 0 )(alocat static), float pret, static float pretMinim, is a Telefon
class ProdusMagazin :public IFile
{
private:
	char* model = nullptr; //modelul este preluat automat de la obiectul de tip Telefon
	int nrVanzariLunareAnPrec[12] = { 0 };
	static float pretMinim;
	float pret;
	Telefon tel;
public:

	void fromTextFileToBinFile(ifstream& f) { }

	void fromCSVFileToBinFile(ifstream& f) { }

	void writeToBinFile(fstream& f)
	{
		int lg = sizeof(this->model) + 1;
		f.write((char*)&lg, sizeof(int));
		f.write(this->model, lg);
		int nrluni = 12;
		f.write((char*)&nrluni, sizeof(int));
		for (int i = 0; i < 12; i++)
			f.write((char*)&this->nrVanzariLunareAnPrec[i], sizeof(int));
		f.write((char*)&this->pretMinim, sizeof(float));
		f.write((char*)&this->pret, sizeof(float));
		int nrRecenzii = this->tel.getNrRecenzii();
		f.write((char*)&nrRecenzii, sizeof(int));
		float* recenzii = this->tel.getSteleRecenzii();
		for (int i = 0; i < nrRecenzii; i++)
			f.write((char*)&recenzii[i], sizeof(float));
		Procesor Cpu = this->tel.getCpu();
		int lg2 = sizeof(Cpu.getModel()) + 1;
		f.write(Cpu.getModel(), lg2);
		int nrnuclee = Cpu.getNrNuclee();
		f.write((char*)&nrnuclee, sizeof(int));
		float* nucleeGHz = Cpu.getNucleeGHz();
		for (int i = 0; i < nrnuclee; i++)
			f.write((char*)&nucleeGHz[i], sizeof(float));
	}

	ProdusMagazin(Telefon tel, int* nrVanzariLunareAnPrec, float pret) :tel(tel)
	{
		this->model = new char[strlen(tel.getModel()) + 1];
		strcpy(this->model, tel.getModel());
		for (int i = 0; i < 12; i++)
			if (nrVanzariLunareAnPrec[i] > 0)
				this->nrVanzariLunareAnPrec[i] = nrVanzariLunareAnPrec[i];
		if (pret >= this->pretMinim)
			this->pret = pret;
		else throw new exception("pret > pret minim!");
	}

	ProdusMagazin(Telefon tel) :tel(tel)
	{
		this->model = new char[strlen(tel.getModel()) + 1];
		strcpy(this->model, tel.getModel());
	}

	ProdusMagazin(const ProdusMagazin& p) :tel(p.tel)
	{
		if (p.model != nullptr) {
			this->model = new char[strlen(p.model) + 1];
			strcpy(this->model, p.model);
		}
		for (int i = 0; i < 12; i++)
			this->nrVanzariLunareAnPrec[i] = p.nrVanzariLunareAnPrec[i];
		this->pret = p.pret;
	}

	ProdusMagazin& operator=(const ProdusMagazin& p)
	{
		if (this != &p)
		{
			if (this->model != nullptr)
				delete[] this->model;
			if (p.model != nullptr)
			{
				this->model = new char[strlen(p.model) + 1];
				strcpy(this->model, p.model);
			}
			else this->model = nullptr;
			for (int i = 0; i < 12; i++)
				this->nrVanzariLunareAnPrec[i] = p.nrVanzariLunareAnPrec[i];
			this->pret = p.pret;
			this->tel = p.tel;
		}
		return *this;
	}

	int& operator[](int index)
	{
		if (index >= 0 && index < 12)
			return this->nrVanzariLunareAnPrec[index];
	}

	ProdusMagazin operator+(int pret)
	{
		ProdusMagazin rez = *this;
		rez.pret += pret;
		return rez;
	}

	ProdusMagazin& operator++() //pre
	{
		this->pret += 100;
		return *this;
	}

	ProdusMagazin operator++(int x) //post
	{
		ProdusMagazin copie = *this;
		this->pret += 100;
		return copie;
	}

	operator int()
	{
		return this->pret;
	}

	bool operator!()
	{
		if (this->nrVanzariLunareAnPrec[11] == 0)
			return true;
		return false;
	}

	bool operator>(const ProdusMagazin& p)
	{
		if (this->pret > p.pret)
			return true;
		return false;
	}

	bool operator==(const ProdusMagazin& p)
	{
		if (strcmp(this->model, p.model) != 0)
			return 0;
		for (int i = 0; i < 12; i++)
			if (this->nrVanzariLunareAnPrec[i] != p.nrVanzariLunareAnPrec[i])
				return 0;
		if (!(this->tel == p.tel))
			return 0;
		if (this->pret != p.pret)
			return 0;
		return 1;
	}

	char* getModel()
	{
		return this->model;
	}

	void setModel(char* model)
	{
		if (this->model != nullptr)
			delete[] this->model;
		this->model = new char[strlen(model) + 1];
		strcpy(this->model, model);
	}

	float getPret()
	{
		return this->pret;
	}

	void setPret(float pret)
	{
		if (pret >= this->pretMinim)
			this->pret = pret;
		else throw new exception("Pretul trebuie sa fie >= pret minim");
	}

	float getPretMinim()
	{
		return this->pretMinim;
	}

	void setPretMinim(float pretMinim)
	{
		if (pretMinim >= 0)
			ProdusMagazin::pretMinim = pretMinim;
		else throw new exception("Pretul minim trebuie sa fie pozitiv");
	}

	int* getNrVanzariLunareAnPrec()
	{
		return this->nrVanzariLunareAnPrec;
	}

	void setNrVanzariLunareAnPrec(int* nrVanzariLunareAnPrec)
	{
		for (int i = 0; i < 12; i++)
			if (nrVanzariLunareAnPrec[i] >= 0)
				this->nrVanzariLunareAnPrec[i] = nrVanzariLunareAnPrec[i];
			else throw new exception("Nr vanzari trebuie sa fie pozitiv");
	}

	Telefon getTel()
	{
		return this->tel;
	}

	void setTel(Telefon tel)
	{
		this->tel = tel;
	}

	friend istream& operator>>(istream& in, ProdusMagazin& p)
	{
		if (p.model != nullptr)
			delete[] p.model;
		cout << "Introduceti numele produsului: ";
		char model[30];
		in.getline(model, 30);
		p.model = new char[strlen(model) + 1];
		strcpy(p.model, model);
		cout << "Introduceti numarul de vanzari lunare pentru anul precedent: ";
		for (int i = 0; i < 12; i++)
		{
			cout << "Luna " << i + 1 << ": ";
			in >> p.nrVanzariLunareAnPrec[i];
		}
		cout << "Introduceti pretul produsului: ";
		in >> p.pret;
		cout << "Introduceti specificatiile Telefonului: ";
		in >> p.tel;
		return in;
	}

	friend ostream& operator<<(ostream& out, const ProdusMagazin& p)
	{
		out << "\n==================================";
		out << "\nModel: " << p.model;
		out << "\nNumar de vanzari lunare pentru anul precedent: ";
		for (int i = 0; i < 12; i++)
			out << p.nrVanzariLunareAnPrec[i] << " ";
		out << "\nPret: " << p.pret;
		out << "\nSpecificatii Telefon: ";
		out << p.tel;
		out << "\n==================================";
		return out;
	}

	~ProdusMagazin()
	{
		delete[] model;
	}
};

//Furnizor: char nume (alocat static), int* nrBucati ( > 0 )(alocat dinamic)
class Furnizor :public IFile
{
private:
	char nume[20] = { 'A', 'n', 'o', 'n', 'i', 'm'};
	int nrProduseFurnizate = 0; // >= 0
	int* nrBucati = nullptr; // >= 0
public:

	void fromTextFileToBinFile(ifstream& f)
	{
		fstream fout("furnizor.bin", ios::out | ios::binary);
		char nume[20];
		int i = 0;
		while (true)
		{
			int nrProduseFurnizate;
			if (i != 0)
				f.ignore();
			f.getline(nume, 20);
			strcpy(this->nume, nume);
			if (f >> nrProduseFurnizate)
				{ }
			else 
				break;
			if (this->nrBucati != nullptr)
				delete[] this->nrBucati;
			int lg = sizeof(this->nume) + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(this->nume, lg);
			fout.write((char*)&nrProduseFurnizate, sizeof(int));
			int nrBucati[15];
			this->nrBucati = new int[nrProduseFurnizate];
			for (int i = 0; i < nrProduseFurnizate; i++)
				f >> nrBucati[i];
			for (int i = 0; i < nrProduseFurnizate; i++)
			{
				this->nrBucati[i] = nrBucati[i];
				fout.write((char*)&nrBucati[i], sizeof(int));
			}
			i++;
		}
		fout.close();
	}

	void fromCSVFileToBinFile(ifstream& f)
	{
		fstream fout("furnizorCSV.bin", ios::out | ios::binary);
		while (true)
		{
			string nrProduse;
			f.getline(this->nume, 20);
			char* nume = new char[strlen(this->nume) + 1];
			strcpy(nume, this->nume);
			if (getline(f, nrProduse, ','))
			{
			}
			else
			{
				break;
			}
			int lg = strlen(nume);
			fout.write((char*)&lg, sizeof(int));
			fout.write(nume, lg);
			lg = nrProduse.length() + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(nrProduse.data(), lg);
			int nrprod = stoi(nrProduse);
			string nrbuc;
			for (int j = 0; j < (nrprod - 1); j++)
			{
				getline(f, nrbuc, ',');
				lg = nrbuc.length() + 1;
				fout.write((char*)&lg, sizeof(int));
				fout.write(nrbuc.data(), lg);
			}
			getline(f, nrbuc, '\n');
			lg = nrbuc.length() + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(nrbuc.data(), lg);
			delete[] nume;
		}
		fout.close();
	}

	void writeToBinFile(fstream& f)
	{
		int lg = sizeof(this->nume) + 1;
		f.write((char*)&lg, sizeof(int));
		f.write(this->nume, lg);
		f.write((char*)&this->nrProduseFurnizate, sizeof(int));
		for (int i = 0; i < this->nrProduseFurnizate; i++)
			f.write((char*)&this->nrBucati[i], sizeof(int));
	}

	virtual void afisare()
	{
		cout << "\nFUNCTIE VIRTUALA DE AFISARE, CLASA DE BAZA";
		cout << "\nNume: " << this->nume;
		cout << "\nNumar tipuri de produse furnizate : " << this->nrProduseFurnizate;
		cout << "\nNumar bucati furnizate din fiecare produs:";
		for (int i = 0; i < this->nrProduseFurnizate; i++)
			cout << this->nrBucati[i] << " ";
		cout << endl;
	}

	Furnizor(char* nume, int nrProduseFurnizate, int* nrBucati)
	{
		strcpy(this->nume, nume);
		if (nrProduseFurnizate > 0 && nrBucati != nullptr)
		{
			this->nrProduseFurnizate = nrProduseFurnizate;
			this->nrBucati = new int[this->nrProduseFurnizate];
			for (int i = 0; i < this->nrProduseFurnizate; i++)
				if (nrBucati[i] >= 0)
					this->nrBucati[i] = nrBucati[i];
		}
	}

	Furnizor() {};

	Furnizor(const Furnizor& f)
	{
		strcpy(this->nume, f.nume);
		if (f.nrProduseFurnizate > 0 && f.nrBucati!= nullptr)
		{
			this->nrProduseFurnizate = f.nrProduseFurnizate;
			this->nrBucati = new int[this->nrProduseFurnizate];
			for (int i = 0; i < this->nrProduseFurnizate; i++)
				if (f.nrBucati[i] >= 0)
					this->nrBucati[i] = f.nrBucati[i];
		}
	}

	Furnizor& operator=(const Furnizor& f)
	{
		if (this != &f)
		{
			strcpy(this->nume, f.nume);
			if (f.nrProduseFurnizate > 0 && f.nrBucati != nullptr)
			{
				this->nrProduseFurnizate = f.nrProduseFurnizate;
				this->nrBucati = new int[this->nrProduseFurnizate];
				for (int i = 0; i < this->nrProduseFurnizate; i++)
					if (f.nrBucati[i] >= 0)
						this->nrBucati[i] = f.nrBucati[i];
			}
			else
			{
				this->nrBucati = nullptr;
				this->nrProduseFurnizate = 0;
			}
		}
		return *this;
	}

	int& operator[](int index)
	{
		if (index >= 0 && index < this->nrProduseFurnizate)
			return this->nrBucati[index];
	}

	Furnizor operator+(int nrBucati)
	{
		Furnizor rez = *this;
		if (nrBucati > 0)
		{
			rez.nrProduseFurnizate++;
			delete[] rez.nrBucati;
			rez.nrBucati = new int[rez.nrProduseFurnizate];
			for (int i = 0; i < (rez.nrProduseFurnizate - 1); i++)
				rez.nrBucati[i] = this->nrBucati[i];
			rez.nrBucati[rez.nrProduseFurnizate - 1] = nrBucati;
		}
		return rez;
	}

	Furnizor& operator--() //pre
	{
		if (this->nrProduseFurnizate > 0)
		{
			int nrBucatiCopie[20] = { 0 };
			this->nrProduseFurnizate--;
			for (int i = 0; i < this->nrProduseFurnizate; i++)
				nrBucatiCopie[i] = this->nrBucati[i];
			delete[] this->nrBucati;
			this->nrBucati = new int[this->nrProduseFurnizate];
			for (int i = 0; i < this->nrProduseFurnizate; i++)
				this->nrBucati[i] = nrBucatiCopie[i];
		}
		return *this;
	}

	Furnizor operator--(int x) //post
	{
		Furnizor copie = *this;
		if (this->nrProduseFurnizate > 0)
		{
			int nrBucatiCopie[20] = { 0 };
			this->nrProduseFurnizate--;
			for (int i = 0; i < this->nrProduseFurnizate; i++)
				nrBucatiCopie[i] = this->nrBucati[i];
			delete[] this->nrBucati;
			this->nrBucati = new int[this->nrProduseFurnizate];
			for (int i = 0; i < this->nrProduseFurnizate; i++)
				this->nrBucati[i] = nrBucatiCopie[i];
		}
		return copie;
	}

	operator string()
	{
		string nume;
		nume = this->nume;
		return nume;
	}

	bool operator!()
	{
		int contor = 0;
		for (int i = 0; i < this->nrProduseFurnizate; i++)
			if (this->nrBucati[i] != 0)
				contor++;
		if (contor == 0)
			return true;
		if (this->nrProduseFurnizate == 0)
			return true;
		return false;
	}

	bool operator>(const Furnizor& f)
	{
		if (this->nrProduseFurnizate > f.nrProduseFurnizate)
			return true;
		return false;
	}

	bool operator==(const Furnizor& f)
	{
		if ((strcmp(this->nume, f.nume) != 0) || (this->nrProduseFurnizate != f.nrProduseFurnizate))
			return 0;
		for (int i = 0; i < this->nrProduseFurnizate; i++)
			if (this->nrBucati[i] != f.nrBucati[i])
				return 0;
		return 1;
	}

	char* getNume()
	{
		return this->nume;
	}

	void setNume(char* nume)
	{
		strcpy(this->nume, nume);
	}

	int getNrProduseFurnizate()
	{
		return this->nrProduseFurnizate;
	}

	void setNrProduseFurnizate(int nrProduseFurnizate)
	{
		if (nrProduseFurnizate > 0)
			this->nrProduseFurnizate = nrProduseFurnizate;
	}

	int* getNrBucati()
	{
		return this->nrBucati;
	}

	void setNrBucati(int* nrBucati, int nrProduseFurnizate)
	{
		if (this->nrBucati != nullptr)
		{
			delete[] this->nrBucati;
		}
		this->nrProduseFurnizate = nrProduseFurnizate;
		this->nrBucati = new int[this->nrProduseFurnizate];
		if (nrProduseFurnizate > 0)
			for (int i = 0; i < this->nrProduseFurnizate; i++)
				if (nrBucati[i] > 0)
					this->nrBucati[i] = nrBucati[i];
				else throw new exception("Nr de produse trebuie sa fie pozitiv");
		else throw new exception("Nr de produse trebuie sa fie pozitiv");
	}

	friend istream& operator>>(istream& in, Furnizor& f)
	{
		if (f.nrBucati != nullptr)
			delete[] f.nrBucati;
		cout << "Introduceti numele Furnizorului: ";
		in.getline(f.nume, 20);
		cout << "Introduceti numarul tipurilor de produse furnizate: ";
		in >> f.nrProduseFurnizate;
		f.nrBucati = new int[f.nrProduseFurnizate];
		cout << "Introduceti numarul bucatilor furnizate din fiecare produs: ";
		for (int i = 0; i < f.nrProduseFurnizate; i++)
		{
			cout << "Bucati produs " << i << ": ";
			in >> f.nrBucati[i];
		}
		return in;
	}

	friend ostream& operator<<(ostream& out, const Furnizor& f)
	{
		out << "\n==================================";
		out << "\nNume: " << f.nume;
		out << "\nNumar tipuri de produse furnizate : " << f.nrProduseFurnizate;
		out << "\nNumar bucati furnizate din fiecare produs:";
		for (int i = 0; i < f.nrProduseFurnizate; i++)
			out << f.nrBucati[i] << " ";
		out << "\n==================================";
		return out;
	}

	~Furnizor()
	{
		delete[] nrBucati;
	}
};

//Client: char nume (alocat static),int nrProduseCumparate, float* sumePlatite ( > 0 )(alocat dinamic)
class Client :public IFile {
private:
	char nume[20] = { 'A', 'n', 'o', 'n', 'i', 'm' };
	int nrProduseCumparate = 0; // >= 0
	float* sumePlatite = nullptr; // >= 0
public:

	virtual void afisare()
	{
		cout << "\nFUNCTIE VIRTUALA DE AFISARE, CLASA DE BAZA";
		cout << "\nNume: " << this->nume;
		cout << "\nNumar tipuri de produse cumparate : " << this->nrProduseCumparate;
		cout << "\nSumele platite pentru fiecare produs:";
		for (int i = 0; i < this->nrProduseCumparate; i++)
			cout << this->sumePlatite[i] << " ";
		cout << endl;
	}

	void fromTextFileToBinFile(ifstream& f)
	{
		fstream fout("client.bin", ios::out | ios::binary);
		char nume[20];
		int i = 0;
		while (true)
		{
			int nrProduseCumparate;
			if (i != 0)
				f.ignore();
			f.getline(nume, 20);
			strcpy(this->nume, nume);
			if (f >> nrProduseCumparate)
				{ }
			else
				break;
			if (this->sumePlatite != nullptr)
				delete[] this->sumePlatite;
			int lg = sizeof(this->nume) + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(this->nume, lg);
			fout.write((char*)&nrProduseCumparate, sizeof(int));
			float sumePlatite[15];
			this->sumePlatite = new float[nrProduseCumparate];
			for (int i = 0; i < nrProduseCumparate; i++)
				f >> sumePlatite[i];
			for (int i = 0; i < nrProduseCumparate; i++)
			{
				this->sumePlatite[i] = sumePlatite[i];
				fout.write((char*)&sumePlatite[i], sizeof(float));
			}
			i++;
		}
		fout.close();
	}

	void fromCSVFileToBinFile(ifstream& f)
	{
		fstream fout("clientCSV.bin", ios::out | ios::binary);
		while (true)
		{
			string nrProduse;
			f.getline(this->nume, 20);
			char* nume = new char[strlen(this->nume) + 1];
			strcpy(nume, this->nume);
			if (getline(f, nrProduse, ','))
			{
			}
			else
			{
				break;
			}
			int lg = strlen(nume);
			fout.write((char*)&lg, sizeof(int));
			fout.write(nume, lg);
			lg = nrProduse.length() + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(nrProduse.data(), lg);
			int nrprod = stoi(nrProduse);
			string sume;
			for (int j = 0; j < (nrprod - 1); j++)
			{
				getline(f, sume, ',');
				lg = sume.length() + 1;
				fout.write((char*)&lg, sizeof(int));
				fout.write(sume.data(), lg);
			}
			getline(f, sume, '\n');
			lg = sume.length() + 1;
			fout.write((char*)&lg, sizeof(int));
			fout.write(sume.data(), lg);
			delete[] nume;
		}
		fout.close();
	}

	void writeToBinFile(fstream& f)
	{
		int lg = sizeof(this->nume) + 1;
		f.write((char*)&lg, sizeof(int));
		f.write(this->nume, lg);
		f.write((char*)&this->nrProduseCumparate, sizeof(int));
		for (int i = 0; i < this->nrProduseCumparate; i++)
			f.write((char*)&this->sumePlatite[i], sizeof(float));
	}

	Client(char* nume, int nrProduseCumparate, float* sumePlatite)
	{
		strcpy(this->nume, nume);
		if (nrProduseCumparate > 0 && sumePlatite != nullptr)
		{
			this->nrProduseCumparate = nrProduseCumparate;
			this->sumePlatite = new float[this->nrProduseCumparate];
			for (int i = 0; i < this->nrProduseCumparate; i++)
				if (sumePlatite[i] >= 0)
					this->sumePlatite[i] = sumePlatite[i];
		}
	}

	Client() {};

	Client(const Client& c)
	{
		strcpy(this->nume, c.nume);
		if (c.nrProduseCumparate > 0 && c.sumePlatite != nullptr)
		{
			this->nrProduseCumparate = c.nrProduseCumparate;
			this->sumePlatite = new float[this->nrProduseCumparate];
			for (int i = 0; i < this->nrProduseCumparate; i++)
				if (c.sumePlatite[i] >= 0)
					this->sumePlatite[i] = c.sumePlatite[i];
		}
	}

	Client& operator=(const Client& c)
	{
		if (this != &c)
		{
			strcpy(this->nume, c.nume);
			if (c.nrProduseCumparate > 0 && c.sumePlatite != nullptr)
			{
				this->nrProduseCumparate = c.nrProduseCumparate;
				this->sumePlatite = new float[this->nrProduseCumparate];
				for (int i = 0; i < this->nrProduseCumparate; i++)
					if (c.sumePlatite[i] >= 0)
						this->sumePlatite[i] = c.sumePlatite[i];
			}
			else
			{
				this->sumePlatite = nullptr;
				this->nrProduseCumparate = 0;
			}
		}
		return *this;
	}

	float& operator[](int index)
	{
		if (index >= 0 && index < this->nrProduseCumparate)
			return this->sumePlatite[index];
	}

	Client operator*(float reducere)
	{
		Client rez = *this;
		if ((rez.nrProduseCumparate > 0) && (rez.sumePlatite[rez.nrProduseCumparate - 1] != NULL) && (reducere > 0) &&(reducere < 1))
		{
			rez.sumePlatite[rez.nrProduseCumparate - 1] *= reducere;
		}
		return rez;
	}

	Client& operator--() //pre
	{
		if (this->nrProduseCumparate > 0)
		{
			float sumePlatiteCopie[20] = { 0 };
			this->nrProduseCumparate--;
			for (int i = 0; i < this->nrProduseCumparate; i++)
				sumePlatiteCopie[i] = this->sumePlatite[i];
			delete[] this->sumePlatite;
			this->sumePlatite = new float[this->nrProduseCumparate];
			for (int i = 0; i < this->nrProduseCumparate; i++)
				this->sumePlatite[i] = sumePlatiteCopie[i];
		}
		return *this;
	}

	Client operator--(int x) //post
	{
		Client copie = *this;
		if (this->nrProduseCumparate > 0)
		{
			float sumePlatiteCopie[20] = { 0 };
			this->nrProduseCumparate--;
			for (int i = 0; i < this->nrProduseCumparate; i++)
				sumePlatiteCopie[i] = this->sumePlatite[i];
			delete[] this->sumePlatite;
			this->sumePlatite = new float[this->nrProduseCumparate];
			for (int i = 0; i < this->nrProduseCumparate; i++)
				this->sumePlatite[i] = sumePlatiteCopie[i];
		}
		return copie;
	}

	operator string()
	{
		string nume;
		nume = this->nume;
		return nume;
	}

	bool operator!()
	{
		if(this->nrProduseCumparate != 0)
			return true;
		return false;
	}

	bool operator>(const Client& c)
	{
		if (this->nrProduseCumparate > c.nrProduseCumparate)
			return true;
		return false;
	}

	bool operator==(const Client& c)
	{
		if ((strcmp(this->nume, c.nume) != 0) || (this->nrProduseCumparate != c.nrProduseCumparate))
			return 0;
		for (int i = 0; i < this->nrProduseCumparate; i++)
			if (this->sumePlatite[i] != c.sumePlatite[i])
				return 0;
		return 1;
	}

	char* getNume()
	{
		return this->nume;
	}

	void setNume(char* nume)
	{
		strcpy(this->nume, nume);
	}

	int getNrProduseCumparate()
	{
		return this->nrProduseCumparate;
	}

	void setNrProduseCumparate(int nrProduseCumparate)
	{
		if(nrProduseCumparate > 0)
			this->nrProduseCumparate = nrProduseCumparate;
	}

	float* getSumePlatite()
	{
		return this->sumePlatite;
	}

	void setSumePlatite(float* sumePlatite, int nrProduseCumparate)
	{
		if (this->sumePlatite != nullptr)
		{
			delete[] this->sumePlatite;
		}
		this->nrProduseCumparate = nrProduseCumparate;
		this->sumePlatite = new float[this->nrProduseCumparate];
		if (nrProduseCumparate > 0)
			for (int i = 0; i < this->nrProduseCumparate; i++)
				if (nrProduseCumparate > 0)
					this->sumePlatite[i] = sumePlatite[i];
				else throw new exception("Suma trebuie sa fie pozitiva");
		else throw new exception("Nr de produse trebuie sa fie pozitiv");
	}

	float sumaTotalaPlatita()
	{
		float suma = 0;
		for (int i = 0; i < this->nrProduseCumparate; i++)
			suma += this->sumePlatite[i];
		return suma;
	}

	friend istream& operator>>(istream& in, Client& c)
	{
		if (c.sumePlatite != nullptr)
			delete[] c.sumePlatite;
		cout << "Introduceti numele Clientului: ";
		in.getline(c.nume, 20);
		cout << "Introduceti numarul tipurilor de produse cumparate: ";
		in >> c.nrProduseCumparate;
		c.sumePlatite = new float[c.nrProduseCumparate];
		cout << "Introduceti sumele platite pentru fiecare produs: ";
		for (int i = 0; i < c.nrProduseCumparate; i++)
		{
			cout << "Suma pentru produsul " << i << ": ";
			in >> c.sumePlatite[i];
		}
		return in;
	}

	friend ostream& operator<<(ostream& out, const Client& c)
	{
		out << "\n==================================";
		out << "\nNume: " << c.nume;
		out << "\nNumar tipuri de produse cumparate : " << c.nrProduseCumparate;
		out << "\nSumele platite pentru fiecare produs:";
		for (int i = 0; i < c.nrProduseCumparate; i++)
			out << c.sumePlatite[i] << " ";
		out << "\n==================================";
		return out;
	}

	~Client()
	{
		delete[] sumePlatite;
	}
};

class DetaliiFurnizor :public Furnizor
{
private:
	int nrAngajati = 0;
	string adresa = "N\A";
public:

	void afisare()
	{
		cout << "\nFUNCTIE VIRTUALA DE AFISARE, CLASA DERIVATA";
		cout << "\nNumarul de angajati ai firmei furnizoare: " << this->nrAngajati;
		cout << "\nAdresa sediului firmei furnizoare: " << this->adresa;
		cout << endl;
	}

	DetaliiFurnizor() { }

	DetaliiFurnizor(char* nume, int nrProduseFurnizate, int* nrBucati, int nrAngajati, string adresa) :Furnizor(nume, nrProduseFurnizate, nrBucati)
	{
		this->nrAngajati = nrAngajati;
		this->adresa = adresa;
	}

	DetaliiFurnizor(Furnizor f, int nrAngajati, string adresa) :Furnizor(f)
	{
		this->nrAngajati = nrAngajati;
		this->adresa = adresa;
	}

	DetaliiFurnizor(const DetaliiFurnizor& df) :Furnizor(df)
	{
		this->nrAngajati = df.nrAngajati;
		this->adresa = df.adresa;
	}

	DetaliiFurnizor& operator=(const DetaliiFurnizor& df)
	{
		if (this != &df)
		{
			Furnizor::operator=(df);
			this->nrAngajati = df.nrAngajati;
			this->adresa = df.adresa;
		}
		return *this;
	}

	friend ostream& operator<<(ostream& out, const DetaliiFurnizor& df)
	{
		out << (Furnizor)df;
		out << "\nNumar angajati: " << df.nrAngajati;
		out << "\nAdresa: " << df.adresa;
		return out;
	}
};

class DetaliiClient :public Client
{
private: 
	int varsta = 0;
	string adresa = "N\A";
public:

	void afisare()
	{
		cout << "\nFUNCTIE VIRTUALA DE AFISARE, CLASA DERIVATA";
		cout << "\nVarsta clientului: " << this->varsta;
		cout << "\nAdresa clientului: " << this->adresa;
		cout << endl;
	}

	DetaliiClient() { }

	DetaliiClient(char* nume, int nrProduseCumparate, float* sumePlatite, int varsta, string adresa) :Client(nume, nrProduseCumparate, sumePlatite)
	{
		this->varsta = varsta;
		this->adresa = adresa;
	}

	DetaliiClient(Client c, int varsta, string adresa) :Client(c)
	{
		this->varsta = varsta;
		this->adresa = adresa;
	}

	DetaliiClient(const DetaliiClient& dc) :Client(dc)
	{
		this->varsta = dc.varsta;
		this->adresa = dc.adresa;
	}

	DetaliiClient& operator=(const DetaliiClient& dc)
	{
		if (this != &dc)
		{
			Client::operator=(dc);
			this->varsta = dc.varsta;
			this->adresa = dc.adresa;
		}
		return *this;
	}

	friend ostream& operator<<(ostream& out, const DetaliiClient& dc)
	{
		out << (Client)dc;
		out << "\nVarsta: " << dc.varsta;
		out << "\nAdresa: " << dc.adresa;
		return out;
	}
};

float ProdusMagazin::pretMinim = 500;

int main(int argc, char** argv)
{
	float nucleeGHz[6] = { 2.8, 2.8, 2.8, 2.8, 2.8, 2.8 };
	char modelcpu[20] = { 'I', 'n', 't', 'e', 'l', 'A', '3', '0' };
	Procesor p1(modelcpu, 6, nucleeGHz);
	Procesor p2(4);
	Procesor p3(p1);
	p3.setNucleeGHz(1.85);
	char modelcpu2[20];
	strcpy(modelcpu2, "XeonT50");
	p3.setModel(modelcpu2);
	p2 = p1;
	cout << p1[0] << endl;
	p1 = p1 + 3;
	p1++;
	cout << p1[0];
	cout << int(p1);
	cout << endl;
	if (!p3)
		cout << 1;
	else cout << 0;
	cout << endl;
	if (p1 > p2)
		cout << 1;
	else cout << 0;
	cout << endl;
	if (p1 == p2)
		cout << 1;
	else cout << 0;
	cout << endl;
	cout << p1;
	//Procesor p4(4);
	//cin >> p4;
	//cout << p4;
	cout << endl << endl << endl;


	float steleRecenzii[3] = { 4.5, 3.9, 5 };
	char modeltel[20] = { 'S', 'a', 'm', 's', 'u', 'n', 'g', 'I', '4' };
	Telefon t1(modeltel, 3, steleRecenzii, p2);
	Telefon t2(p1);
	char model3[20] = "TestModel";
	t2.setModel(model3);
	char model[20];
	cout << t1.getModel() << endl;
	cout << t2.getCpu() << endl;
	t2.setCpu(p1);
	Telefon t3 = t2;
	t3.setSteleRecenzii(steleRecenzii, 3);
	float* stele = t3.getSteleRecenzii();
	for (int i = 0; i < t3.getNrRecenzii(); i++)
		cout << stele[i] << " ";
	cout << endl << t3[1];
	t2 = t2 + 4;
	t2++;
	cout << t2 << endl;
	cout << int(t2) << " " << (!t1) << " " << (t1 > t2) << " " << (t2 == t3) << endl;
	//Telefon t4 = t3;
	//cin >> t4;
	//cout << t4;


	cout << endl << endl << endl;


	int nrVanzariLunareAnPrec[12] = { 5, 4, 6, 3, 7, 2, 8, 1, 9, 10, 12, 11 };
	int nrVanzariLunareAnPrec2[12] = { 4, 3, 5, 2, 6, 1, 7, 0, 8, 9, 11, 10 };
	ProdusMagazin prod1(t2, nrVanzariLunareAnPrec, 750);
	ProdusMagazin prod2(prod1);
	ProdusMagazin prod3 = prod2;
	cout << prod3 << endl << endl;
	prod2.setModel(modeltel);
	prod2.setNrVanzariLunareAnPrec(nrVanzariLunareAnPrec2);
	prod2.setPretMinim(200);
	prod2.setPret(400);
	prod2.setTel(t1);
	cout << prod2.getModel() << " " << prod2.getPret() << " " << prod2.getPretMinim() << " " << prod2.getTel() << endl;
	int* vanzari = prod2.getNrVanzariLunareAnPrec();
	for (int i = 0; i < 12; i++)
		cout << vanzari[i] << " ";
	cout << endl << prod1[1] << endl;
	prod1 = prod1.operator+(50.7);
	prod1++;
	cout << prod1 << endl; //s-a modificat pretul, in urma operatorilor + si ++
	cout << int(prod1) << " " << (!prod1) << " " << (prod2 > prod1) << " " << (prod2 == prod3) << endl << endl;
	//cin >> prod3;
	//cout << prod3;


	cout << endl << endl << endl;



	char numefurn[20] = "TelefoaneINC.";
	char numefurn2[20] = "BestPhoneCompany";
	int nrBucati[20] = { 10, 12, 15, 21 };
	int nrBucati2[20] = { 5, 6, 7 };
	Furnizor f1(numefurn, 4, nrBucati);
	Furnizor f2(f1);
	Furnizor f3 = f2;
	Furnizor f4;
	cout << f3 << endl << f4 << endl;
	f3.setNrBucati(nrBucati2, 3);
	f3.setNume(numefurn2);
	cout << f3.getNume() << ", " << f3.getNrProduseFurnizate() << ", ";
	int* produse = f3.getNrBucati();
	for (int i = 0; i < f3.getNrProduseFurnizate(); i++)
		cout << produse[i] << " ";
	cout << endl << f1[2] << endl;
	f3 = f3 + 69;
	f3 = f3 + 40;
	f3--;
	cout << f3 << endl; //am adaugat inregistrarile 69 si 40, apoi a fost stearsa intregistrarea 40
	cout << string(f3) << " " << (!f4) << " " << (f2 > f3) << " " << (f1 == f2) << endl << endl;
	//cin >> f1;
	//cout << f1;


	cout << endl << endl << endl;


	char numeClient[20] = "Marius_Bogdan";
	char numeClient2[20] = "Ianis_Klaunus";
	float sumeplatite[20] = { 5000, 1200 };
	float sumeplatite2[20] = { 900, 1200, 1000 };
	Client c1(numeClient, 2, sumeplatite);
	Client c2(c1);
	Client c3 = c2;
	Client c4;
	cout << c3 << endl << c4 << endl;
	c3.setSumePlatite(sumeplatite2, 3);
	c3.setNume(numeClient2);
	cout << c3.getNume() << ", " << c3.getNrProduseCumparate() << ", ";
	float* sume = c3.getSumePlatite();
	for (int i = 0; i < c3.getNrProduseCumparate(); i++)
		cout << sume[i] << " ";
	cout << endl << c1[1] << endl;
	c3--;
	c3 = c3 * 0.85; //reducere de 15% la ultimul element din vector
	cout << c3 << endl;
	cout << string(c3) << " " << (!c4) << " " << (c2 > c3) << " " << (c1 == c2) << endl << endl;
	//cin >> c1;
	//cout << c1;

		/*                     PENTRU FAZA 3                        */
	DetaliiClient dc1(c1, 20, "Strada Soarelui nr 40");
	DetaliiFurnizor df1(f1, 13, "Strada Industriei nr 11");
	Client* cdc = &dc1;
	Furnizor* cdf = &df1;
	cdc->afisare();
	Client(dc1).afisare();
	cdf->afisare();
	Furnizor(df1).afisare();

	//vector la clasa de baza a ierarhiei
	Client* c1ptr = &c1;
	Client* c2ptr = &c2;
	Client* c3ptr = &c3;

	Client* vectClienti[4] = { c1ptr, cdc, c2ptr, c3ptr };
	for (int i = 0; i < 4; i++)
	{
		cout<<"\nClientul nr "<<i+1<<" a cumparat "<<vectClienti[i]->getNrProduseCumparate() <<" produse.\n";
		vectClienti[i]->afisare();
	}

	cout << "\n========VECTOR STL========\n";
	vector<Procesor> vect_procesor;
	vect_procesor.push_back(p1);
	vect_procesor.push_back(p2);
	vect_procesor.push_back(p3);
	for (auto i = vect_procesor.begin(); i != vect_procesor.end(); i++)
		cout << *i;
	Procesor p4(4);

	/*cin >> p4;
	vect_procesor.push_back(p4);
	int dim = vect_procesor.size();
	cout << vect_procesor[dim - 1];*/

	cout << "\n========SET STL========\n";
	set<Telefon> set_tel;
	set_tel.insert(t1);
	set_tel.insert(t2);
	set_tel.insert(t3);
	set<Telefon>::reverse_iterator itt;
	cout << "\nTelefoanele din set sunt modelele: ";
	int ct = 1;
	Telefon copTel(t1);
	for (itt = set_tel.rbegin(); itt != set_tel.rend(); itt++)
	{
		copTel = *itt;
		if (copTel.getModel() != nullptr)
			cout << "\nTelefon " << ct << ": " << copTel.getModel();
		else
			cout << "\nTelefonul " << ct << " nu are nume de model.";
		ct++;
	}

	cout << "\n========LIST STL========\n";
	list<Furnizor> list_furn;
	list_furn.push_back(f1);
	list_furn.push_back(f2);
	list_furn.push_front(f3);
	list<Furnizor>::iterator itf;
	for (itf = list_furn.begin(); itf != list_furn.end(); itf++)
	{
		cout << *itf;
	}

	cout << "\n========MAP STL========\n";
	map<string, ProdusMagazin> map_prod;
	map_prod.insert({ "Produs 1", prod1 });
	map_prod.insert({ "Produs 2", prod2 });
	map_prod.insert({ "Produs 3", prod3 });
	map<string, ProdusMagazin> ::iterator itp;
	itp = map_prod.begin();
	while (itp != map_prod.end())
	{
		cout << "\nCheie: " << itp->first;
		cout << "\nValoare: " << itp->second;
		itp++;
	}

	/*                     PENTRU FAZA 2                        */

	//meniu:

	if (argc > 3) return 0;
	int x;
	int y;
	int z;
	int l;
	cout << "\nBun venit!";
	while (true)
	{
		cout << "\nIntroduceti un numar pentru a folosi o functie:";
		if (argv[1]) //aceasta functie este disponibila doar daca exista fisiere trimise ca argumente
			cout << "\n1. Citirea datelor din fisierele text/csv introduse si scrierea lor in fisiere binare";
		cout << "\n2. Scrierea datelor inregistrate in program in fisiere binare";

				//Cele 3 rapoarte:
		cout << "\n3. Lista Furnizorilor inregistrati in program si scrierea numelor intr-un fisier text";
		//doar Clienti care au fost definiti deja in cod mai sus
		cout << "\n4. Produsele ordonate crescator dupa pret";
		//produse declarate deja in cod mai sus
		cout << "\n5. Clientii ordonati descrescator dupa sumele totale platite";
		//Furnizori declarati deja in cod mai sus
		cout << "\n0. Inchideti meniul si iesiti din program.\n";
		cin >> x;
		switch (x)
		{
		case 1:
		{
			if (argv[1] && argv[2])
			{
				cout << "\nAlegeti un fisier (1 sau 2)\n";
				cin >> y;
				if (y == 1)
				{
					cout << "\nFisierul este .txt (1) sau .csv (2) ?\n";
					cin >> l;
					if (l == 1)
					{
						cout << "\nCe tip de date contine fisierul?";
						cout << "\n1 - Procesoare, 2 - Furnizori, 3 - Clienti\n";
						cin >> z;
						switch (z)
						{
						case 1:
						{
							Procesor paux(8);
							ifstream fintrare(argv[1]);
							cout << "\nSe incearca deschiderea fisierului...\n";
							if (fintrare.is_open())
							{
								cout << "\nFisierul a fost deschis cu succes.\n";
								paux.fromTextFileToBinFile(fintrare);
								fintrare.close();
							}
							break;
						}
						case 2:
						{
							Furnizor faux;
							ifstream fintrare(argv[1]);
							cout << "\nSe incearca deschiderea fisierului...\n";
							if (fintrare.is_open())
							{
								cout << "\nFisierul a fost deschis cu succes.\n";
								faux.fromTextFileToBinFile(fintrare);
								fintrare.close();
							}
							break;
						}
						case 3:
						{
							Client caux;
							ifstream fintrare(argv[1]);
							cout << "\nSe incearca deschiderea fisierului...\n";
							if (fintrare.is_open())
							{
								cout << "\nFisierul a fost deschis cu succes.\n";
								caux.fromTextFileToBinFile(fintrare);
								fintrare.close();
							}
							break;
						}
						}
					}
					else
					{
						cout << "\nCe tip de date contine fisierul?";
						cout << "\n1 - Procesoare, 2 - Furnizori, 3 - Clienti\n";
						cin >> z;
						switch (z)
						{
						case 1:
						{
							Procesor paux(8);
							ifstream fintrare(argv[1]);
							cout << "\nSe incearca deschiderea fisierului...\n";
							if (fintrare.is_open())
							{
								cout << "\nFisierul a fost deschis cu succes.\n";
								paux.fromCSVFileToBinFile(fintrare);
								fintrare.close();
							}
							break;
						}
						case 2:
						{
							Furnizor faux;
							ifstream fintrare(argv[1]);
							cout << "\nSe incearca deschiderea fisierului...\n";
							if (fintrare.is_open())
							{
								cout << "\nFisierul a fost deschis cu succes.\n";
								faux.fromCSVFileToBinFile(fintrare);
								fintrare.close();
							}
							break;
						}
						case 3:
						{
							Client caux;
							ifstream fintrare(argv[1]);
							cout << "\nSe incearca deschiderea fisierului...\n";
							if (fintrare.is_open())
							{
								cout << "\nFisierul a fost deschis cu succes.\n";
								caux.fromCSVFileToBinFile(fintrare);
								fintrare.close();
							}
							break;
						}
						}
					}
				}
				else if (y == 2)
				{
				cout << "\nFisierul este .txt (1) sau .csv (2) ?\n";
				cin >> l;
				if (l == 1)
				{
					cout << "\nCe tip de date contine fisierul?";
					cout << "\n1 - Procesoare, 2 - Furnizori, 3 - Clienti\n";
					cin >> z;
					switch (z)
					{
					case 1:
					{
						Procesor paux(8);
						ifstream fintrare(argv[2]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							paux.fromTextFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					case 2:
					{
						Furnizor faux;
						ifstream fintrare(argv[2]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							faux.fromTextFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					case 3:
					{
						Client caux;
						ifstream fintrare(argv[2]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							caux.fromTextFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					}
				}
				else
				{
					cout << "\nCe tip de date contine fisierul?";
					cout << "\n1 - Procesoare, 2 - Furnizori, 3 - Clienti\n";
					cin >> z;
					switch (z)
					{
					case 1:
					{
						Procesor paux(8);
						ifstream fintrare(argv[2]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							paux.fromCSVFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					case 2:
					{
						Furnizor faux;
						ifstream fintrare(argv[2]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							faux.fromCSVFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					case 3:
					{
						Client caux;
						ifstream fintrare(argv[2]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							caux.fromCSVFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					}
				}
				}
			}
			else if (argv[1])
			{
				cout << "\nFisierul este .txt (1) sau .csv (2) ?\n";
				cin >> l;
				if (l == 1)
				{
					cout << "\nCe tip de date contine fisierul?";
					cout << "\n1 - Procesoare, 2 - Furnizori, 3 - Clienti\n";
					cin >> z;
					switch (z)
					{
					case 1:
					{
						Procesor paux(8);
						ifstream fintrare(argv[1]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							paux.fromTextFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					case 2:
					{
						Furnizor faux;
						ifstream fintrare(argv[1]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							faux.fromTextFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					case 3:
					{
						Client caux;
						ifstream fintrare(argv[1]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							caux.fromTextFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					}
				}
				else
				{
					cout << "\nCe tip de date contine fisierul?";
					cout << "\n1 - Procesoare, 2 - Furnizori, 3 - Clienti\n";
					cin >> z;
					switch (z)
					{
					case 1:
					{
						Procesor paux(8);
						ifstream fintrare(argv[1]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							paux.fromCSVFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					case 2:
					{
						Furnizor faux;
						ifstream fintrare(argv[1]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							faux.fromCSVFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					case 3:
					{
						Client caux;
						ifstream fintrare(argv[1]);
						cout << "\nSe incearca deschiderea fisierului...\n";
						if (fintrare.is_open())
						{
							cout << "\nFisierul a fost deschis cu succes.\n";
							caux.fromCSVFileToBinFile(fintrare);
							fintrare.close();
						}
						break;
					}
					}
				}
			}
			else cout << "\nERR! Nu exista fisier!";
			break;
		}

			case 2:
			{
				fstream fprocesoare("date_procesoare.bin", ios::out | ios::binary);
				if (fprocesoare.is_open())
				{
					p1.writeToBinFile(fprocesoare);
					p2.writeToBinFile(fprocesoare);
					p3.writeToBinFile(fprocesoare);
					fprocesoare.close();
					cout << "\nProcesoare - succes!\n";
				}

				fstream ftelefoane("date_telefoane.bin", ios::out | ios::binary);
				if (ftelefoane.is_open())
				{
					t1.writeToBinFile(ftelefoane);
					t2.writeToBinFile(ftelefoane);
					t3.writeToBinFile(ftelefoane);
					ftelefoane.close();
					cout << "\nTelefoane - succes!\n";
				}

				fstream fproduse("date_produse.bin", ios::out | ios::binary);
				if (fproduse.is_open())
				{
					prod1.writeToBinFile(fproduse);
					prod2.writeToBinFile(fproduse);
					prod3.writeToBinFile(fproduse);
					fproduse.close();
					cout << "\nProduse - succes!\n";
				}

				fstream fFurnizori("date_Furnizori.bin", ios::out | ios::binary);
				if (fFurnizori.is_open())
				{
					f1.writeToBinFile(fFurnizori);
					f2.writeToBinFile(fFurnizori);
					f3.writeToBinFile(fFurnizori);
					f4.writeToBinFile(fFurnizori);
					fFurnizori.close();
					cout << "\nFurnizori - succes!\n";
				}

				fstream fClienti("date_Clienti.bin", ios::out | ios::binary);
				if (fClienti.is_open())
				{
					c1.writeToBinFile(fClienti);
					c2.writeToBinFile(fClienti);
					c3.writeToBinFile(fClienti);
					c4.writeToBinFile(fClienti);
					fClienti.close();
					cout << "\nClienti - succes!\n";
				}

				break;
			}

			case 3:
			{
				ofstream f_nume_Furnizori("NumeFurnizori.txt");
				cout << f1.getNume() << endl;
				cout << f2.getNume() << endl;
				cout << f3.getNume() << endl;
				cout << f4.getNume() << endl;
				if (f_nume_Furnizori.is_open())
				{
					f_nume_Furnizori << f1.getNume() << endl;
					f_nume_Furnizori << f2.getNume() << endl;
					f_nume_Furnizori << f3.getNume() << endl;
					f_nume_Furnizori << f4.getNume() << endl;
					f_nume_Furnizori.close();
					cout << "\nSucces!\n";
				}
				break;
			}

			case 4:
			{
				ProdusMagazin prod1cop(prod1);
				ProdusMagazin prod2cop(prod2);
				ProdusMagazin prod3cop(prod3);
				ProdusMagazin pretSort[] = { prod1cop, prod2cop, prod3cop };
				ProdusMagazin aux(prod1cop);
				int nrProd = 3;
				for (int i = 0; i < nrProd; i++)
					for (int j = i + 1; j < nrProd; j++)
						if (pretSort[i] > pretSort[j])
						{
							aux = pretSort[i];
							pretSort[i] = pretSort[j];
							pretSort[j] = aux;
						}
				ofstream f_preturi_produse("PreturiProduse.txt");
				cout << pretSort[0].getModel() << " " << pretSort[0].getPret() << endl;
				cout << pretSort[1].getModel() << " " << pretSort[1].getPret() << endl;
				cout << pretSort[2].getModel() << " " << pretSort[2].getPret() << endl;
				if (f_preturi_produse.is_open())
				{
					f_preturi_produse << pretSort[0].getModel() << " " << pretSort[0].getPret() << endl;
					f_preturi_produse << pretSort[1].getModel() << " " << pretSort[1].getPret() << endl;
					f_preturi_produse << pretSort[2].getModel() << " " << pretSort[2].getPret() << endl;
					f_preturi_produse.close();
					cout << "\nSucces!\n";
				}
				break;
			}

			case 5:
			{
				Client c1cop(c1);
				Client c2cop(c2);
				Client c3cop(c3);
				Client c4cop(c4);
				Client sumaDesc[] = { c1cop, c2cop, c3cop, c4cop };
				Client aux;
				int nrClienti = 4;
				for (int i = 0; i < nrClienti; i++)
					for (int j = i + 1; j < nrClienti; j++)
						if (sumaDesc[i].sumaTotalaPlatita() < sumaDesc[j].sumaTotalaPlatita())
						{
							aux = sumaDesc[i];
							sumaDesc[i] = sumaDesc[j];
							sumaDesc[j] = aux;
						}
				ofstream f_sume_Clienti("SumeClienti.txt");
				cout << sumaDesc[0].getNume() << " " << sumaDesc[0].sumaTotalaPlatita() << endl;
				cout << sumaDesc[1].getNume() << " " << sumaDesc[1].sumaTotalaPlatita() << endl;
				cout << sumaDesc[2].getNume() << " " << sumaDesc[2].sumaTotalaPlatita() << endl;
				cout << sumaDesc[3].getNume() << " " << sumaDesc[3].sumaTotalaPlatita() << endl;
				if (f_sume_Clienti.is_open())
				{
					f_sume_Clienti << sumaDesc[0].getNume() << " " << sumaDesc[0].sumaTotalaPlatita() << endl;
					f_sume_Clienti << sumaDesc[1].getNume() << " " << sumaDesc[1].sumaTotalaPlatita() << endl;
					f_sume_Clienti << sumaDesc[2].getNume() << " " << sumaDesc[2].sumaTotalaPlatita() << endl;
					f_sume_Clienti << sumaDesc[3].getNume() << " " << sumaDesc[3].sumaTotalaPlatita() << endl;
					f_sume_Clienti.close();
					cout << "\nSucces!\n";
				}
				break;
			}

			case 0:
			{
				cout << "\n\nLa revedere!\n\n";
				exit(0);
				break;
			}

			}
		}
		return 0;
}