# Processing-images

	Programul a fost realizat astfel:
		- procesul cu rank-ul 0 realizeaza citirea imaginii de input
		- in cazul in care exista mai multe procese, procesul 0 realizeaza impartirea matricei initiale si trimite celorlalte procese 3 informatii:
			- pgm- daca poza este pgm sau nu
			- heightP - inaltimea matricei pe care o va avea matricea
			- width - latimea matricei
				Dupa ce aceste informtii sunt trimise, procesul 0 mai trimite in functie de caz,
			pentru fiecare filtru, fiecare bucata de matrice corespunzatoare fiecarui proces,
			dupa caz. (prima bucata de matrice ii revine procesului 1, a doua procesului 2, .., si ultima ii ramane procesului 0);
				Dupa ce fiecare proces are bucata sa de matrice, se fac operatiile corespunzatoare fiecarui filtru.
				Dupa ce procesele si-au filtrat bucata de matrice, o trimit modificata procesului 0, acesta din urma
			construinf matricea finala din toate bucatile primite.

	Scalabilitate: (calculator cu 12 core-uri)

	Pentru imaginile de dimensiune mare de poate observa cel mai bine ca programul scaleaza corespunzator:

		De exemplu imaginea de input 'baby-yoda.pnm' prin aplicarea filtrelor: 'blur smooth sharpen emboss mean blur smooth sharpen emboss mean'

		Se obtin timpii(real): -> tip PNM
			- pentru 1 proces - 4.715s;
			- pentru 2 procese - 3.299s;
			- pentru 3 procese - 2.791s;
			- pentru 4 procese - 2.557s;
			- pentru 5 procese - 2.427s;
			- pentru 6 procese - 2.369s;
			- pentru 7 procese - 2.648s;
			- pentru 8 procese - 2.737s;
			- pentru 9 procese - 2.879s;
			- pentru 10 procese - 2.902s;
			- pentru 11 procese - 3.033s;
			- pentru 12 procese - 3.146s;


		Imaginea de input 'rorschach.pgm' prin aplicarea filtrelor: 'blur smooth sharpen emboss mean blur smooth sharpen emboss mean'

		Se obtin timpii(real): -> tip PGM (dimensiune 3853 2000)
			- pentru 1 proces - 5.748s;
			- pentru 2 procese - 1.752s;
			- pentru 3 procese - 1.559s;
			- pentru 4 procese - 1.495s;
			- pentru 5 procese - 1.414s;
			- pentru 6 procese - 1.366s;
			- pentru 7 procese - 1.414s;
			- pentru 8 procese - 1.681s;
			- pentru 9 procese - 1.571s;
			- pentru 10 procese - 1.723s;
			- pentru 11 procese - 1.819s;
			- pentru 12 procese - 1.851s;

				Se observa ca programul scaleaza bine pana la 6 core-uri, deoarece doar 6 se executa in paralel, ceea ce imbunatateste viteza de executie.
				Crescand numarul de procese cu mai mult de 6, se observa o crestere treptata, deoarece nici dimensiunea imaginii nu este destul de mare, astfel
			incat un numar de procese ridicat sa imbunatateasca substantial executia programului.

		Imaginea de input 'landscape.pgm' prin aplicarea filtrelor: 'blur smooth sharpen emboss mean blur smooth sharpen emboss mean'

		Se obtin timpii(real): -> tip PGM (dimensiune 768 432)
			- pentru 1 proces - 0.451s;
			- pentru 2 procese - 0.443s;
			- pentru 3 procese - 0.416s;
			- pentru 4 procese - 0.442s;
			- pentru 5 procese - 0.442s;
			- pentru 6 procese - 0.462s;
			- pentru 7 procese - 0.502s;
			- pentru 8 procese - 0.499s;
			- pentru 9 procese - 0.526s;
			- pentru 10 procese - 0.564s;
			- pentru 11 procese - 0.599s;
			- pentru 12 procese - 0.620s;

				Se observa ca in acest caz, imaginea fiind de o dimensiune mica, programul nu scaleaza foarte bine.
